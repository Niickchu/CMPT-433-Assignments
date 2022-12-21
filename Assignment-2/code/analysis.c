#include "analysis.h"
#include "common.h"
#include "sampler.h"
#include "LED_output.h"
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static void* analyzeSamples();
static void setVoltages(double inputVoltages[], int numberOfSamples);
static void setTimeBetweenSamples(long long inputTimes[], int numberOfSamples);
static void printAnalysisData(void);
static void convertToVolts(double inputVoltages[], int numberOfSamples);

typedef struct{
    int numberOfSamples;

    int numberOfDips;       //voltage statistics
    double averageVoltage;
    double minVoltage;
    double maxVoltage;

    double minTimeBetweenSamples;    //time statistics
    double maxTimeBetweenSamples;
    double averageTimeBetweenSamples;
}analysisData;

analysisData currentData;

pthread_t analysisThreadID;

static bool isRunning = true;
void Analysis_init(void){
    pthread_create(&analysisThreadID, NULL, analyzeSamples, NULL);
}

void Analysis_stop(void){
    isRunning = false;  //stop creating new threads
    pthread_join(analysisThreadID, NULL);   //end current thread
}

static void* analyzeSamples(){

    while(isRunning){
        double voltageBuffer[BUFFER_SIZE];         //every second we get a new copy of these arrays
        long long timeBuffer[BUFFER_SIZE];
        int numberOfSamples = 0;

        Sampler_readCurrentBuffer(voltageBuffer, timeBuffer, &numberOfSamples);  //locks and unlocks mutex

        currentData.numberOfSamples = numberOfSamples;  
        setVoltages(voltageBuffer, numberOfSamples);
        setTimeBetweenSamples(timeBuffer, numberOfSamples);
        LEDoutput_updateLEDData(currentData.numberOfDips, currentData.minVoltage, currentData.maxVoltage, currentData.minTimeBetweenSamples, currentData.maxTimeBetweenSamples);
        printAnalysisData();
        sleepForMs(1000);

    }
    return NULL;
}

static bool isFirstTime = true;
static double historyVoltage = 0;
static void setVoltages(double inputVoltages[], int numberOfSamples)
{

    convertToVolts(inputVoltages, numberOfSamples);
    double minValue = inputVoltages[0];
    double maxValue = inputVoltages[0];
    double currentFilteredVoltage;
    double weightingFactor = 0.01;
    int numberOfDips = 0;
    bool isValidDip = true;



    if (isFirstTime)
    {
        historyVoltage = inputVoltages[0];
        isFirstTime = false;
    }
    for (int i = 0; i < numberOfSamples; i++){      //compute statistics in one loop to avoid iterating over array multiple times

        if (inputVoltages[i] < minValue){         //setting minVals
            currentData.minVoltage = inputVoltages[i];
        }

        if (inputVoltages[i] > maxValue){         //setting maxVals
            maxValue = inputVoltages[i];
        }

        currentFilteredVoltage = (inputVoltages[i] * weightingFactor) + (historyVoltage * (1 - weightingFactor));  //exponential moving average
        historyVoltage = currentFilteredVoltage;

        if (inputVoltages[i] < (currentFilteredVoltage - 0.1) && isValidDip){
            numberOfDips++;                       
            isValidDip = false;
        }

        if (inputVoltages[i] > (currentFilteredVoltage + 0.03)){
            isValidDip = true;                        
        }

    }
    currentData.minVoltage = minValue;                      //don't want to update until we are all done analyzing the data
    currentData.maxVoltage = maxValue;
    currentData.averageVoltage = currentFilteredVoltage;
    currentData.numberOfDips = numberOfDips;
}


static void convertToVolts(double inputVoltages[], int numberOfSamples)
{
    for (int i = 0; i < numberOfSamples; i++)
    {
        inputVoltages[i] = (inputVoltages[i]  *1.8) / 4095;
    }
}

static long long lastSample = 0;
static bool firstDelta = true;
static const double USPerMS = 1000.0;
static void setTimeBetweenSamples(long long inputTimes[], int numberOfSamples)
{
    double deltaArray[numberOfSamples];

    if (firstDelta){
        deltaArray[0] = 0;
        firstDelta = false;
    }
    else{
        deltaArray[0] = (inputTimes[0] - lastSample)/USPerMS;
    }

    lastSample = inputTimes[numberOfSamples - 1];   //update last sample

    for (int i = 1; i < numberOfSamples-1; i++){
        deltaArray[i] = (inputTimes[i+1] - inputTimes[i])/USPerMS;
    }

    double minTime = deltaArray[0];
    double maxTime = deltaArray[0];


    double totalDelta = 0;
    for (int i = 0; i < numberOfSamples-1; i++){
        if (deltaArray[i] < minTime){
            minTime = deltaArray[i];
        }
        if (deltaArray[i] > maxTime){
            maxTime = deltaArray[i];
        }
        totalDelta += deltaArray[i];
    }

    currentData.averageTimeBetweenSamples = totalDelta/(numberOfSamples-1);   
    currentData.minTimeBetweenSamples = minTime;
    currentData.maxTimeBetweenSamples = maxTime;

}

static void printAnalysisData(void){
    printf("Interval ms (%.3f, %.3f) avg=%.3f\t", currentData.minTimeBetweenSamples, currentData.maxTimeBetweenSamples, currentData.averageTimeBetweenSamples);
    printf("Samples V (%.3f, %.3f) avg=%.3f\t dips:\t%d\t", currentData.minVoltage, currentData.maxVoltage, currentData.averageVoltage, currentData.numberOfDips);
    printf("# Samples: %d\n", currentData.numberOfSamples);
}