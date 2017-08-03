// task of satelliteComes

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

#define THRUSTER_COMMAND  *(*(Data*)satelliteComs).thrusterCommandPtr
#define SOLAR_PANEL_STATE *(*(Data*)satelliteComs).solarPanelStatePtr
#define BATTERY_LEVEL     *(*(Data*)satelliteComs).batteryLevelPtr
#define BATTERY_LOW       *(*(Data*)satelliteComs).batteryLowPtr
#define FUEL_LEVEL        *(*(Data*)satelliteComs).fuelLevelPtr
#define FUEL_LOW          *(*(Data*)satelliteComs).fuelLowPtr
#define POWER_CONSUMPTION *(*(Data*)satelliteComs).powerConsumptionPtr
#define POWER_GENERATION  *(*(Data*)satelliteComs).powerGenerationPtr

int terminal1;
double timediff;
int firstRun;


/******************************************************************************
* function name: satelliteComs
* function inputs: pointer to void
* function outputs: void
* function description: This function takes information from earth. Send info
						back to earth.
* author: Yayin Huang
******************************************************************************/

void satelliteComs(void* satelliteComs) {
    if (fmod(timediff, 5.0) == 0.0 && firstRun) {
         //Model thruster command from "earth" to satellite
        srand(time(NULL));   // should only be called once
        // returns a pseudo-random integer between 0 and RAND_MAX
        THRUSTER_COMMAND = rand()%65532;
        
        //statue information to "earth" from satellite
        if(SOLAR_PANEL_STATE) {
            dprintf(terminal1, "Solar panel state deployed\n");
        } else {
            dprintf(terminal1, "Solar panel state not deployed\n");
        }
        
        if(FUEL_LOW) {
            dprintf(terminal1, "Fuel level low!\n");
        }
        if(BATTERY_LOW) {
            dprintf(terminal1, "Battery level low!\n");
        }
        dprintf(terminal1, "battery level is %i\n", BATTERY_LEVEL);
        if (FUEL_LEVEL > 0) {
            dprintf(terminal1, "fuel level is %i\n", FUEL_LEVEL);
        } else {
            dprintf(terminal1, "fuel tank is empty!\n");
        }
        dprintf(terminal1, "power consumption is %i\n", POWER_CONSUMPTION);
        dprintf(terminal1, "power generation is %i\n\n", POWER_GENERATION);
}