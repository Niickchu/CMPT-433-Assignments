#include "userInput.h"
#include "LEDMatrix.h"
#include "audioMixer.h"
#include "beatGeneration.h"
#include "button.h"
#include "common.h"
#include "joystick.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

static pthread_t userInputThreadID;
static bool isRunning;

static void *pollUserInput();
static void implementJoystick(int xValue, int yValue);

void userInput_init(){
    isRunning = true;
    pthread_create(&userInputThreadID, NULL, pollUserInput, NULL);
}

void userInput_cleanup(){
    isRunning = false;
    pthread_join(userInputThreadID, NULL);
}

static void *pollUserInput(){
    int joystickX = 0;
    int joystickY = 0;
    bool buttonIsHeld = false;
    while (isRunning){
        joystick_readXY(&joystickX, &joystickY);

        if (joystickX || joystickY){
            implementJoystick(joystickX, joystickY); // Joystick will have priority over buttons
            sleepForMs(500);                         // continue displaying the value for 0.5 seconds
        }
        LEDMatrix_displayMode(beatGen_getMode());

        int buttonStatus = button_getAllButtonStatus();

        if (buttonStatus && !buttonIsHeld){
            buttonIsHeld = true;
            if (buttonStatus & BUTTON_1_BIT){
                beatGen_cycleBeat();
                LEDMatrix_displayMode(beatGen_getMode());
            }
            buttonStatus = (buttonStatus & ~BUTTON_1_BIT); // clear the button 1 bit since
                                                           // button 1 is not a sound
            AudioMixer_playNote(buttonStatus);
        }
        if (!button_getAllButtonStatus()){
            buttonIsHeld = false;
        }
        sleepForMs(10);
    }
    return NULL;
}

static void implementJoystick(int xValue, int yValue){
    do{ // execute joystick functionality immediately
        if (xValue){
            if (xValue < 0){ // button is left, decrease tempo
                beatGen_decrementBPM();
                LEDMatrix_displayInt(beatGen_getBPM());
            }
            else{ // button is right, increase tempo
                beatGen_incrementBPM();
                LEDMatrix_displayInt(beatGen_getBPM());
            }
        }
        else if (yValue > 0){ // button is up, increase volume
            AudioMixer_incrementVolume();
            LEDMatrix_displayInt(AudioMixer_getVolume());
        }
        else{
            AudioMixer_decrementVolume(); // button is down, decrease volume
            LEDMatrix_displayInt(AudioMixer_getVolume());
        }
        sleepForMs(500); // wait half of a second to see if they're still
                         // holding the joystick
        joystick_readXY(&xValue, &yValue);
    } while (xValue || yValue);
}