// userInput.h 
// Module to get the user input from the joystick and buttons
// Provides a function to initialize the user input thread
// User input thread will continuously read the joystick and buttons and update the LED matrix
// Thread will also update the mode, tempo and volume variables in their respective modules
// Provides a function to clean up the user input thread

#ifndef USERINPUT_H
#define USERINPUT_H

void userInput_init();
void userInput_cleanup();

#endif