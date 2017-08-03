#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include "tcb.h"

#define DRIVE_MOTOR_SPEED_INC *(*(Data*)solarPanelData).driveMotorSpeedIncPtr
#define DRIVE_MOTOR_SPEED_DEC *(*(Data*)solarPanelData).driveMotorSpeedDecPtr


unsigned short motorDrive;
void motorDrivePWM(unsigned short);
bool solarPanelDeploy;
bool solarPanelRetract;
/******************************************************************************
* function name: solarPanelControl
* function inputs:  pointer to void
* function outputs: void
* function description: manages the deployment and retraction of the 
* satellite’s solar panels.
* author: Sam Shen
******************************************************************************/
void* solarPanelControl(void* solarPanelData) {
	while (solarPanelDeploy || solarPanelRetract) {
		bool valueChanged = false;
        if (DRIVE_MOTOR_SPEED_INC) {
            motorDrive += 5;
			if (motorDrive > 100) {
				motorDrive = 100;
			}
			valueChanged = true;
            DRIVE_MOTOR_SPEED_INC = false;
        } else if (DRIVE_MOTOR_SPEED_DEC) {
            motorDrive -= 5;
			if (motorDrive > 100) {
				motorDrive = 0;
			}
			valueChanged = true;
            DRIVE_MOTOR_SPEED_DEC = false;
        }
		if (valueChanged) {
			motorDrivePWM(motorDrive);
			valueChanged = false;
		}
	}
	motorDrive = 5;
	motorDrivePWM(motorDrive);
	isrChar = 'e';
	raise(SIGINT);
    return NULL;
}

void motorDrivePWM(unsigned short motorDrive) {
    FILE *duty, *run;

	duty = fopen("/sys/devices/ocp.3/pwm_test_P8_13.16/duty", "w");
	fseek(duty,0,SEEK_SET);
	fprintf(duty,"%d", motorDrive * 5000000);
	fflush(duty);

	run = fopen("/sys/devices/ocp.3/pwm_test_P8_13.16/run", "w");

	fseek(run,0,SEEK_SET);
	fprintf(run,"%d",0);
	fflush(run);

	fseek(run,0,SEEK_SET);
	fprintf(run,"%d",1);
	fflush(run);

	fclose(duty);
	fclose(run);

	return;
}