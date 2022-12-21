#include "LEDMatrix.h"
#include "common.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>    //for write()
#include <fcntl.h>     //0_RDWR
#include <sys/ioctl.h> //for ioctl()
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

static void writeToLedMatrix(unsigned char *value);
static int initI2cBus(char *bus, int address);

static int i2cFileDesc = -1;
static const unsigned char ledRegisterAddresses[] = {0x0, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E};

static const unsigned char digits[10][7] = {
    // bottom row                           top row
    {0x02, 0x05, 0x05, 0x05, 0x05, 0x05, 0x02}, // 0

    {0x07, 0x02, 0x02, 0x02, 0x02, 0x06, 0x02}, // 1

    {0x07, 0x04, 0x02, 0x02, 0x01, 0x05, 0x02}, // 2

    {0x06, 0x01, 0x01, 0x07, 0x01, 0x01, 0x06}, // 3

    {0x01, 0x01, 0x01, 0x07, 0x05, 0x05, 0x05}, // 4

    {0x02, 0x05, 0x01, 0x07, 0x04, 0x04, 0x07}, // 5

    {0x02, 0x05, 0x05, 0x06, 0x04, 0x04, 0x03}, // 6

    {0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x07}, // 7

    {0x02, 0x05, 0x05, 0x02, 0x05, 0x05, 0x02}, // 8

    {0x02, 0x05, 0x01, 0x03, 0x05, 0x05, 0x02} // 9

};

static const unsigned char letterM[8] = {0x05, 0x05, 0x05, 0x05, 0x07, 0x07, 0x05}; // M
void LEDMatrix_init()
{
    // Set up I2C pins

    char *configPin17Command = "config-pin p9.17 i2c";
    char *configPin18Command = "config-pin p9.18 i2c";

    runCommand(configPin17Command);
    runCommand(configPin18Command);

    // set up I2C bus
    i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);

    char *turnOnLEDMatrix = "i2cset -y 1 0x70 0x21 0x00";
    char *setLEDDisplayOn = "i2cset -y 1 0x70 0x81 0x00";

    runCommand(turnOnLEDMatrix);
    runCommand(setLEDDisplayOn);
}

void LEDMatrix_displayMode(int modeNumber){
    // Write an M to the tens place and the mode number to the ones place
    unsigned char fullBits[8];

    fullBits[0] = NO_DECIMAL_POINT;

    for (int i = 1; i < 8; i++){
        fullBits[i] = (letterM[i - 1] << 5) | digits[modeNumber][i - 1]; // shift tens bits into position and OR with ones bits
        fullBits[i] = (fullBits[i] >> 1) | (fullBits[i] << 7);           // rotate bits
    }

    writeToLedMatrix(&fullBits[0]);
}

void LEDMatrix_displayInt(int number){
    // first want to check range on input values
    if (number < 0){
        number = 0;
    }
    if (number > 99){
        number = 99;
    }

    int tens = number / 10;
    int ones = number % 10;

    unsigned char fullBits[8];

    fullBits[0] = NO_DECIMAL_POINT; // set bottom row to off

    for (int i = 1; i < 8; i++){
        fullBits[i] = (digits[tens][i - 1] << 5) | digits[ones][i - 1]; // shift tens bits into position and OR with ones bits
        fullBits[i] = (fullBits[i] >> 1) | (fullBits[i] << 7);          // rotate bits
    }

    writeToLedMatrix(&fullBits[0]);
}

static void writeToLedMatrix(unsigned char *value){
    unsigned char buff[2];
    for (int i = 0; i < 8; i++){
        buff[0] = ledRegisterAddresses[i];
        buff[1] = value[i];

        int res = write(i2cFileDesc, buff, 2);
        if (res != 2){
            perror("I2C: Unable to write to I2C device.");
            exit(1);
        }
    }
}

static int initI2cBus(char *bus, int address){
    i2cFileDesc = open(bus, O_RDWR);
    int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
    if (result < 0){
        perror("I2C: Unable to set I2C device to slave address.");
        exit(1);
    }
    return i2cFileDesc;
}
