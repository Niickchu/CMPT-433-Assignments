#include "user_button.h"
#include "common.h"
#include "analysis.h"
#include "LED_output.h"
#include "sampler.h"
#include "shutdown.h"
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#define USER_BUTTON_FILEPATH "/sys/class/gpio/gpio72/value"

static void initializeButton();
static int getButtonStatus();
static void *pollUserButton();

static pthread_t userButtonThreadID;
static bool isRunning = true;

void userButton_initButton(void)                          
{
    initializeButton();
    pthread_create(&userButtonThreadID, NULL, pollUserButton, NULL);
}


void userButton_shutdownButton(){
    pthread_join(userButtonThreadID, NULL);     //end button thread
}

static void *pollUserButton()
{
    while (isRunning)
    {
        if (!getButtonStatus())
        {
            break;
        }

        sleepForMs(100);
    }
    printf("Button pressed, shutting down\n");
    signalShutdown();
    return NULL;
}

static int getButtonStatus()
{
    FILE *pFile = fopen(USER_BUTTON_FILEPATH, "r");
    if (pFile == NULL)
    {
        printf("ERROR: Unable to open file (%s) for read\n", USER_BUTTON_FILEPATH);
        exit(-1);
    }

    int buttonStatus = 0;

    fscanf(pFile, "%d", &buttonStatus);
    fclose(pFile);
    return buttonStatus;
}

static void initializeButton()
{
    // configure button to be used as input and be active high
    char *configPinCommand = "config-pin p8.43 gpio";
    char *setButtonAsInput = "echo in > /sys/class/gpio/gpio72/direction";

    runCommand(configPinCommand);
    runCommand(setButtonAsInput);
}