#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef TCB_H
#define TCB_H

typedef struct myStruct {
    void  (*myTask)(void*);
    void* taskDataPtr;
    int priority;
    struct myStruct* next;
    struct myStruct* prev;
} TCB;

typedef struct taksData {
    unsigned short* fuelLevelPtr;
    unsigned short* powerConsumptionPtr;
    unsigned short* powerGenerationPtr;
    unsigned short* transportDistPtr;
    bool* solarPanelStatePtr;
    bool* solarPanelDeployPtr;
    bool* solarPanelRetractPtr;
    bool* fuelLowPtr;
    bool* batteryLowPtr;
    bool* driveMotorSpeedIncPtr;
    bool* driveMotorSpeedDecPtr;
    bool* batteryOverTempPtr;
    bool* dockReadyPtr;
    bool* liftReadyPtr;
    unsigned int* batteryLevelPtr;
    unsigned int* thrusterCommandPtr;
    unsigned int* batteryTempPtr;
    unsigned int* imageDataRawPtr;
    unsigned int* imageDataPtr;
    unsigned int* mPtr;
    char* commandPtr;
    char* requestPtr;
    char* responsePtr;
    struct complex xPtr;
    struct complex wPtr;
} Data;

typedef struct complex{
    int *real, *imag;
} COMPLEX;

void insert(TCB* node);
void delete(TCB* node);

int getch(void);

void power(void*);
void* solarPanelControl(void*);
void thruster(void*);
void transportDistance(void*);
void imageCapture(void*);
void satelliteComs(void*);
void vehicle(void*);
void consoleDisplay(void*);
void* consoleKeypad(void*);
void warningAlarm(void*);

#endif
