#include "LEDMatrix.h"
#include "audioMixer.h"
#include "beatGeneration.h"
#include "button.h"
#include "common.h"
#include "intervalTimer.h"
#include "outputStatistics.h"
#include "userInput.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


void cleanUpAllModules();
void initializeAllModules();

int main(){

    initializeAllModules();
    while (true){
        // Quit?
        if (toupper(getchar()) == 'Q'){
            cleanUpAllModules();
            return 0;
        }
    }
}

void cleanUpAllModules(){
    userInput_cleanup();
    outputStatistics_Cleanup();
    beatGen_cleanup();
    AudioMixer_cleanup();
    Interval_cleanup();
}

void initializeAllModules(){
    Interval_init();
    LEDMatrix_init();
    AudioMixer_init();
    beatGen_init();
    userInput_init();
    printf("Enter 'Q' to quit.\n");
    sleepForMs(1000);
    outputStatistics_Init();
}