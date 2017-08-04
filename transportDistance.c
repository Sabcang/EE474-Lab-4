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

#define TRANSPORT_DIST = *(*(Data*)distData).transportDistPtr

bool edgeDetect(int);
void transportDistance(void* distData) {
    double frequency;
    FILE *sigReceiver;
    int value[256];
    int i;
    clock_t start_t, end_t;
    start_t = clock();
    for(i = 0; i < 256; i++) {
    	sigReceiver = fopen("/sys/class/gpio/gpio49", "r");
        fscanf(sigReceiver, "%d", &value[i]);
        fclose(sigReceiver);
    }
    end_t = clock();
    double totalT = (double)(end_t - start_t)/ CLOCKS_PER_SEC;
    int counter = 0;
    for (i = 0; i < 256; i++) {
        counter += edgeDetect(value[i]);
    }
    frequency = round(counter / totalT);

    TRANSPORT_DIST = 300000000 / frequency / 2;
    return NULL;
}

bool edgeDetect(int curr) {
    static prev = 0;
    bool result = false;
    if (0 == prev && 1 == curr) {
        result = true;
    }
    prev = curr;
    return result;
}