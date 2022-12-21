#include "shutdown.h"
#include "analysis.h"
#include "sampler.h"
#include "LED_output.h"
#include "pthread.h"
#include <stdio.h>
#include <stdlib.h>

static pthread_mutex_t shutdownMutex = PTHREAD_MUTEX_INITIALIZER;

void init_shutdown(){
    pthread_mutex_lock(&shutdownMutex);
}

void signalShutdown(){
    Analysis_stop();
    LEDOutput_stopDisplaying();
    Sampler_stopSampling();
    pthread_mutex_unlock(&shutdownMutex);
}

void waitForShutdown(){
    pthread_mutex_lock(&shutdownMutex);
    pthread_mutex_unlock(&shutdownMutex);
}