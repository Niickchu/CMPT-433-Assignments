// shutdown.h
// Module to signal shutdown and wait for shutdown mutexes.
// Provides functions to initialize the shutdown and to wait for shutdown.
#ifndef SHUTDOWN_H
#define SHUTDOWN_H

void init_shutdown();
void signalShutdown();
void waitForShutdown();

#endif