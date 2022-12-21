#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "joystick.h"

#define A2D_FILE_VOLTAGE3 "/sys/bus/iio/devices/iio:device0/in_voltage3_raw"
#define A2D_FILE_VOLTAGE2 "/sys/bus/iio/devices/iio:device0/in_voltage2_raw"
#define AVERAGE_A2D_VALUE 2048.0

static double Joystick_normalizeReadings(int a2dReading);
static int Joystick_evaluateThreshold(double a2dValue);

//when called, want to open both files + read the values, write to x and y, then close both files //probably want to use with a thread, no mutex needed
void Joystick_readXY(int *x, int *y) {

    FILE *xFile = fopen(A2D_FILE_VOLTAGE3, "r");
    FILE *yFile = fopen(A2D_FILE_VOLTAGE2, "r");

    if (xFile == NULL || yFile == NULL) {
        printf("Error opening Joystick A2D file");
        exit(-1);
    }

    // // Get reading
    int xA2dValue = 0;
    fscanf(xFile, "%d", &xA2dValue);
    fclose(xFile);

    int yA2dValue = 0;
    fscanf(yFile, "%d", &yA2dValue);
    fclose(yFile);

    // normalize values
    *x = Joystick_normalizeReadings(xA2dValue);
    *y = Joystick_normalizeReadings(yA2dValue);

}

static double Joystick_normalizeReadings(int a2dReading) {

    double normalizedValue = (a2dReading - AVERAGE_A2D_VALUE)/AVERAGE_A2D_VALUE;
    return Joystick_evaluateThreshold(normalizedValue *-1);   //want to make left negative, right positive, up positive, down negative. Therefore, multiply by -1
}

static int Joystick_evaluateThreshold(double a2dValue){
    if (a2dValue > 0.5){
        return 1;
    }
    else if (a2dValue < -0.5){
        return -1;
    }
    else{
        return 0;
    }
}
