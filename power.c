#include <time.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "tcb.h"

#define BATTERY_LEVEL       *(*(Data*)powerData).batteryLevelPtr
#define POWER_CONSUMPTION   *(*(Data*)powerData).powerConsumptionPtr
#define POWER_GENERATION    *(*(Data*)powerData).powerGenerationPtr
#define SOLAR_PANEL_STATE   *(*(Data*)powerData).solarPanelStatePtr
#define SOLAR_PANEL_DEPLOY  *(*(Data*)powerData).solarPanelDeployPtr
#define SOLAR_PANEL_RETRACT *(*(Data*)powerData).solarPanelRetractPtr
#define BATTERY_TEMP        *(*(Data*)powerData).batteryTempPtr
#define BATTERY_OVER_TEMP   *(*(Data*)powerData).batteryOverTempPtr

double timediff;
int firstRun;
int terminal0;
unsigned short motorDrive;
bool solarPanelFullOff;
pthread_t thread_id1;
bool panelThreadCreated;
void batteryTemp(void);

/******************************************************************************
* function name:power
* function inputs: pointer to void
* function outputs: void
* function description: power controls the batterylevel and solar 
						panel states of satellite system.
* author: Griffin Wu
******************************************************************************/
void power(void* powerData) {
    if (fmod(timediff, 5.0) == 0.0 && firstRun) {
        firstRun = 0;
        dprintf(terminal0, "motorDrive is %i\n", motorDrive);
        // local variables
        static int calls1 = 0;
        static int calls2 = 0;
        static int up = 1;
        static unsigned short panelDrive = 0;
        solarPanelFullOff = (bool)(0 == panelDrive);
        // enable the ADC ports
        FILE* ain0;
        unsigned int value;
        // raise(SIGUSR2);
        usleep(600);
		ain0 = fopen("/sys/devices/ocp.3/helper.15/AIN0", "r");
		fseek(ain0,0,SEEK_SET); // go to beginning of buffer
		fscanf(ain0, "%u", &value); // write analog value to buffer
		fclose(ain0); // close buffer
        unsigned int batteryLevelHistory[16];
        int i;
        for (i = 15; i > 0; i--) {
            batteryLevelHistory[i] = batteryLevelHistory[i - 1];
        }
        batteryLevelHistory[0] = (unsigned int)round(value / 15.0);
        BATTERY_LEVEL = batteryLevelHistory[0];

        // powerConsumption part
        if (POWER_CONSUMPTION < 5) {
            if (calls1 == 0) {
                POWER_CONSUMPTION+=2;
            } else {
                (POWER_CONSUMPTION)--;
            }
            up = 1;
        } else if (POWER_CONSUMPTION > 10) {
            if (calls1 == 0) {
                POWER_CONSUMPTION+=-2;
            } else {
                POWER_CONSUMPTION+=1;
            }
            up = 0;
        } else {
            if (up == 1) {
                if (calls1 == 0) {
                    POWER_CONSUMPTION+=2;
                } else {
                    POWER_CONSUMPTION+=-1;
                }
            } else {
                if (calls1 == 0) {
                    POWER_CONSUMPTION+=-2;
                } else {
                    POWER_CONSUMPTION+=1;
                }
            }
        }
        
        // powerGeneration part and battery level
        if (SOLAR_PANEL_STATE) {
            if (BATTERY_LEVEL > 95) {
                SOLAR_PANEL_STATE = false;
                POWER_GENERATION = 0;
            } else if(BATTERY_LEVEL < 50) {
                if (calls2 == 0) {
                    POWER_GENERATION+=2;
                } else {
                    POWER_GENERATION+=1;
                }
            } else {
                if (calls2 == 0) {
                    POWER_GENERATION+=2;
                }
            }
        } else {
            if (BATTERY_LEVEL <= 10) {
                SOLAR_PANEL_STATE = true;
            }
        }

        // panel deployment and retraction control
        if (SOLAR_PANEL_STATE) {
            if (panelDrive != 100) {
                SOLAR_PANEL_DEPLOY = true;
            }
        } else if (BATTERY_LEVEL > 95) { // state is false
            if (panelDrive != 0) {
                SOLAR_PANEL_RETRACT = true;
            }
        }

        if (SOLAR_PANEL_DEPLOY) {
            panelDrive += motorDrive;
        } else if (SOLAR_PANEL_RETRACT) {
            panelDrive -= motorDrive;
        }     // if both are false, do nothing

        if (panelDrive > 100) {
            if (SOLAR_PANEL_DEPLOY) {       // full Deployment
                panelDrive = 100;
                SOLAR_PANEL_DEPLOY = false;
            } else {                               // full retraction
                panelDrive = 0;
                SOLAR_PANEL_RETRACT = false;
            }
        }

         dprintf(terminal0, "paneldeploy percentage: %i%%\n", panelDrive);
        // if (panelDrive == 100) {
        //     dprintf(terminal0, "solarPenel Fully deployed\n");
        // } else if (panelDrive == 0) {
        //     dprintf(terminal0, "solarPenel Fully retracted\n");
        // }
        // if (*data.batteryLevelPtr > 101 && *data.powerGenerationPtr < *data.powerConsumptionPtr) {
        //     *data.batteryLevelPtr = 0;
        // }

        // if (*data.batteryLevelPtr > 101 && *data.powerGenerationPtr > *data.powerConsumptionPtr) {
        //     *data.batteryLevelPtr = 100;
        // }

        calls1+=1;
        calls2+=1;

        if (calls1 == 2) {
            calls1 = 0;
        }

        if (calls2 == 2) {
            calls2 = 0;
        }
    }

    if (solarPanelDeploy || solarPanelRetract) {
        if (!panelThreadCreated) {
            int rc1 = pthread_create(&thread_id1, NULL, solarPanelControl, solarPanelControlTCB.taskDataPtr);
            panelThreadCreated = true;
            if (rc1) {
                printf("ERROR, thread12 create failed\n");
            }
        }
    }
    // create signal to exit or create battery temperature thread.
    if (solarPanelState) {
        batteryTemp();
    }
	return;
}

void batteryTemp(void) {
    double battTemp;
    static int location = 1;
    unsigned int value;
    switch(location) {
        case 1: {
            FILE* ain1;
            ain1 = fopen("/sys/devices/ocp.3/helper.15/AIN1", "r");
            fseek(ain1,0,SEEK_SET); // go to beginning of buffer
            fscanf(ain1, "%u", &value); // write analog value to buffer
            fclose(ain1); // close buffer
            location = 2;
            break;
        }
        case 2: {
            FILE* ain2;
            ain2 = fopen("/sys/devices/ocp.3/helper.15/AIN2", "r");
            fseek(ain2,0,SEEK_SET); // go to beginning of buffer
            fscanf(ain2, "%u", &value); // write analog value to buffer
            fclose(ain2); // close buffer
            location = 1;
            break;
        }
    }
    double batteryTempHistory[16];
    int i;
    for (i = 15; i > 0; i--) {
        batteryTempHistory[i] = batteryTempHistory[i - 1];
    }
    batteryTempHistory[0] = (double)value / 100;
    battTemp = batteryTempHistory[0];
    BATTERY_TEMP = (unsigned int)(32 * battTemp + 33);
    if (location == 1) {
        if (battTemp > batteryTempHistory[1] * 1.2 || 
            battTemp > batteryTempHistory[2] * 1.2) {
            BATTERY_OVER_TEMP = true;
        }
    } else {
        if (battTemp > batteryTempHistory[2] * 1.2 || 
            battTemp > batteryTempHistory[3] * 1.2) {
            BATTERY_OVER_TEMP = true;
        }
    }
    usleep(500);
    return NULL;
}