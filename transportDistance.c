#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#include "tcb.h"

const int EMW_SPEED = 300000000;           // Electromagnetic wave speed in unit [m/s]

void transportDistance(void* distData) {
    unsigned int frequency;
    //frequency = some data from measurement
    *(*(Data*)distData).transportDistPtr = EMW_SPEED/frequency;
    return NULL;
}