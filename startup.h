#ifndef STARTUP_H
#define STARTUP_H

void startup(void);
void defineTCBs(void);
void initData(void);
void handleSig(void);
void restOfStartup(void);
void enableADC(void);
void enablePWM(void);
void startup();
void endOfTravel(int);
void stable(int);
void enableModeSel(void);
void deleteThruster(int);

#endif