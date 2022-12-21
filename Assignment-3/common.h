// common.h
// Module to include common functions between modules
// Provides a function to sleep for a given number of milliseconds.
// Provides a function to run a linux terminal command

#ifndef COMMON_H
#define COMMON_H

void sleepForMs(long long delayInMs);
void runCommand(char *command);

#endif