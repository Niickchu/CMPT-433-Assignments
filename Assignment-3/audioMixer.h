// Playback sounds in real time, allowing multiple simultaneous wave files
// to be mixed together and played without jitter.
#ifndef AUDIO_MIXER_H
#define AUDIO_MIXER_H

typedef struct {		//some audio file in memory
	int numSamples;		//number of samples in memory
	short *pData;		//pointer to start of data
} wavedata_t;

#define AUDIOMIXER_MAX_VOLUME 100
#define DEFAULT_VOLUME 80
#define NUMBER_OF_SOUNDS 7

// init() must be called before any other functions, to initialize the audio mixer
// cleanup() must be called last to stop playback threads and free memory.
void AudioMixer_init(void);
void AudioMixer_cleanup(void);

// HEX VALUE   SOUND    INT VALUE		// play a sound from the allsounds array located in audioMixer.c
// 0000 0000 nothing     = 0			// the sounds are summarized in the table to the left
// 0000 0001 high hat    = 1			// you may play multiple sounds at once by adding the values together
// 0000 0010 snare       = 2			// and passing the sum to this function
// 0000 0100 bass        = 4			// a value of 1 in the bitmask/index of a sound will play that sound
// 0000 1000 piano G#    = 8
// 0001 0000 piano A     = 16
// 0010 0000 piano B     = 32
// 0100 0000 piano C#    = 64
void AudioMixer_playNote(int soundIndex);


void AudioMixer_incrementVolume();		// increases the volume by 5%
void AudioMixer_decrementVolume();		// decreases the volume by 5%
int AudioMixer_getVolume();			


#endif
