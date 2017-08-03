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


void transportDistance(void* distData) {
    unsigned int frequency;
    FILE *sigReceiver;
    int value;
    sigReceiver = fopen("/sys/class/gpio/gpio49", "r");
    fseek(sigReceiver,0,SEEK_SET);
    fscanf(sigReceiver, "%d", &value);
    return NULL;
}

void edgeDetect(int curr) {
    static prev = 0;
    bool result = false;
    if (0 == prev && 1 == curr) {
        result = true;
    }
    prev = curr;
    return result;
}