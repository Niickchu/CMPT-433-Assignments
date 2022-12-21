// beatGeneration.h
// Module to start and stop the beat generation thread
// Provides a function to initialize the beat generation thread, and start generating beats
// Provides a function to clean up the beat generation thread
// Provides getter and setter functions for the tempo and volume variables

#ifndef BEATGENERATION_H
#define BEATGENERATION_H

#define MIN_BPM 40
#define DEFAULT_BPM 120
#define MAX_BPM 300
#define NUMBER_OF_BEATS 3
#define EIGHTH_NOTES_PER_BAR 8
#define DEFAULT_BEAT 1

void beatGen_incrementBPM();    //increment the current BPM by 5
void beatGen_decrementBPM();    //decrement the current BPM by 5
int beatGen_getBPM();
int beatGen_getMode();          //get the current beat mode

void beatGen_init();        //initializes the beat generation thread, must be called before any other functions
void beatGen_cleanup();

// Default is the standard rock beat.
// Order of cycle is: standard rock beat, custom beat(s), none (then repeat). 
void beatGen_cycleBeat();


#endif