#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include "tcb.h"

#define DEBUG
#define DRIVE_MOTOR_SPEED_INC *(*(Data*)keypad).driveMotorSpeedIncPtr
#define DRIVE_MOTOR_SPEED_DEC *(*(Data*)keypad).driveMotorSpeedDecPtr

bool solarPanelDeploy;
bool solarPanelRetract;
int isrChar;

/******************************************************************************
* function name: consoleKeypad
* function inputs: pointer to void
* function outputs: void
* function description:interrogated for new key presses as needed.
 The task is only scheduled during deployment or retraction of the solar panels.
* author: Yaying Huang
******************************************************************************/
void* consoleKeypad(void* keypad) {
    while (solarPanelDeploy || solarPanelRetract) {
        char input;
        char temp[1];
        #ifdef DEBUG
        //printf("now consoleKeypad should be waiting for a input\n");
        #endif
        usleep(1);
        input = getch();
        temp[0] = input;
        #ifdef DEBUG
        //printf("consoleKeypad got the input %c\n", input);
        #endif
        usleep(1);
        switch(input) {
            case '=': DRIVE_MOTOR_SPEED_INC = true; break;
            case '-': DRIVE_MOTOR_SPEED_DEC = true; break;
            default:{
                write(fd0, temp, 1);
                isrChar = 'v';
                raise(SIGINT);
                break;
            }
        }         
    }
    return NULL;
}

int getch(void) {
      int c=0;

      struct termios org_opts, new_opts;
      int res=0;
          //-----  store old settings -----------
      res=tcgetattr(STDIN_FILENO, &org_opts);
      assert(res==0);
          //---- set new terminal parms --------
      memcpy(&new_opts, &org_opts, sizeof(new_opts));
      new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
      tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
      c=getchar();
          //------  restore old settings ---------
      res=tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
      assert(res==0);
      return(c);
}
