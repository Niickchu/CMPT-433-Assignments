#include "button.h"
#include <stdio.h>
#include <stdlib.h>

static char *buttonFiles[] = {HIHAT_B_IN, SNARE_B_IN, BASS_B_IN, MODE_B_IN};
static int button_getButtonStatus(char *filename);

int button_getAllButtonStatus(){
    int buttonStatus = 0;
    for (int i = 0; i < 4; i++){
        buttonStatus |= (button_getButtonStatus(buttonFiles[i]) << i); // 0001 - hi hat
    }                                                                  // 0010 - snare
    return buttonStatus;                                               // 0100 - bass
}                                                                      // 1000 - mode

static int button_getButtonStatus(char *filename){
    FILE *buttonFile = fopen(filename, "r");
    if (buttonFile == NULL){
        printf("ERROR: Unable to open button file for read \n");
        exit(-1);
    }
    int buttonValue = 0;

    fscanf(buttonFile, "%d", &buttonValue);
    fclose(buttonFile);
    return buttonValue;
}
