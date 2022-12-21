#include "morseApp.h"

bool morsecode[2048];
static int length = 0;
char *buffer;
char *morse;
char ledInput = 0;
volatile sharedMemStruct_t *pSharedPru0;

void getMorse()
{
    morse = malloc(length);
    for(int i=0; i<length; i++)
    {
        if(morsecode[i])
        {
            morse[i] = 'X';
        }
        else
        {
            morse[i]= '_';
        }
    }
    printf("%s\n", morse);
    
}
void getBool(unsigned short x)
{
    int last_bit = 0; 
    if(x==0)
    {
        for(int i = 0; i<7;i++)
        {
            pSharedPru0->message[length] = 0;
            length++;
        }
        return;
    }
    while(!(x & (1<<last_bit)))
    {
        last_bit++;
    }
    for(int i=15; i>=last_bit; i--)
    {
        bool bit = x & (1<<i);
        pSharedPru0->message[length] = bit;
        length++;
    }
    for(int i = 0; i<3;i++)
    {
        pSharedPru0->message[length] = 0;
        length++;
    }
}

int analizeText(char* buffer, size_t size)
{
    char filtered_buff[size];
    int filtered_index = 0;
    for(int i=0; i<size-1; i++)
    {
        unsigned short x = MorseCode_getFlashCode(buffer[i]);
        if(x==0 && !isspace(buffer[i]))
        {
            continue;
        }
        filtered_buff[filtered_index] = buffer[i];
        filtered_index++;
        getBool(x);
    }
    filtered_buff[filtered_index] = 0;
    length -= 3;
    for(int i=0; i<length; i++)
    {
        morsecode[i] = pSharedPru0->message[i];
    }
    if(filtered_index == 0)
    {
        printf("Invalid input. Try again \n");
        return 1;
    }
    printf("Flashing out %d characters: %s\n", filtered_index, filtered_buff);
    return 0;
}

void driveMatrix()
{
    int index = 0;
    getMorse();
    for(int i = 0; i<8; i++)
    {
        ledInput |= (morsecode[7-i] << i);
    }
    displayMorse(ledInput);
    printf("%c", morse[index]);
    while(index<length-1)
    {
        if(index< pSharedPru0->index)
        {
            index++;
            ledInput = ledInput*2 + morsecode[index+7];
            displayMorse(ledInput);
            printf("%c", morse[index]);
            fflush(stdout);
        }
        sleepForMs(10);   
    }
    
    printf("\n");
    clearAll();
    free(morse);
    ledInput = 0;
}

int getInput()
{
    size_t bufsize = 32;
    size_t characters;

    printf("> ");
    characters = getline(&buffer,&bufsize,stdin);
    if(characters==1)
    {
        return 1;
    }
    for(int i = characters-2; i>=0; i--)
    {
        if(isspace(buffer[i]))
        {
            characters--;
        }
        else{
            break;
        }
    }
    int status = analizeText(buffer, characters);
    if(status)
    {
        return 0;
    }
    pSharedPru0->size = length;
    pSharedPru0->messageReady = 1;
    driveMatrix();
    length = 0;
    return 0;
}

void startSpying()
{
    LED_init();
    clearAll();
    // Get access to shared memory for my uses
    volatile void *pPruBase = getPruMmapAddr();
    pSharedPru0 = PRU0_MEM_FROM_BASE(pPruBase);

    runCommand("sudo config-pin p9.27 pruout");
    runCommand("sudo config-pin p9.28 pruin");

    buffer = malloc(50*sizeof(char));
    while(true)
    {
        int status = getInput();
        if(status)
        {
            break;
        }
        
    }
    // Cleanup
    free(buffer);
    freePruMmapAddr(pPruBase);
    clearAll();
}
