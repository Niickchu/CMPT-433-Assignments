#include "LED_output.h"
#include "joystick.h"
#include "LED_matrix.h"
#include <pthread.h>
#include <stdbool.h>

static void* writeToLED();

typedef struct {
    int numberOfDips;
    double minVoltage;
    double maxVoltage;
    double minTimeBetweenSamples;
    double maxTimeBetweenSamples;
} LEDData;

static pthread_t ledOutputThreadID;
//static pthread_mutex_t ledValuesMutex = PTHREAD_MUTEX_INITIALIZER;
static bool isRunning = true;

static LEDData currentLEDData;

void LEDoutput_updateLEDData(int numberOfDips, double minVoltage, double maxVoltage, double minTimeBetweenSamples, double maxTimeBetweenSamples){
    //pthread_mutex_lock(&ledValuesMutex);
    currentLEDData.numberOfDips = numberOfDips;
    currentLEDData.minVoltage = minVoltage;
    currentLEDData.maxVoltage = maxVoltage;
    currentLEDData.minTimeBetweenSamples = minTimeBetweenSamples;
    currentLEDData.maxTimeBetweenSamples = maxTimeBetweenSamples;
    //pthread_mutex_unlock(&ledValuesMutex);
}

void LEDOutput_startDisplaying(void){
    pthread_create(&ledOutputThreadID, NULL, writeToLED, NULL);
}

void LEDOutput_stopDisplaying(void){
    isRunning = false;
    pthread_join(ledOutputThreadID, NULL);
}

static void* writeToLED(){
    while(isRunning){
        int joystickX = 0;
        int joystickY = 0;

        Joystick_readXY(&joystickX, &joystickY);

        //pthread_mutex_lock(&ledValuesMutex);                              //values are written to the LED almost continuously, and the struct is updated once every second
        if (joystickX != 0){                                                //therefore this is a benign race case and a mutex is redundant. I have chosen to leave it out to
            if (joystickX < 0){                                             //increase performace
                LEDMatrix_displayDouble(currentLEDData.minTimeBetweenSamples);
            }
            else{
                LEDMatrix_displayDouble(currentLEDData.maxTimeBetweenSamples);
            }
        }
        else if (joystickY != 0){
            if (joystickY < 0){
                LEDMatrix_displayDouble(currentLEDData.minVoltage);
            }
            else{
                LEDMatrix_displayDouble(currentLEDData.maxVoltage);
            }
        }
        else{
            LEDMatrix_displayInt(currentLEDData.numberOfDips);
        }
        //pthread_mutex_unlock(&ledValuesMutex);
    }
    return NULL;
}