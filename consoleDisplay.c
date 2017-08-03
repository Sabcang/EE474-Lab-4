#include <string.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include "tcb.h"

#define SOLAR_PANEL_STATE *(*(Data*)displayData).solarPanelStatePtr
#define BATTERY_LEVEL     *(*(Data*)displayData).batteryLevelPtr
#define FUEL_LEVEL        *(*(Data*)displayData).fuelLevelPtr
#define POWER_CONSUMPTION *(*(Data*)displayData).powerConsumptionPtr
#define POWER_GENERATION  *(*(Data*)displayData).powerGenerationPtr
#define FUEL_LOW          *(*(Data*)displayData).fuelLowPtr
#define BATTERY_LOW       *(*(Data*)displayData).batteryLowPtr

int terminal0;
double timediff;
int firstRun;

int getMode(void);
/******************************************************************************
* function name: consoleDisplay
* function inputs: pointer to void
* function outputs: void
* function description: this function displays satellite's status onto the 
						console screen on satellite.
* author: Griffin Wu
******************************************************************************/
void consoleDisplay(void* displayData) {
    if (fmod(timediff, 5.0) == 0.0 && firstRun) {
        // main functions
        char e[13] = "not deployed";
        if (getMode()) {
            if (SOLAR_PANEL_STATE == true) {
                strncpy(e, "deployed", 9);
            }
            dprintf(terminal0, "solar panel state is %s\n", e);
            dprintf(terminal0, "battery level is %i\n", BATTERY_LEVEL);
            if (FUEL_LEVEL > 0) {
                dprintf(terminal0, "fuel level is %i\n", FUEL_LEVEL);
            } else {
                dprintf(terminal0, "fuel tank is empty!\n");
            }
            dprintf(terminal0, "powerConsumption is %i\n", POWER_CONSUMPTION);
            dprintf(terminal0, "powerGeneration is %i\n", POWER_GENERATION);
        } else {
            if (FUEL_LOW == true) {
                dprintf(terminal0, "warning!!!Fuel level is low!\n");
            } else {
                dprintf(terminal0, "Fuel level is normal. No warning\n");
            }
            if (BATTERY_LOW == true) {
                dprintf(terminal0, "warning!!!Battery level is low!\n");
            } else {
                dprintf(terminal0, "Battery level is normal. No warning\n");
            }
        }
    }
	return;
}

int getMode(void) {
    FILE *gpio67;
    char value;
    gpio67 = fopen("/sys/class/gpio/gpio67/value", "r");
    fseek(gpio67, 0, SEEK_SET);
    fscanf(gpio67, "%c", &value);
    fflush(gpio67);
    fclose(gpio67);
    return ('1' == value);
}
