#include "audioMixer.h"
#include "common.h"
#include "intervalTimer.h"
#include <alloca.h> // needed for mixer
#include <alsa/asoundlib.h>
#include <limits.h>
#include <pthread.h>
#include <stdbool.h>

static snd_pcm_t *handle;

#define SAMPLE_RATE 44100
#define NUM_CHANNELS 1
#define SAMPLE_SIZE (sizeof(short)) // bytes per sample

static unsigned long playbackBufferSize = 0;
static short *playbackBuffer = NULL;

// Currently active (waiting to be played) sound bites
#define MAX_SOUND_BITES 30
typedef struct{
	// A pointer to a previously allocated sound bite (wavedata_t struct).
	// Note that many different sound-bite slots could share the same pointer
	// (overlapping cymbal crashes, for example)
	wavedata_t *pSound;

	// The offset into the pData of pSound. Indicates how much of the
	// sound has already been played (and hence where to start playing next).
	int location;
} playbackSound_t;

static playbackSound_t soundBites[MAX_SOUND_BITES]; // 30 possible sounds at once
static void AudioMixer_queueSound(wavedata_t *pSound);
static void AudioMixer_setVolume(int newVolume);

// Playback threading
static void *playbackThread(void *arg);
static _Bool stopping = false;
static pthread_t playbackThreadId;
static pthread_mutex_t audioMutex = PTHREAD_MUTEX_INITIALIZER;
static int volume = 0;

static char *filenames[NUMBER_OF_SOUNDS] = {
	"beatbox-wav-files/100054__menegass__gui-drum-ch.wav",			// hi-hat
	"beatbox-wav-files/100058__menegass__gui-drum-snare-hard.wav",	// snare drum
	"beatbox-wav-files/100051__menegass__gui-drum-bd-hard.wav",		// bass drum
	"beatbox-wav-files/piano_G_Sharp.wav",							// piano G#
	"beatbox-wav-files/piano_A.wav",								// piano A
	"beatbox-wav-files/piano_B.wav",								// piano B
	"beatbox-wav-files/piano_C_Sharp.wav"};							// piano C#			

static wavedata_t allSounds[NUMBER_OF_SOUNDS];

static void AudioMixer_readWaveFileIntoMemory(char *fileName, wavedata_t *pSound);

void AudioMixer_playNote(int soundIndex){
	for (int i = 0; i < NUMBER_OF_SOUNDS; i++){
		if (soundIndex & (1 << i)){		// check each bit to see if its queued to play
			AudioMixer_queueSound(&allSounds[i]); // bit corresponds to a sound index, so queue it
		}
	}
}

static void readAllSoundsIntoMemory(){
	for (int i = 0; i < NUMBER_OF_SOUNDS; i++){
		AudioMixer_readWaveFileIntoMemory(filenames[i], &allSounds[i]);		// read all sounds into memory, file index will correspond to allSound[] index
	}
}

void AudioMixer_init(void){
	AudioMixer_setVolume(DEFAULT_VOLUME);

	// Initialize the currently active sound-bites being played
	for (int i = 0; i < MAX_SOUND_BITES; i++){
		soundBites[i].pSound = NULL; // pSound NULL and location 0 means open slot
		soundBites[i].location = 0;
	}

	// Open the PCM output
	int err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (err < 0){
		printf("Playback open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	// Configure parameters of PCM output
	err = snd_pcm_set_params(handle, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED, NUM_CHANNELS, SAMPLE_RATE,
							 1,		 // Allow software resampling
							 50000); // 0.05 seconds per buffer
	if (err < 0){
		printf("Playback open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	// Allocate this software's playback buffer to be the same size as the
	// the hardware's playback buffers for efficient data transfers.
	// ..get info on the hardware buffers:
	unsigned long unusedBufferSize = 0;
	snd_pcm_get_params(handle, &unusedBufferSize, &playbackBufferSize);
	// ..allocate playback buffer:
	playbackBuffer = malloc(playbackBufferSize * sizeof(*playbackBuffer));

	// Launch playback thread:
	readAllSoundsIntoMemory();	//must be done before playback thread is launched
	pthread_create(&playbackThreadId, NULL, playbackThread, NULL);
}

// Client code must call AudioMixer_freeWaveFileData to free dynamically allocated data.
static void AudioMixer_readWaveFileIntoMemory(char *fileName, wavedata_t *pSound){
	assert(pSound); // assert not null

	// The PCM data in a wave file starts after the header:
	const int PCM_DATA_OFFSET = 44;

	// Open the wave file
	FILE *file = fopen(fileName, "r");
	if (file == NULL){
		fprintf(stderr, "ERROR: Unable to open file %s.\n", fileName);
		exit(EXIT_FAILURE);
	}

	// Get file size
	fseek(file, 0, SEEK_END);
	int sizeInBytes = ftell(file) - PCM_DATA_OFFSET;
	pSound->numSamples = sizeInBytes / SAMPLE_SIZE;

	// Search to the start of the data in the file
	fseek(file, PCM_DATA_OFFSET, SEEK_SET);

	// Allocate space to hold all PCM data
	pSound->pData = malloc(sizeInBytes);
	if (pSound->pData == 0){
		fprintf(stderr, "ERROR: Unable to allocate %d bytes for file %s.\n", sizeInBytes, fileName);
		exit(EXIT_FAILURE);
	}

	// Read PCM data from wave file into memory
	int samplesRead = fread(pSound->pData, SAMPLE_SIZE, pSound->numSamples, file);
	if (samplesRead != pSound->numSamples){
		fprintf(stderr, "ERROR: Unable to read %d samples from file %s (read %d).\n", pSound->numSamples, fileName, samplesRead);
		exit(EXIT_FAILURE);
	}
	fclose(file);
}

static void AudioMixer_freeWaveFileData(wavedata_t *pSound){
	pSound->numSamples = 0;
	free(pSound->pData);
	pSound->pData = NULL;
}

static void AudioMixer_queueSound(wavedata_t *pSound){
	// Ensure we are only being asked to play "good" sounds:
	assert(pSound->numSamples > 0);
	assert(pSound->pData);

	// Insert the sound by searching for an empty sound bite spot

	pthread_mutex_lock(&audioMutex);
	for (int i = 0; i < MAX_SOUND_BITES; i++){
		if (soundBites[i].pSound == NULL){
			soundBites[i].pSound = pSound;
			soundBites[i].location = 0;
			pthread_mutex_unlock(&audioMutex);
			return;
		}
	}
	pthread_mutex_unlock(&audioMutex);
	printf("ERROR: No free sound bites available.\n");
	return;
}

void AudioMixer_cleanup(void){
	printf("Stopping audio...\n");

	// Stop the PCM generation thread
	stopping = true;
	pthread_join(playbackThreadId, NULL);

	// Shutdown the PCM output, allowing any pending sound to play out (drain)
	snd_pcm_drain(handle);
	snd_pcm_close(handle);

	// Free playback buffer
	// (note that any wave files read into wavedata_t records must be freed
	//  in addition to this by calling AudioMixer_freeWaveFileData() on that struct.)
	free(playbackBuffer);
	playbackBuffer = NULL;

	for (int i = 0; i < NUMBER_OF_SOUNDS; i++){
		AudioMixer_freeWaveFileData(&allSounds[i]);
	}

	printf("Done stopping audio...\n");
	fflush(stdout);
}

int AudioMixer_getVolume(){
	// Return the cached volume; good enough unless someone is changing
	// the volume through other means and the cached value is out of date.
	return volume;
}

// Function copied from:
// http://stackoverflow.com/questions/6787318/set-alsa-master-volume-from-c-code
// Written by user "trenki".
static void AudioMixer_setVolume(int newVolume){
	// Ensure volume is reasonable; If so, cache it for later getVolume() calls.
	if (newVolume < 0){
		newVolume = 0;
	}
	else if (newVolume > 100){
		newVolume = 100;
	}
	volume = newVolume;

	long min, max;
	snd_mixer_t *mixerHandle;
	snd_mixer_selem_id_t *sid;
	const char *card = "default";
	// const char *selem_name = "PCM";	// For ZEN cape
	const char *selem_name = "Speaker"; // For USB Audio

	snd_mixer_open(&mixerHandle, 0);
	snd_mixer_attach(mixerHandle, card);
	snd_mixer_selem_register(mixerHandle, NULL, NULL);
	snd_mixer_load(mixerHandle);

	snd_mixer_selem_id_alloca(&sid);
	snd_mixer_selem_id_set_index(sid, 0);
	snd_mixer_selem_id_set_name(sid, selem_name);
	snd_mixer_elem_t *elem = snd_mixer_find_selem(mixerHandle, sid);

	snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
	snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);

	snd_mixer_close(mixerHandle);
}

void AudioMixer_incrementVolume(){
	AudioMixer_setVolume(volume + 5);
}

void AudioMixer_decrementVolume(){
	AudioMixer_setVolume(volume - 5);
}

// Fill the buff array with new PCM values to output.
//    buff: buffer to fill with new PCM data from sound bites.
//    size: the number of *values* to store into buff
static void fillPlaybackBuffer(short *buffer, int bufferSize){
	memset(buffer, 0, bufferSize * sizeof(*buffer));

	pthread_mutex_lock(&audioMutex);
	for (int i = 0; i < MAX_SOUND_BITES; i++){
		if (soundBites[i].pSound){
			// compiler will optimize this, but this is human readable code
			int offset = soundBites[i].location;
			int totalSamples = soundBites[i].pSound->numSamples;
			int samplesLeft = totalSamples - offset;

			// 2 bounds on loop: 1) samplesLeft, 2) playbackBufferSize
			int loopBound = samplesLeft <= bufferSize ? samplesLeft : bufferSize; // if equal, then samplesLeft has priority

			for (int j = 0; j < loopBound; j++, offset++){
				int currentSample = (int)soundBites[i].pSound->pData[offset] + (int)playbackBuffer[j];
				
				if (currentSample < SHRT_MIN){
					currentSample = SHRT_MIN;
				}

				if (currentSample > SHRT_MAX){
					currentSample = SHRT_MAX;
				}
				playbackBuffer[j] = (short)currentSample;
			}

			soundBites[i].location = offset + 1; // want to get the next sample next fill. If loopBound == samplesLeft,
												 // then location is set to 0 regardless, therefore no worry of overflow
			if (loopBound == samplesLeft){
				soundBites[i].pSound = 0; // set pointer to null and location to 0 if we are done this bite
			}
		}
	}
	pthread_mutex_unlock(&audioMutex);
	Interval_markInterval(0);
}

static void *playbackThread(void *_arg){
	while (!stopping){
		// Generate next block of audio
		fillPlaybackBuffer(playbackBuffer, playbackBufferSize);

		// Output the audio
		snd_pcm_sframes_t frames = snd_pcm_writei(handle, playbackBuffer, playbackBufferSize);

		// Check for (and handle) possible error conditions on output
		if (frames < 0){
			fprintf(stderr, "AudioMixer: writei() returned %li\n", frames);
			frames = snd_pcm_recover(handle, frames, 1);
		}
		if (frames < 0){
			fprintf(stderr, "ERROR: Failed writing audio with snd_pcm_writei(): %li\n", frames);
			exit(EXIT_FAILURE);
		}
		if (frames > 0 && frames < playbackBufferSize){
			printf("Short write (expected %li, wrote %li)\n", playbackBufferSize, frames);
		}
	}
	return NULL;
}