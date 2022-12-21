//This module defines all the necessary functions to use the 8*8 led matrix
#ifndef LED_H
#define LED_H

void LED_init();
void clearAll();
void display_int(int x);
void display_double(double x);
void display_mode(int mode);
void displayMorse(char bits);

#endif