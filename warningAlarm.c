#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include "tcb.h"

#define FUEL_LEVEL        *(*(Data*)warningData).fuelLevelPtr
#define FUEL_LOW          *(*(Data*)warningData).fuelLowPtr
#define BATTERY_LEVEL     *(*(Data*)warningData).batteryLevelPtr
#define BATTERY_LOW       *(*(Data*)warningData).batteryLowPtr
#define BATTERY_OVER_TEMP *(*(Data*)warningData).batteryOverTempPtr

void setTimertoLED1(unsigned int);
void setTimertoLED2(unsigned int);
void ringAlarm();
void turnOffAlarm();
void resetLEDs();
void resetLED2();
void resetLED1();
int LED1RateIsNot(unsigned int);
int LED2RateIsNot(unsigned int);

time_t startAlarm, curr;
double timediff;
int firstRun;

/******************************************************************************
* function name: warningAlarm
* function inputs:  pointer to void
* function outputs: void
* function description: this function controls LED lights on the BBB board.
                        to display warning effect.
* author: Sam Shen
******************************************************************************/
void warningAlarm(void* warningData) {
    char const *triggerPath = "/sys/class/leds/beaglebone:green:usr3/trigger";
    if (FUEL_LEVEL > 50 && BATTERY_LEVEL > 50) {
        
        // set the LED3 to be on
        resetLEDs();
        FILE *trigger = fopen(triggerPath, "w+");
        fwrite("default-on", 1, sizeof("default-on"), trigger);
        fflush(trigger);
        fclose(trigger);
    } else {
        FILE *trigger = fopen(triggerPath, "w+");
        fwrite("none", 1, sizeof("none"), trigger);
        fflush(trigger);
        fclose(trigger);
        
        if (FUEL_LEVEL <= 10) {
            FUEL_LOW = true;
            if (LED1RateIsNot(1000)) {
                setTimertoLED1(1000);
            }
        } else if (FUEL_LEVEL <= 50) {
            if (LED1RateIsNot(2000)) {
                setTimertoLED1(2000);
            }
        }
        else {
            resetLED1();
        }
        
        if (BATTERY_LEVEL <= 10) {
            if (LED2RateIsNot(1000)) {
                setTimertoLED2(1000);
            }
        } else if (BATTERY_LEVEL <= 50) {
            if (LED2RateIsNot(2000)) {
                setTimertoLED2(2000);
            }
        }
        else {
            resetLED2();
        }
    } 
    if (BATTERY_LEVEL <= 10) {
        BATTERY_LOW = true;
    } else {
        BATTERY_LOW = false;
    }

    static bool created = false;
    if (BATTERY_OVER_TEMP) {
        if (!created) {
            time(&startAlarm);
            ringAlarm();
            created = true;
        }
        time(&curr);
        double duration = difftime(curr, startAlarm);
        if (duration >= 15) {
            if (fmod(timediff, 5.0) == 0.0 && firstRun) {
                static bool flash = true;
                switch(flash) {
                    case true: {
                        setTimertoLED1(100);
                        setTimertoLED2(100);
                        break;
                    }
                    case false: {
                        FILE *trig = fopen("/sys/class/leds/beaglebone:green:usr1/trigger", "w+");
                        fwrite("default-on", 1, sizeof("default-on"), trig);
                        fflush(trig);
                        fclose(trig);

                        trig = fopen("/sys/class/leds/beaglebone:green:usr2/trigger", "w+");
                        fwrite("default-on", 1, sizeof("default-on"), trig);
                        fflush(trig);
                        fclose(trig);
                        break;
                    }
                }
                flash = !flash;
            }
        }
    } else {
        turnOffAlarm();
        created = false;
    }
    return;
}

void ringAlarm() {
    // PWM signal to speaker.
    return;
}

void turnOffAlarm() {
    return;
}
/******************************************************************************
* function name: resetLED1
* function inputs: none
* function outputs: void
* function description: this function reset LED1 on BBB
* author: Sam Shen
******************************************************************************/
void resetLED1() {
    char const *triggerPath1 = "/sys/class/leds/beaglebone:green:usr1/trigger";
    FILE *trigger1 = fopen(triggerPath1, "w+");
    fwrite("none", 1, sizeof("none"), trigger1);
    fflush(trigger1);
    fclose(trigger1);
    return;
}

/******************************************************************************
* function name: resetLED2
* function inputs: none
* function outputs: void
* function description: this function reset LED2 on BBB
* author: Sam Shen
******************************************************************************/
void resetLED2() {
    char const *triggerPath2 = "/sys/class/leds/beaglebone:green:usr2/trigger";
    FILE *trigger2 = fopen(triggerPath2, "w+");
    fwrite("none", 1, sizeof("none"), trigger2);
    fflush(trigger2);
    fclose(trigger2);
    return;
}

/******************************************************************************
* function name: resetLEDs
* function inputs: none
* function outputs: void
* function description: this function reset all LEDs on BBB
* author: Sam Shen
******************************************************************************/
void resetLEDs() {
    char const *triggerPath1 = "/sys/class/leds/beaglebone:green:usr1/trigger";
    char const *triggerPath2 = "/sys/class/leds/beaglebone:green:usr2/trigger";
    char const *triggerPath3 = "/sys/class/leds/beaglebone:green:usr3/trigger";
    char const *triggerPath0 = "/sys/class/leds/beaglebone:green:usr0/trigger";
    
    FILE *trigger1 = fopen(triggerPath1, "w+");
    fwrite("none", 1, sizeof("none"), trigger1);
    fflush(trigger1);
    fclose(trigger1);
    
    FILE *trigger2 = fopen(triggerPath2, "w+");
    fwrite("none", 1, sizeof("none"), trigger2);
    fflush(trigger2);
    fclose(trigger2);
    
    FILE *trigger3 = fopen(triggerPath3, "w+");
    fwrite("none", 1, sizeof("none"), trigger3);
    fflush(trigger3);
    fclose(trigger3);
    
    FILE *trigger0 = fopen(triggerPath0, "w+");
    fwrite("none", 1, sizeof("none"), trigger0);
    fflush(trigger0);
    fclose(trigger0);
    return;
}

/******************************************************************************
* function name: LED1RateIsNot
* function inputs: unsigned int vlaue in unit of ms
* function outputs: int
* function description: checks if the LED1 is not at a certain rate
* author: Sam Shen
******************************************************************************/
int LED1RateIsNot(unsigned int value) {
    char delayTime[5];
    char cValue[5];
    FILE* delayOn;
    delayOn = fopen("/sys/class/leds/beaglebone:green:usr1/delay_on", "r");
    if (delayOn == NULL) {
        return 1;
    }
    fscanf(delayOn, "%s", delayTime);
    snprintf(cValue, 5, "%d", value/2);
    fflush(delayOn);
    fclose(delayOn);
    return strcmp(cValue, delayTime) != 0;
}

/******************************************************************************
* function name: LED2RateIsNot
* function inputs: unsigned int vlaue in unit of ms
* function outputs: int
* function description: checks if the LED2 is not at a certain rate
* author: Sam Shen
******************************************************************************/
int LED2RateIsNot(unsigned int value) {
    char delayTime[5];
    char cValue[5];
    FILE* delayOn;
    delayOn = fopen("/sys/class/leds/beaglebone:green:usr2/delay_on", "r");
    if (delayOn == NULL) {
        return 1;
    }
    fscanf(delayOn, "%s", delayTime);
    snprintf(cValue, 5, "%d", value/2);
    fflush(delayOn);
    fclose(delayOn);
    return strcmp(cValue, delayTime) != 0;
}

/******************************************************************************
* function name: setTimertoLED1
* function inputs: unsigned int in unit of ms
* function outputs: void
* function description:  set LED1 to flash at a rate of input value.
* author: Sam Shen
******************************************************************************/
void setTimertoLED1(unsigned int ms) {
    FILE *trigger;
    FILE *delayOn;
    FILE *delayOff;
    
    char const *triggerPath = "/sys/class/leds/beaglebone:green:usr1/trigger";
    
    trigger = fopen(triggerPath, "w+");
    fwrite("timer", 1, sizeof("timer"), trigger);
    fflush(trigger);
    fclose(trigger);
    
    char const *onPath = "/sys/class/leds/beaglebone:green:usr1/delay_on";
    char const *offPath = "/sys/class/leds/beaglebone:green:usr1/delay_off";
    
    char msString[50];
    snprintf(msString, 50, "%d", ms/2);
    delayOn = fopen(onPath, "w+");
    fwrite(msString, 1, sizeof(msString), delayOn);
    fflush(delayOn);
    fclose(delayOn);
    
    delayOff = fopen(offPath, "w+");
    fwrite(msString, 1, sizeof(msString), delayOff);
    fflush(delayOff);
    fclose(delayOff);
    return;
}

/******************************************************************************
* function name: setTimertoLED2
* function inputs: unsigned int in unit of ms
* function outputs: void
* function description:  set LED2 to flash at a rate of input value.
* author: Sam Shen
******************************************************************************/
void setTimertoLED2(unsigned int ms) {
    FILE *trigger;
    FILE *delayOn;
    FILE *delayOff;
    
    char const *triggerPath = "/sys/class/leds/beaglebone:green:usr2/trigger";
    
    trigger = fopen(triggerPath, "w+");
    fwrite("timer", 1, sizeof("timer"), trigger);
    fflush(trigger);
    fclose(trigger);
    
    char const *onPath = "/sys/class/leds/beaglebone:green:usr2/delay_on";
    char const *offPath = "/sys/class/leds/beaglebone:green:usr2/delay_off";
    
    char msString[50];
    snprintf(msString, 50, "%d", ms/2);
    delayOn = fopen(onPath, "w+");
    fwrite(msString, 1, sizeof(msString), delayOn);
    fflush(delayOn);
    fclose(delayOn);
    
    delayOff = fopen(offPath, "w+");
    fwrite(msString, 1, sizeof(msString), delayOff);
    fflush(delayOff);
    fclose(delayOff);
    return;
}
