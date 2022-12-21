#include "LED_matrix.h"
#include "common.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h> //for write()
#include <fcntl.h>  //0_RDWR
#include <sys/ioctl.h> //for ioctl()
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define DECIMAL_POINT 0x04
#define NO_DECIMAL_POINT 0x00
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2C_DEVICE_ADDRESS 0x70

static void LEDMatrix_displayNumber(int number, bool isDouble);
static void writeToLedMatrix(unsigned char *value);
static int initI2cBus(char *bus, int address);

static int i2cFileDesc = -1;
static const unsigned char ledRegisterAddresses[] = {0x0, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E};

static unsigned char digits[10][7] =
    {
        //  bottom row                           top row
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

void LEDMatrix_displayInt(int inputInteger)
{
    bool isDouble = false;
    LEDMatrix_displayNumber(inputInteger, isDouble);
}

void LEDMatrix_displayDouble(double inputDouble)
{
    bool isDouble = true;
    int inputInteger = inputDouble * 10;
    LEDMatrix_displayNumber(inputInteger, isDouble);
}

static void LEDMatrix_displayNumber(int number, bool isDouble)
{
    // first want check range on input values
    if (number < 0 || number > 99)
    {
        number = 99;
    }

    int tens = number / 10;
    int ones = number % 10;

    unsigned char fullBits[8];
    if (isDouble)
    {
        fullBits[0] = DECIMAL_POINT;         //hash define these
    }
    else
    {
        fullBits[0] = NO_DECIMAL_POINT;
    }

    for (int i = 1; i < 8; i++)
    {
        fullBits[i] = (digits[tens][i - 1] << 5) | digits[ones][i - 1];     //shift tens bits into position and OR with ones bits
        fullBits[i] = (fullBits[i] >> 1) | (fullBits[i] << 7);              //rotate bits
    }

    writeToLedMatrix(&fullBits[0]);
}

static void writeToLedMatrix(unsigned char *value)
{
    unsigned char buff[2];
    for (int i = 0; i < 8; i++)
    {
        buff[0] = ledRegisterAddresses[i];
        buff[1] = value[i];
        int res = write(i2cFileDesc, buff, 2);
        if (res != 2)
        {
            perror("I2C: Unable to write to I2C device.");
            exit(1);
        }
    }
}

void LEDMatrix_init()
{
    // Set up I2C pins

    char* configPin17Command = "config-pin p9.17 i2c";
    char* configPin18Command = "config-pin p9.18 i2c";

    runCommand(configPin17Command);
    runCommand(configPin18Command);

    // set up I2C bus
    i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);

    char* turnOnLEDMatrix = "i2cset -y 1 0x70 0x21 0x00";
    char* setLEDDisplayOn = "i2cset -y 1 0x70 0x81 0x00";

    runCommand(turnOnLEDMatrix);
    runCommand(setLEDDisplayOn);
}

static int initI2cBus(char *bus, int address)
{
    int i2cFileDesc = open(bus, O_RDWR);
    int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
    if (result < 0)
    {
        perror("I2C: Unable to set I2C device to slave address.");
        exit(1);
    }
    return i2cFileDesc;
}
