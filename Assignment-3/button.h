// button.h
// Module to get the status of all buttons in and return a bitmask
// The returned bitmask is summarized as follows:
//  0001 - green button
//  0010 - yellow button
//  0100 - red button
//  1000 - grey button
// A value of 1 in the bit position of a button indicates that the button is pressed

#ifndef BUTTON_H
#define BUTTON_H

#define MODE_B_IN   "/sys/class/gpio/gpio47/value"
#define BASS_B_IN   "/sys/class/gpio/gpio46/value"
#define SNARE_B_IN  "/sys/class/gpio/gpio27/value"
#define HIHAT_B_IN  "/sys/class/gpio/gpio65/value"

#define BUTTON_1_BIT 0x08

int button_getAllButtonStatus();

#endif