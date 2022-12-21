#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "sampler.h"
#include "common.h"

#define A2D_FILE_VOLTAGE1 "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"

static void* Sampler_writeSamples();
static int getCurrentLightSample();

static bool isRunning = true;
static int writeIndex = 0;

samplerDatapoint_t sampleBuffer[BUFFER_SIZE];

static pthread_t writeThreadID;
static pthread_mutex_t bufferMutex = PTHREAD_MUTEX_INITIALIZER;



void Sampler_startSampling(void){
    printf("Starting to sample data...\n");
    pthread_create(&writeThreadID, NULL, Sampler_writeSamples, NULL);
}

void Sampler_stopSampling(void){
    isRunning = false;
    pthread_join(writeThreadID, NULL);
}

void Sampler_readCurrentBuffer(double voltageBuffer[], long long timeBuffer[], int *bufferSize){
    pthread_mutex_lock(&bufferMutex);
    {
        *bufferSize = writeIndex;
        for (int i = 0; i < writeIndex; i++)                                    //perhaps try again with structs
            {
                voltageBuffer[i] = (sampleBuffer[i].sampleValue);
                timeBuffer[i] = sampleBuffer[i].timestampInMicroS;
                
            }
    
        writeIndex = 0;
    }
    pthread_mutex_unlock(&bufferMutex);
}

static void* Sampler_writeSamples(){
    writeIndex = 0;
    while(isRunning){

        pthread_mutex_lock(&bufferMutex);
        {
            sampleBuffer[writeIndex].sampleValue = getCurrentLightSample();
            sampleBuffer[writeIndex].timestampInMicroS = getTimeInUS();
            writeIndex++;
        }
        pthread_mutex_unlock(&bufferMutex);

        sleepForMs(1);

    }
    return NULL;
}

static int getCurrentLightSample(){

    FILE *lightSampleFile = fopen(A2D_FILE_VOLTAGE1, "r"); //avoid checking for null to speed up performance

    int currentLightSample = 0;
    fscanf(lightSampleFile, "%d", &currentLightSample);
    fclose(lightSampleFile);

    return currentLightSample;
}
