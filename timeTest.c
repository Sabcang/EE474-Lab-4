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

bool fuelLow, batteryLow, solarPanelState, solarPanelDeploy, solarPanelRetract, 
     driveMotorSpeedInc, driveMotorSpeedDec, solarPanelFullOff, batteryOverTemp;
unsigned int thrusterCommand, batteryTemp;
unsigned int batteryLevel;
unsigned int* batteryLevelPtr, *imageDataRawPtr, *imageDataPtr;
unsigned short fuelLevel, powerConsumption, powerGeneration, motorDrive, transportDist;
char command, response;
// other global variable
int terminal0, terminal1, firstRun, fd0;
char* myfifo0 = "/tmp/myfifo0";
double timediff;
time_t timestart, currtime;
rw coms;
TCB  powerTCB, solarPanelControlTCB, thrusterTCB, satelliteComsTCB, 
     satelliteComsTCB2, vehicleComsTCB, consoleDisplayTCB, consoleKeypadTCB, 
     warningAlarmTCB, batteryTempTCB, transportDistanceTCB;

Data powerData, solarPanelControlData, thrusterData, satelliteComsData, 
     vehicleComsData, consoleDisplayData, consoleKeypadData, warningAlarmData,
     batteryTempData, transportDistanceData;

TCB *head, *tail, *aTCBPtr;

int main(void) {
    startup();
    int i;
    for(i = 0; i < 100; i++) {
        power(powerTCB.taskDataPtr);
    }
}