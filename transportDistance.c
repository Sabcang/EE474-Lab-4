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

int terminal0
bool edgeDetect(int);
void transportDistance(void* distData) {
    double frequency;
    FILE *sigReceiver;
    int value[256];
    int transportDistance[8];
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

    TRANSPORT_DIST = (unsigned short)(300000000 / frequency / 2);

    if(TRANSPORT_DIST>=(unsigned short)(1.1*transportDistance[0])
    |TRANSPORT_DIST<=(unsigned short)(0.9*transportDistance[0])) {
        //store in a circular 8 reading buffer
        for(i=15; i>0, i--) {
            transportDistance[i] = transportDistance[i-1];
        }
        transportDistance[0] = TRANSPORT_DIST;
    }
    
    dprintf(terminal0, "The distance between the satellite and the approaching transport is %i m\n", TRANSPORT_DIST);

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