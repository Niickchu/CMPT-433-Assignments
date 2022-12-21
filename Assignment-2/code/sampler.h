// sampler.h
// Module to sample light levels in the background (thread).
// It provides access to the raw samples and then deletes them.
#ifndef _SAMPLER_H_
#define _SAMPLER_H_
#define BUFFER_SIZE 2000

typedef struct
{
    int sampleValue;
    long long timestampInMicroS;
} samplerDatapoint_t;


// Begin/end the background thread which samples light levels.      init and cleanup
void Sampler_startSampling(void);
void Sampler_stopSampling(void);
void Sampler_readCurrentBuffer(double voltageBuffer[], long long timeBuffer[], int *bufferSize);

#endif