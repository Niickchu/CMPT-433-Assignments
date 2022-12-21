// LED_output.h
// Module to update the LED matrix struct with the current analysis results and read the joystick position.
// Provides a function to initialize the LED matrix and a function to start displaying the analysis results.
// Will create a thread to read the joystick position.

#ifndef LED_OUTPUT_H
#define LED_OUTPUT_H

//output to terminal
//output to LED

void LEDOutput_startDisplaying(void);
void LEDOutput_stopDisplaying(void);
void LEDoutput_updateLEDData(int numberOfDips, double minVoltage, double maxVoltage, double minTimeBetweenSamples, double maxTimeBetweenSamples);

#endif