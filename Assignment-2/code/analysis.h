// analysis.h
// Module to analyze the sampled data and to update the LED matrix struct with the current analysis results.
// Provides a function to initialize the analysis module and a function to stop analyzing the data.
// Will create a thread to analyze the data.

#ifndef ANALYSIS_H
#define ANALYSIS_H

void Analysis_init(void);
void Analysis_stop(void);

#endif