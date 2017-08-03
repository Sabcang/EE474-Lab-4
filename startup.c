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

// #define DEBUG

// global data variables
bool fuelLow, batteryLow, solarPanelState, solarPanelDeploy, solarPanelRetract, 
     driveMotorSpeedInc, driveMotorSpeedDec, solarPanelFullOff, batteryOverTemp
     dockReady, liftReady, panelThreadCreated;
unsigned int thrusterCommand, batteryTemp, batteryLevel, m;
unsigned int* batteryLevelPtr, *imageDataRawPtr, *imageDataPtr, *mPtr;
unsigned short fuelLevel, powerConsumption, powerGeneration, motorDrive, transportDist;
char command, response;

// other global variable
int terminal0, terminal1, terminal2, firstRun, fd0, rc1, rc2;
char* myfifo0;
double timediff;
time_t timestart, currtime;
COMPLEX x, w;
COMPLEX *xPtr, *wPtr;

TCB  powerTCB, solarPanelControlTCB, thrusterTCB, satelliteComsTCB, 
     vehicleComsTCB, consoleDisplayTCB, consoleKeypadTCB, 
     warningAlarmTCB, transportDistanceTCB, imageCaptureTCB;

Data powerData, solarPanelControlData, thrusterData, satelliteComsData, 
     vehicleComsData, consoleDisplayData, consoleKeypadData, warningAlarmData,
     transportDistanceData, imageCaptureData;

TCB *head, *tail, *aTCBPtr;

pthread_t thread_id1;

void startup(void) {
    printf("."); fflush(stdout);
    #ifndef DEBUG 
    sleep(1); 
    #endif
    printf("."); fflush(stdout);
    #ifndef DEBUG
    sleep(1);
    #endif
    printf(".\n"); 
    #ifndef DEBUG
    sleep(1);
    #endif
    printf("Configuring and activating system time base...  "); fflush(stdout);
    #ifndef DEBUG
    sleep(1);
    #endif
    printf("completed\n\n");
    time(&timestart);
    printf("initializing satellite system data...  "); fflush(stdout);
    #ifndef DEBUG
    sleep(1);
    #endif
    initData();
    printf("completed\n\n");
    #ifndef DEBUG
    sleep(1);
    #endif
    printf("creating a \nd initializing all statically scheduled tasks...  "); fflush(stdout);
    #ifndef DEBUG
    sleep(1);
    #endif
    defineTCBs();
    printf("completed\n\n");
    #ifndef DEBUG
    sleep(1);
    #endif
    printf("initializing user signals...  "); fflush(stdout);
    handleSig();
    #ifndef DEBUG
    sleep(1);
    #endif
    printf("completed\n\n");
    #ifndef DEBUG
    sleep(1);
    #endif
    printf("configure and initialize all hardware subsystem...  "); fflush(stdout);
    enableADC();
    sleep(1);
    enablePWM();
    sleep(1);
    enableModeSel();
    #ifndef DEBUG
    sleep(1);
    #endif
    printf("completed\n\n");
    #ifndef DEBUG
    sleep(1);
    #endif
    printf("finalizing startup setting...  "); fflush(stdout);
    #ifndef DEBUG
    sleep(1);
    #endif
    restOfStartup();
    printf("completed\n\n");
    #ifndef DEBUG
    sleep(1);
    #endif
    printf("start operation!\n\n");

    return;
}

void defineTCBs(void) {
    // define all the pointers in TCBs

    // 1.define pointers for power subsystem
    powerTCB.myTask = &power;
    powerTCB.taskDataPtr = &powerData;
    powerTCB.priority = 5;
    powerTCB.next = NULL;
    powerTCB.prev = NULL;
    powerData.batteryLevelPtr = batteryLevelPtr;
    powerData.powerConsumptionPtr = &powerConsumption;
    powerData.powerGenerationPtr = &powerGeneration;
    powerData.solarPanelStatePtr = &solarPanelState;
    powerData.solarPanelDeployPtr = &solarPanelDeploy;
    powerData.solarPanelRetractPtr = &solarPanelRetract;
    powerData.batteryTempPtr = &batteryTemp;
    powerData.batteryOverTempPtr = &batteryOverTemp;

    // 2.define pointers for solar panel control subsystem
    solarPanelControlTCB.taskDataPtr = &solarPanelControlData;
    solarPanelControlTCB.next = NULL;
    solarPanelControlTCB.prev = NULL;
    solarPanelControlData.solarPanelStatePtr = &solarPanelState;
    solarPanelControlData.solarPanelDeployPtr = &solarPanelDeploy;
    solarPanelControlData.solarPanelRetractPtr = &solarPanelRetract;
    solarPanelControlData.driveMotorSpeedIncPtr = &driveMotorSpeedInc;
    solarPanelControlData.driveMotorSpeedDecPtr = &driveMotorSpeedDec;
   
    // 3.define pointers for thruster subsystem
    thrusterTCB.myTask = &thruster;
    thrusterTCB.taskDataPtr = &thrusterData;
    thrusterTCB.priority = 3;
    thrusterTCB.next = NULL;
    thrusterTCB.prev = NULL;
    thrusterData.thrusterCommandPtr = &thrusterCommand;
    thrusterData.fuelLevelPtr = &fuelLevel;
    
    // 4.define pointers for satellite communication subsystem
    satelliteComsTCB.myTask = &satelliteComs;
    satelliteComsTCB.taskDataPtr = &satelliteComsData;
    satelliteComsTCB.priority = 2;
    satelliteComsTCB.next = NULL;
    satelliteComsTCB.prev = NULL;
    satelliteComsData.fuelLowPtr = &fuelLow;
    satelliteComsData.batteryLowPtr = &batteryLow;
    satelliteComsData.solarPanelStatePtr = &solarPanelState;
    satelliteComsData.batteryLevelPtr = batteryLevelPtr;
    satelliteComsData.fuelLevelPtr = &fuelLevel;
    satelliteComsData.powerConsumptionPtr = &powerConsumption;
    satelliteComsData.powerGenerationPtr = &powerGeneration;
    satelliteComsData.thrusterCommandPtr = &thrusterCommand;


    // 5.define pointers for vehicle communication subsystem    
    vehicleComsTCB.myTask = &vehicle;
    vehicleComsTCB.taskDataPtr = &vehicleComsData;
    vehicleComsTCB.priority = 3;
    vehicleComsTCB.next = NULL;
    vehicleComsTCB.prev = NULL;
    vehicleComsData.commandPtr = NULL;
    vehicleComsData.requestPtr = NULL;
    vehicleComsData.responsePtr = NULL;
    vehicleComsData.dockReadyPtr = &dockReady;
    vehicleComsData.liftReadyPtr = &liftReady;


    // 6.define pointers for consoleDisplay subsystem
    consoleDisplayTCB.myTask = &consoleDisplay;
    consoleDisplayTCB.taskDataPtr = &consoleDisplayData; 
    consoleDisplayTCB.priority = 2;
    consoleDisplayTCB.next = NULL;
    consoleDisplayTCB.prev = NULL;
    consoleDisplayData.fuelLowPtr = &fuelLow;
    consoleDisplayData.batteryLowPtr = &batteryLow;
    consoleDisplayData.batteryLevelPtr = batteryLevelPtr;
    consoleDisplayData.solarPanelStatePtr = &solarPanelState;
    consoleDisplayData.fuelLevelPtr = &fuelLevel;
    consoleDisplayData.powerConsumptionPtr = &powerConsumption;
    consoleDisplayData.powerGenerationPtr = &powerGeneration;
    consoleDisplayData.batteryTempPtr = &batteryTemp;
    consoleDisplayData.transportDistPtr = &transportDist;

    // 7.define pointers for console keypad subsystem
    consoleKeypadTCB.taskDataPtr = &consoleKeypadData;
    consoleKeypadTCB.next = NULL;
    consoleKeypadTCB.prev = NULL;
    consoleKeypadData.solarPanelDeployPtr = &solarPanelDeploy;
    consoleKeypadData.solarPanelRetractPtr = &solarPanelRetract;
    consoleKeypadData.driveMotorSpeedIncPtr = &driveMotorSpeedInc;
    consoleKeypadData.driveMotorSpeedDecPtr = &driveMotorSpeedDec;

    // 8.define pointers for warning subsystem
    warningAlarmTCB.myTask = &warningAlarm;
    warningAlarmTCB.taskDataPtr = &warningAlarmData;
    warningAlarmTCB.priority = 1;
    warningAlarmTCB.next = NULL;
    warningAlarmTCB.prev = NULL;
    warningAlarmData.fuelLowPtr = &fuelLow;
    warningAlarmData.batteryLowPtr = &batteryLow;
    warningAlarmData.batteryLevelPtr = batteryLevelPtr;
    warningAlarmData.fuelLevelPtr = &fuelLevel;
    warningAlarmData.batteryOverTempPtr = &batteryOverTemp;

    // 9. define pointer for image capture subsystem
    imageCaptureTCB.myTask = &imageCapture;
    imageCaptureTCB.taskDataPTr = &imageCaptureData;
    imageCatpureTCB.priority = 4;
    imageCaptureTCB.prev = NULL;
    imageCaptureTCB.next = NULL;
    imageCaptureData.imageDataRawPtr = NULL;
    imageCaptureData.imageDataPtr = NULL;
    imageCaptureData.xPtr = x&;
    imageCaptureData.wPtr = w&;
    imageCaptureData.mPtr = m&;

    // 10. define pointers for transport distance subsystem
    transportDistanceTCB.myTask = &transportDistance;
    transportDistanceTCB.taskDataPtr = &transportDistanceData;
    transportDistanceTCB.priority = 4;
    transportDistanceTCB.next = NULL;
    transportDistanceTCB.prev = NULL;
    transportDistanceData.transportDistPtr = &transportDist;
    

    return;
}

void initData(void) {
    // initialize values
    thrusterCommand = 0;
    batteryLevel = 100;
    batteryLevelPtr = &batteryLevel;
    fuelLevel = 100;
    powerConsumption = 0;
    powerGeneration = 0;
    transportDist = 2000;
    solarPanelState = false;
    solarPanelDeploy = false;
    solarPanelRetract = false;
    fuelLow = false;
    batteryLow = false;
    batteryOverTemp = false;
    dockReady = false;
    liftReady = false;
    batteryTemp = 0;
    driveMotorSpeedInc = false;
    driveMotorSpeedDec = false;
    panelThreadCreated = false;
    motorDrive = 5;
    thread_id1 = NULL;
    
    // open up communication terminals.
    terminal0 = open("/dev/pts/0", O_WRONLY);
    terminal1 = open("/dev/pts/1", O_WRONLY);
    terminal2 = open("/dev/pts/2", O_WRONLY);
    return;
}

void handleSig(void) {
    signal(SIGINT, intHandler);
    return;
}

void restOfStartup() {
    // initialize the pipe for data transfer
    mkfifo(myfifo0, 0666);
    fd0 = open(myfifo0, O_RDWR);
    if (-1 == fd0) {
        printf("error making FIFO file!");
    }
    // the dynamic loop
    insert(&warningAlarmTCB);
    insert(&consoleDisplayTCB);
    insert(&satelliteComsTCB);
    insert(&vehicleComsTCB);
    insert(&thrusterTCB);
    insert(&powerTCB);
    insert(&imageCaptureTCB);
    insert(&transportDistanceTCB);
    #ifndef DEBUG
    sleep(2);
    #endif
    printf("All initialization completed.\n");
    #ifndef DEBUG
    sleep(1);
    #endif
    printf("Communication connected.\n");
    #ifndef DEBUG
    sleep(1);
    #endif
    printf("Connection established.\n");
    firstRun = 1;
    aTCBPtr = head;
    coms = wrt;
    printf("system ready! Activate in 5 seconds\n");
    #ifndef DEBUG
    int k;
    for (k = 5; k > 0; k--) {
  	  printf("%i ", k);
          fflush(stdout);
	  sleep(1);
    }
    #endif
    printf("start!!!\n");

    pthread_t thread_id2 = NULL;
    int rc2 = pthread_create(&thread_id2, NULL, consoleKeypad, consoleKeypadTCB.taskDataPtr);
    if (rc2) {
        printf("ERROR, thread12 create failed\n");
    }
    return;
}

// enable ADC which creates a helper.15 directory in directory /sys/devices/ocp.3/
// inside of helper.15, there are AIN0, AIN1, etc.
void enableADC(void) {
    FILE *ain;
    ain = fopen("/sys/devices/bone_capemgr.9/slots", "w");
    fseek(ain,0,SEEK_SET);
    fprintf(ain,"cape-bone-iio");
    fflush(ain);
    fclose(ain);
    return;
}

// enable PWM which creates a pwm_test_P8_13.16 directory in directory /sys/devices/ocp.3/
// inside of pwm_test_P8_13.16, there are period, duty, run, etc.
void enablePWM(void) {
    FILE *pwm, *thruster, *motor, *tDuty, *mDuty, *period;
    pwm = fopen("/sys/devices/bone_capemgr.9/slots", "w");
    fseek(pwm,0,SEEK_SET);
    fprintf(pwm,"am33xx_pwm");
    fflush(pwm);
    fprintf(pwm,"bone_pwm_P8_13");
    fflush(pwm);
    fprintf(pwm,"bone_pwm_P8_19");
    fflush(pwm);
    fclose(pwm);

    sleep(1);
    period = fopen("/sys/devices/ocp.3/pwm_test_P8_13.16/period", "w");
	fseek(period,0,SEEK_SET);
	fprintf(period,"%d",500000000);
	fflush(period);

	period = fopen("/sys/devices/ocp.3/pwm_test_P8_13.16/polarity", "w");
	fseek(period,0,SEEK_SET);
	fprintf(period,"%d",0);
	fflush(period);

    mDuty = fopen("/sys/devices/ocp.3/pwm_test_P8_13.16/duty", "w");
	fseek(mDuty,0,SEEK_SET);
	fprintf(mDuty,"%d", 5000000);
	fflush(mDuty);
    fclose(mDuty);

    motor = fopen("/sys/devices/ocp.3/pwm_test_P8_13.16/run", "w");
    fseek(motor,0,SEEK_SET);
    fprintf(motor,"%d",0);
    fflush(motor);
    fseek(motor,0,SEEK_SET);
    fprintf(motor,"%d",1);
    fflush(motor);
    fclose(motor);

        // reset thruster signal to 0
    tDuty = fopen("/sys/devices/ocp.3/pwm_test_P8_19.17/duty", "w");
	fseek(tDuty,0,SEEK_SET);
	fprintf(tDuty,"%d", 500000);
	fflush(tDuty);
    fclose(tDuty);
    
    thruster = fopen("/sys/devices/ocp.3/pwm_test_P8_19.17/run", "w");
    fseek(thruster,0,SEEK_SET);
    fprintf(thruster,"%d",0);
    fflush(thruster);
    fclose(thruster);

    return;
}

void intHandler(int aSig) {
    printf("in the handler isrChar %c\n", isrChar);

    switch(isrChar) {
        case 'v': { vehicleTCB->myTask(vehicleTCB->taskDataPtr); break;
        }
        case 's': {         // stable
            break;
        }
        case 'e': {         //end of travel
                pthread_join(thread_id1, NULL);
                panelThreadCreated = false;
        }
            break;
        case 't': delete(&thrusterTCB); break;
    }
}

void stable(int aSig) {
    printf("the battery signal is stable\n");
    return;
}

void enableModeSel(void) {
    FILE *exp, *gpio67, *gpio68d, *gpio68v;

    exp = fopen("/sys/class/gpio/export", "w");
    fseek(exp, 0, SEEK_SET);
    fprintf(exp, "67");
    fflush(exp);
    fprintf(exp, "68");
    fflush(exp);
    fprintf(exp, "49");
    fflush(exp);
    fclose(exp);

    gpio67 = fopen("/sys/class/gpio/gpio67/direction", "w");
    fseek(gpio67, 0, SEEK_SET);
    fprintf(gpio67, "in");
    fflush(gpio67);
    fclose(gpio67);

    gpio68d = fopen("/sys/class/gpio/gpio68/direction", "w");
    fseek(gpio68d, 0, SEEK_SET);
    fprintf(gpio68d, "out");
    fflush(gpio68d);
    fclose(gpio68d);

    gpio68v = fopen("/sys/class/gpio/gpio68/value", "w");
    fseek(gpio68v, 0, SEEK_SET);
    fprintf(gpio68v, "0");
    fflush(gpio68d);
    fclose(gpio68d);
    return;
}
