#include "beatGeneration.h"
#include "audioMixer.h"
#include "intervalTimer.h"
#include "common.h"
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>

typedef struct {
    int currentBPM;         //benign race case?
    int currentBeat;
} beat_t;

static bool isRunning = true;
static beat_t beatAttributes;
static pthread_t playBeatID;
static pthread_mutex_t beatMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t bpmMutex = PTHREAD_MUTEX_INITIALIZER;

static void setBPM(int inputBPM);
static void *playBeat();

//HEX VALUE   SOUND    INT VALUE
//0000 0000 nothing     = 0
//0000 0001 high hat    = 1           // To add a sound to a specific eighth note in a beat add the 
//0000 0010 snare       = 2           // value of the sound to the index of that eighth note
//0000 0100 bass        = 4           // the values corresponding to each sound is summarized
//0000 1000 piano G#    = 8           // to the left. There is a maximum of 32 sounds that can
//0001 0000 piano A     = 16          // be added to the system. You must first add the wav file
//0010 0000 piano B     = 32          // to the audioMixer.c file, ensuring that the index of that
//0100 0000 piano C#    = 64          // file in the array corresponds to the bit position of the value of the sound here.
static int allBeats [NUMBER_OF_BEATS][EIGHTH_NOTES_PER_BAR*2] = {
    {0, 0, 0, 0, 0, 0, 0, 0,            0, 0, 0, 0, 0, 0, 0, 0},            //off -
    {5, 1, 3, 1, 5, 1, 3, 1,            5, 1, 3, 1, 5, 1, 3, 1},            //classic rock beat
    {39, 17, 8, 64, 33, 17, 8, 64,      39, 17, 8, 64, 33, 17, 8, 0}        //custom beat - Money so Big by Yeat
};

void beatGen_init(){
    beatAttributes.currentBPM = DEFAULT_BPM;
    beatAttributes.currentBeat = DEFAULT_BEAT;
    isRunning = true;
    pthread_create(&playBeatID, NULL, playBeat, NULL);
}

void beatGen_cleanup(){
	isRunning = false;
	pthread_join(playBeatID, NULL);
}

int beatGen_getMode(){                 //single core processor, no need to lock mutex
    return beatAttributes.currentBeat;
}

int beatGen_getBPM(){                   //single core processor, no need to lock mutex
    return beatAttributes.currentBPM;
}

void beatGen_incrementBPM(){
    setBPM(beatAttributes.currentBPM + 5);  
}

void beatGen_decrementBPM(){
    setBPM(beatAttributes.currentBPM - 5);
}

void beatGen_cycleBeat(){
    pthread_mutex_unlock(&beatMutex);
    {
		beatAttributes.currentBeat = (beatAttributes.currentBeat + 1) % NUMBER_OF_BEATS;
    }    
    pthread_mutex_unlock(&beatMutex);
}

static void setBPM(int inputBPM){

    pthread_mutex_lock(&bpmMutex);       //can maybe avoid locking mutex if we precheck the inputBPM bounds before entering function
    {
        if (inputBPM < MIN_BPM){
            beatAttributes.currentBPM = MIN_BPM;
        } 
        else if (inputBPM > MAX_BPM){
            beatAttributes.currentBPM = MAX_BPM;
        } 
        else{
            beatAttributes.currentBPM = inputBPM;
        }
    }
    pthread_mutex_unlock(&bpmMutex);

}

static void waitForNextEighthNote(){
    pthread_mutex_lock(&bpmMutex);
    //{
        int msForEighthNote = (60*1000) / (beatAttributes.currentBPM * 2);
    //}
    pthread_mutex_unlock(&bpmMutex);

    sleepForMs(msForEighthNote);
}

static void *playBeat(){ //thread will run constantly, and will play the beat at the current BPM in 4/4 time.
    while(isRunning){

        for(int i = 0; i < EIGHTH_NOTES_PER_BAR*2; i++){
            if(!isRunning){
                break;
            }
        	pthread_mutex_lock(&beatMutex);
                {

                AudioMixer_playNote(allBeats[beatAttributes.currentBeat][i]);
                
                }
            pthread_mutex_unlock(&beatMutex);
            Interval_markInterval(1);
            waitForNextEighthNote();
        }
    }
	return NULL;
}