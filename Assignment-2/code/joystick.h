// joystick.h
// Module to initialize the joystick and to read the joystick position.
// Provides a function to read the joystick position.
// values are mapped to the range [-1, 1]
// -1 is left/down
// 1 is right/up

#ifndef JOYSTICK_H
#define JOYSTICK_H

void Joystick_readXY(int *x, int *y);

#endif
