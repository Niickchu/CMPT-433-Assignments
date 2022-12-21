#include "utilities.h"

void runCommand(char *command)
{
    FILE *pipe = popen(command, "r");

    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe))
    {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
            break;
    }

    int exitCode = WEXITSTATUS(pclose(pipe));

    if (exitCode != 0)
    {
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
}

bool readGPIO(int gpio)
{
   bool button = 0;
   char address[50];
   sprintf(address, "/sys/class/gpio/gpio%d/value", gpio);
   FILE *pFile = fopen(address, "r");
   if (pFile == NULL)
   {
      printf("ERROR: Unable to open file (%s) for read\n", address);
      exit(-1);
   }
   else
   {
      char buff[2];
      fgets(buff, 2, pFile);
      if (buff[0] == '1') // inverse the bits (by default 0 means pressed)
      {
        button = 1;
      }
      fclose(pFile);
   }
   return button;
}

long long getTimeInMs(void)
{
	struct timespec spec;
	clock_gettime(CLOCK_REALTIME, &spec);
	long long seconds = spec.tv_sec;
	long long nanoSeconds = spec.tv_nsec;
	long long microSeconds = seconds * 1000000
	+ nanoSeconds / 1000;
	return microSeconds;
}

void sleepForMs(long long delayInMs)
{
	const long long NS_PER_MS = 1000 * 1000;
	const long long NS_PER_SECOND = 1000000000;
	long long delayNs = delayInMs * NS_PER_MS;
	int seconds = delayNs / NS_PER_SECOND;
	int nanoseconds = delayNs % NS_PER_SECOND;
	struct timespec reqDelay = {seconds, nanoseconds};
	nanosleep(&reqDelay, (struct timespec *) NULL);
}

// Return the address of the PRU's base memory
volatile void* getPruMmapAddr(void)
{
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("ERROR: could not open /dev/mem");
        exit(EXIT_FAILURE);
    }

    // Points to start of PRU memory.
    volatile void* pPruBase = mmap(0, PRU_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU_ADDR);
    if (pPruBase == MAP_FAILED) {
        perror("ERROR: could not map memory");
        exit(EXIT_FAILURE);
    }
    close(fd);

    return pPruBase;
}

void freePruMmapAddr(volatile void* pPruBase)
{
    if (munmap((void*) pPruBase, PRU_LEN)) {
        perror("PRU munmap failed");
        exit(EXIT_FAILURE);
    }
}