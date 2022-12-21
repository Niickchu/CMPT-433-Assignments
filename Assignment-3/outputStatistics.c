#include "outputStatistics.h"
#include "audioMixer.h"
#include "beatGeneration.h"
#include "common.h"
#include "intervalTimer.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>


static pthread_t outputThreadId;
static bool isRunning;

static Interval_statistics_t lowLevelStats;
static Interval_statistics_t beatGenStats;

static void *outputStatistics(void *arg);

void outputStatistics_Init(){
    isRunning = true;
    pthread_create(&outputThreadId, NULL, outputStatistics, NULL);
}

void outputStatistics_Cleanup(){
    isRunning = false;
    pthread_join(outputThreadId, NULL);
}

static void *outputStatistics(void *arg){
    while (isRunning){
        Interval_getStatisticsAndClear(0, &lowLevelStats);
        Interval_getStatisticsAndClear(1, &beatGenStats);

        int mode = beatGen_getMode();
        int bpm = beatGen_getBPM();
        int volume = AudioMixer_getVolume();

        double lowMin = lowLevelStats.minIntervalInMs;      //compiler will optimize this
        double lowMax = lowLevelStats.maxIntervalInMs;      //this is human readable
        double lowAvg = lowLevelStats.avgIntervalInMs;
        int lowCount = lowLevelStats.numSamples;

        double beatMin = beatGenStats.minIntervalInMs;
        double beatMax = beatGenStats.maxIntervalInMs;
        double beatAvg = beatGenStats.avgIntervalInMs;
        int beatCount = beatGenStats.numSamples;

        // output statistics
        printf("M%d %dbpm vol:%d Low[%.2f, %.2f] avg %.2f/%d Beat[%.2f, %.2f] avg %.2f/%d\n",
            mode, bpm, volume, 
            lowMin, lowMax, lowAvg, lowCount, 
            beatMin, beatMax, beatAvg, beatCount);

        sleepForMs(1000);
    }
    return NULL;
}
