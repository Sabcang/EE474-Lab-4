#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
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
#include "startup.h"

//hahahah
//sagduashdi

// other global variable
int firstRun;
double timediff;
time_t timestart, currtime;

TCB *tail, *aTCBPtr;
/******************************************************************************
* function name: main
* function inputs:  void
* function outputs: int
* function description:
* author: Sam Shen, Griffin Wu, Yayin Huang
******************************************************************************/
int main(void) {
    startup();
    int priority = 1;
    while(1) {

        // thread
        static bool panelThreadCreated = false;
        
        if (solarPanelDeploy || solarPanelRetract) {
            if (!panelThreadCreated) {
                int rc1 = pthread_create(&thread_id1, NULL, solarPanelControl, solarPanelControlTCB.taskDataPtr);
                panelThreadCreated = true;
                if (rc1) {
                    printf("ERROR, thread12 create failed\n");
                }
            }
        }

        // scheduler 
        time(&currtime);
        timediff = difftime(currtime, timestart);
        if (fmod(timediff,5) == 4) {
            firstRun = 1;
        }
//你好
        // task queue
        while(aTCBPtr != tail) {
            if (priority == aTCBPtr->priority) {
                aTCBPtr->myTask(aTCBPtr->taskDataPtr);
            }
            aTCBPtr = aTCBPtr->next;
        }
        if (priority == aTCBPtr->priority) {
            aTCBPtr->myTask(aTCBPtr->taskDataPtr);
        }
        aTCBPtr = aTCBPtr->next;
        priority++;
        if (priority == 6) {
          priority = 1;
        }
    }
}
