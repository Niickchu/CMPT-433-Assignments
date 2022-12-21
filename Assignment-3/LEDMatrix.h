// LED_matrix.h 
// Module to initialize the LED matrix and to display the mode, tempo and volume on the LED matrix
// Provides a function to initialize the LED matrix and a function to display the mode and an integer value on the LED matrix

#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#define DECIMAL_POINT 0x04
#define NO_DECIMAL_POINT 0x00
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2C_DEVICE_ADDRESS 0x70

void LEDMatrix_init();                  // Initialize the LED matrix must be called before any other functions
void LEDMatrix_displayInt(int number);  // Display an integer on the LED matrix, useful for displaying tempo and volume
void LEDMatrix_displayMode(int modeNumber); // Display the mode number (0 to 2) on the LED matrix

#endif