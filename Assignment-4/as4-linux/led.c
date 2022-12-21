#include "led.h"
#include "utilities.h"
#include <math.h>

#define BASE_ADDR "i2cset -y 1 0x70"

static char **head = NULL;
static char* letter = NULL;

static int rotate_right(int x)
{

    if (x % 2 == 0)
    {
        return (x >> 1);
    }
    else
    {
        int ans = (x >> 1);
        return ans + 128;
    }
}

static void setRowBits(int row, char bits)
{
    char command[50];
    row = (8 - row) * 2;
    sprintf(command, "i2cset -y 1 0x70 %d %d", row, bits);
    runCommand(command);
}

void LED_init()
{
    runCommand("config-pin P9_18 i2c");
    runCommand("config-pin P9_17 i2c");
    runCommand("i2cset -y 1 0x70 0x21 0x00");
    runCommand("i2cset -y 1 0x70 0x81 0x00");
}
void clearAll()
{
    for (int i = 1; i < 9; i++)
    {
        setRowBits(i, 0);
    }
}

// static void checkBits()
// {

//     for (int i = 1; i < 9; i++)
//     {
//         int bits = pow(2.0, i - 1);
//         bits = rotate_right(bits);
//         setRowBits(i, bits);
//         printf("Bits: %d\n", bits);
//         sleepForMs(500);
//         bits++;
//     }
// }

void displayMorse(char bits)
{
    bits = rotate_right(bits);
    for(int i = 5; i<9; i++)
    {
        setRowBits(i, bits);
    }
}

static void display_digit(int x)
{
    for (int i = 1; i < 8; i++)
    {
        //printf("Bit %d: \n", i);
        int bits = rotate_right(head[x][i]);
        setRowBits(i, bits);
    }
}

static void display_2digit(int dozen, int digit)
{
    for (int i = 1; i < 8; i++)
        {
            // printf("Number %d %d\n", dozen, digit);
            int bits = (head[dozen][i] << 5) | head[digit][i];
            bits = rotate_right(bits);
            setRowBits(i, bits);
        }
}


void display_int(int x)
{
    if(x>99){x=99;}
    if (x < 10)
    {
        display_digit(x);
    }
    else
    {
        int dozen = x / 10;
        int digit = x % 10;
        display_2digit(dozen, digit);
    }
    setRowBits(8, 0);
}

void display_double(double x)
{   
    if(x>=9.9){
        display_2digit(9,9);
    }
    else{
        //x = round(x*10.0)/10.0;
        int dozen = x;
        int digit = (x*10);
        digit = digit%10;
        display_2digit(dozen, digit);
    }

    setRowBits(8, 1<<3);
}

void display_mode(int mode)
{
    for (int i = 1; i < 8; i++)
        {
            int bits = (letter[i] << 3) | head[mode][i];
            bits = rotate_right(bits);
            setRowBits(i, bits);
        }
}