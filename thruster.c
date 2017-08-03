
// task of thruster subsystem

#include "tcb.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <signal.h>

#define FUEL_LEVEL       *(*(Data*)thrusterData).fuelLevelPtr
#define THRUSTER_COMMAND *(*(Data*)thrusterData).thrusterCommandPtr




int terminal0;
double timediff;
int firstRun;
char isrChar;
void PWMSignal(double);
/******************************************************************************
* function name: thrusterSubsystem
* function inputs: a pointer to void
* function outputs: a void
* function description: this function takes information from satelliteComs to 
                        control the direction, magnitude and duration of 
                        satellite's thruster.
* author: Yayin Huang
******************************************************************************/
void thruster(void* thrusterData) {
    if (0 == FUEL_LEVEL) {
        raise(SIGUSR1);
    }
    if (fmod(timediff, 5.0) == 0.0 && firstRun) {
        //  local varibles
        // unsigned short left;
        // unsigned short right;
        // unsigned short up;
        // unsigned short down;
        unsigned int magnitude;
        double magPrecent;
        unsigned int duration;
        
        // left = 0;
        // right = 0;
        // up = 0;
        // down = 0;
        
        //  mask value
        magnitude = (THRUSTER_COMMAND & 240) >> 4;
        magPrecent = (magnitude / 15.0) * 100;
        duration = (THRUSTER_COMMAND & 65280) >> 8;
        
        dprintf(terminal0, "interpretation: ");
        if((THRUSTER_COMMAND & 1) == 1 && (THRUSTER_COMMAND & 2) != 2) {  // left
            //left = 1;
            dprintf(terminal0, "left ");
        }
        if((THRUSTER_COMMAND & 2) == 2 && (THRUSTER_COMMAND & 1) != 1) {  // right
            //right = 1;
            dprintf(terminal0, "right ");
        }
        if((THRUSTER_COMMAND & 4) == 4 && (THRUSTER_COMMAND & 8) != 8) {  // up
            //up = 1;
            dprintf(terminal0, "up ");
        }
        if((THRUSTER_COMMAND & 8) == 8 && (THRUSTER_COMMAND & 4) != 4) {  // down
            //down = 1;
            dprintf(terminal0, "down ");
        }
        
        dprintf(terminal0, "mag:%f%%, dura:%is\n\n", magPrecent, duration);
        
        //  fuel level calculate (rate: 5% :406%), if magnitude is 100%, program can run 255s
        FUEL_LEVEL -= magnitude * duration * 0.001;
        PWMSignal(magPrecent);
    }
    if (FUEL_LEVEL <= 0) {
        FUEL_LEVEL = 0;
        isrChar = 't';
        raise(SIGINT);
    }
    return;
}

void PWMSignal(double magPrecent) {
    FILE *duty,*period,*run;

    period = fopen("/sys/devices/ocp.3/pwm_test_P8_19.17/period", "w");
    fseek(period,0,SEEK_SET);
    fprintf(period,"%d",500000000);
    fflush(period);

    period = fopen("/sys/devices/ocp.3/pwm_test_P8_19.17/polarity", "w");
    fseek(period,0,SEEK_SET);
    fprintf(period,"%d",0);
    fflush(period);

    duty = fopen("/sys/devices/ocp.3/pwm_test_P8_19.17/duty", "w");
    fseek(duty,0,SEEK_SET);
    fprintf(duty,"%d", (int)magPrecent * 5000000);
    fflush(duty);

    run = fopen("/sys/devices/ocp.3/pwm_test_P8_19.17/run", "w");
    fseek(run,0,SEEK_SET);
    fprintf(run,"%d",0);
    fflush(run);

    fseek(run,0,SEEK_SET);
    fprintf(run,"%d",1);
    fflush(run);

    fclose(duty);
    fclose(period);
    fclose(run);

    return;
}

