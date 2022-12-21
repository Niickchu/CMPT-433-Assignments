// outputStatistics.h
// Module to start and stop the output statistics thread
// Provides a function to initialize the output statistics thread, and start printing the output statistics every second
// Provides a function to clean up the output statistics thread
#ifndef OUTPUTSTATISTICS_H
#define OUTPUTSTATISTICS_H

void outputStatistics_Init();
void outputStatistics_Cleanup();

#endif