// joystick.h
// Provides a function to read the joystick position.
// values are mapped to the range [-1, 1]
// For X axis, -1 is left, 1 is right
// For Y axis, -1 is down, 1 is up

#ifndef JOYSTICK_H
#define JOYSTICK_H

void joystick_readXY(int *x, int *y);

#endif