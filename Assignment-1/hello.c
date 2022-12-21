#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define USER_BUTTON_FILEPATH "/sys/class/gpio/gpio72/value"

//two arrays of file paths for Led files. Used for iteratively changing LED characterstics
const char *ledTriggerFilePaths[4] = {
    "/sys/class/leds/beaglebone:green:usr0/trigger",
    "/sys/class/leds/beaglebone:green:usr1/trigger",
    "/sys/class/leds/beaglebone:green:usr2/trigger",
    "/sys/class/leds/beaglebone:green:usr3/trigger"
};

const char *ledBrightnessFilePaths[4] = {
    "/sys/class/leds/beaglebone:green:usr0/brightness",
    "/sys/class/leds/beaglebone:green:usr1/brightness",
    "/sys/class/leds/beaglebone:green:usr2/brightness",
    "/sys/class/leds/beaglebone:green:usr3/brightness"
};

//function declarations
static void disableMultipleLEDsTrigger();
static void modifySingleLEDBrightness(int index, int state);
static void modifyMultipleLEDsBrightness(int ledStates[]);
static void sleepForMs(long long delayInMs);
static void waitRandomTime();
static long long getTimeInMs(void);
static void runCommand(char* command);
static int getButtonStatus();
static void displayStats(int currentTime, int bestTime);
static void initializeGame();

int main(int argc, char* args[]){

    initializeGame();

    int allOff[] = {0,0,0,0};   //bitmasks for Led control. 1 == turn LED on. array[index] corresponds to LED[index]
    int firstOn[] = {1,0,0,0};
    int allOn[] = {1,1,1,1};

    long long int currentResponseTime = 5000;
    long long int bestResponseTime = 5000;

    while(1)
    {
        //start of one loop of the game

        //wait while user holds down the button
        while(getButtonStatus());

        modifyMultipleLEDsBrightness(firstOn);

        //wait a random amount of time, after random time is up, reaction time starts
        waitRandomTime();
        
        //first check if user is holding down the button already
        int buttonStatus = getButtonStatus();
        if (buttonStatus)
        {
            modifyMultipleLEDsBrightness(allOn);
            currentResponseTime = 5000;

            //printf("\nYour reaction time was %lldms; best so far in game is %lldms", currentResponseTime, bestResponseTime);
            displayStats(currentResponseTime, bestResponseTime);
            continue;
        }


        //if user isn't holding down button, light up led3
        modifySingleLEDBrightness(3,1);

        long long int endTime = getTimeInMs() + 5000;    //5 seconds after

        while(getTimeInMs() < endTime)
        {     //5 second timer
            buttonStatus = getButtonStatus();
            
            if (buttonStatus)   //if user presses button within 5 seconds
            {
                currentResponseTime = getTimeInMs() - endTime + 5000;
                modifyMultipleLEDsBrightness(allOn);

                if (currentResponseTime < bestResponseTime)
                {
                    bestResponseTime = currentResponseTime;
                    printf("New Best Time!\n");
                }

            //printf("\nYour reaction time was %lldms; best so far in game is %lldms", currentResponseTime, bestResponseTime);
            displayStats(currentResponseTime, bestResponseTime);
            goto EndOuterWhileLoop;
            }

        }
        printf("No input within 5000ms; quitting!\n");
        modifyMultipleLEDsBrightness(allOff);
        return 0;

        EndOuterWhileLoop:;
    }

}

static void initializeGame(){

    //set triggers to none
    disableMultipleLEDsTrigger();

    //turn on only LED 0
    int firstOn[] = {1,0,0,0};
    modifyMultipleLEDsBrightness(firstOn);
    
    //configure button to be used as input and be active high
    char* configPinCommand = "config-pin p8.43 gpio";
    char* setButtonAsInput = "echo in > /sys/class/gpio/gpio72/direction";
    char* setButtonToActiveHigh = "echo 1 > /sys/class/gpio/gpio72/active_low";
    runCommand(configPinCommand);
    runCommand(setButtonAsInput);
    runCommand(setButtonToActiveHigh);

    printf("Hello embedded world, from Nick\n\nWhen LED3 lights up, press the USER button!\n");
}

static void displayStats(int currentTime, int bestTime){
    printf("Your reaction time was %dms; best so far in game is %dms\n", currentTime, bestTime);
}

static int getButtonStatus(){
    FILE *pFile = fopen(USER_BUTTON_FILEPATH, "r");
    if (pFile == NULL) {
        printf("ERROR: Unable to open file (%s) for read\n", USER_BUTTON_FILEPATH);
        exit(-1);
    }

    int buttonStatus = 0;

    fscanf(pFile, "%d", &buttonStatus);
    fclose(pFile);
    return buttonStatus;
}

static void runCommand(char* command){
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");

    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
            break;
        // printf("--> %s", buffer); // Uncomment for debugging
    }

    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));

    if (exitCode != 0) {
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
}

static long long getTimeInMs(void){
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);

    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec;
    long long milliSeconds = seconds * 1000 + nanoSeconds / 1000000;

    return milliSeconds;
}

static void sleepForMs(long long delayInMs){
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;

    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;

    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}

static void waitRandomTime(){
    srand(time(NULL));
    int randomTimeInMS = (rand() % (3000 - 500 + 1)) + 500;

    sleepForMs(randomTimeInMS);
}

static void modifyMultipleLEDsBrightness(int ledStates[]){
    //a value of 1 in the position means turn on
    //highest order bit == LED3 downto lowest order bit == LED0
    for (int i =0; i <= 3; i++){
        int currentLEDState = ledStates[i];

        FILE *ledBrightnessFile = fopen(ledBrightnessFilePaths[i], "w");
        
        if (ledBrightnessFile == NULL)
        {
            printf("Error opening file");
            exit(1);
        }

        int checkWritten = fprintf(ledBrightnessFile, "%d", currentLEDState);
        if (checkWritten <= 0)
        {
            printf("Error writing to file");
            exit(1);
        }
        fclose(ledBrightnessFile);

    }
}

//seperate function to modify just a single LED to increase performance + accuracy of reaction test
static void modifySingleLEDBrightness(int index, int state){      
    
    FILE *LedBrightnessFile = fopen(ledBrightnessFilePaths[index], "w");

    if (LedBrightnessFile == NULL)
    {
        printf("Error opening file");
        exit(1);
    }

    int charWritten = fprintf(LedBrightnessFile, "%d", state);
    if (charWritten <= 0){
        printf("Error writing to Brightness file");
        exit(1);
    }
    fclose(LedBrightnessFile);
}

static void disableMultipleLEDsTrigger(){
    //we will only use this function to set the triggers to none
    //highest order bit == LED3 downto lowest order bit == LED0
    for (int i =0; i <= 3; i++){
        FILE *ledTriggerFile = fopen(ledTriggerFilePaths[i], "w");
        
        if (ledTriggerFile == NULL)
        {
            printf("Error opening file");
            exit(1);
        }

        int checkWritten = fprintf(ledTriggerFile, "none");
        if (checkWritten <= 0)
        {
            printf("Error writing to file");
            exit(1);
        }
        fclose(ledTriggerFile);
    }
}
