//This module has all common functions
#ifndef UTILITIES_H
#define UTILITIES_H

#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <ctype.h>
// General PRU Memomry Sharing Routine
// ----------------------------------------------------------------
#define PRU_ADDR      0x4A300000   // Start of PRU memory Page 184 am335x TRM
#define PRU_LEN       0x80000      // Length of PRU memory
#define PRU0_DRAM     0x00000      // Offset to DRAM
#define PRU1_DRAM     0x02000
#define PRU_SHAREDMEM 0x10000      // Offset to shared memory
#define PRU_MEM_RESERVED 0x200     // Amount used by stack and heap

// Convert base address to each memory section
#define PRU0_MEM_FROM_BASE(base) ( (base) + PRU0_DRAM + PRU_MEM_RESERVED)
#define PRU1_MEM_FROM_BASE(base) ( (base) + PRU1_DRAM + PRU_MEM_RESERVED)
#define PRUSHARED_MEM_FROM_BASE(base) ( (base) + PRU_SHAREDMEM)


void runCommand(char *command);
bool readGPIO(int gpio);
long long getTimeInMs(void);
void sleepForMs(long long delayInMs);
volatile void* getPruMmapAddr(void);
void freePruMmapAddr(volatile void* pPruBase);


#endif