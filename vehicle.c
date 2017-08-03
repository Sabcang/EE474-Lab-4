// task of vehicle communication
// taking another argument for pipe usage

#include "tcb.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

#define COMMAND     *(*(Data*)vehicleData).commandPtr
#define RESPONSE    *(*(Data*)vehicleData).commandPtr
#define DOCK_READY  *(*(Data*)vehicleData).dockReadyPtr
#define LIFT_READY  *(*(Data*)vehicleData).liftReadyPtr

int fd0;
double timediff;
int firstRun;
int terminal2;
int terminal0;

void vehicle(void* vehicleData) {
    char temp[1];
    read(fd0, temp, 1);
    switch(temp[0]) {
      case 'F': dprintf(terminal2, "successfully received forward command"); break;		
      case 'B': dprintf(terminal2, "successfully received back command"); break;		
      case 'L': dprintf(terminal2, "successfully received left command"); break;
      case 'R': dprintf(terminal2, "successfully received right command"); break;
      case 'D': {
        if (DOCK_READY) {
          dprintf(terminal0, "C--Confirm dock");
          DOCK_READY = false;
          LIFT_READY = true;
        } else {
          dprintf(terminal2, "successfully received drill down start command");
        }
        break;	
      }
      case 'H': dprintf(terminal2, "successfully received drill up stop command"); break;
      case 'C': {
        if (LIFT_READY) {
          dprintf(terminal0, "K--OK to lift off");
          LIFT_READY = false;
        }
        break;
      }
      default: {
        dprintf(terminal0, "Not a correct request received");
        dprintf(terminal2, "Not a correct command received");
      }
  }
  return;
}
