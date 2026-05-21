///////////////////////////////////
//
//    BLDC Driver for FHJ
//    (c) Park 2026
//
///////////////////////////////////

#include <stdio.h>
#include <stdint.h>
#include "F2806x_Device.h"
#include "F2806x_Examples.h"
#include "F2806x_SysCtrl.h"
#include "F2806x_Gpio.h"
#include "F2806x_GlobalPrototypes.h"

#include "defines.h"

#if TESTING
#include "MAGLEV_testing.h"
#endif

// Global function declarations
extern void InitGPIO(void);
extern void InitAdcRegs(void);
extern void InitInterrupts(void);
extern void sciaPutChar(char ch);
extern void sciaPutString(char *str);
extern void processCommand();
extern void InitEPwm(void);
extern void initTimer0(void);

extern char     cmdEnteredFlag;
extern Uint16   stepRunStop, stepNo, modeNum;
extern Uint32   Cnt_total, Cnt_on;

// From Ex 4-1
Uint32 loopCnt = 0, loopToggle = 300000;

int test(int input) {
    return input;
}

#if TESTING == 1
int16 debug = 0;
int dbchan = 0;
#endif


void main(void) {
#ifdef _RELEASE
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (Uint32)&RamfuncsLoadSize);
#endif

    // Disable interrupts during initialization

    DINT;

    // TI Standard initialization routines to default values
    InitSysCtrl();
    InitGpio();
    InitPieCtrl();
    InitPieVectTable();

    IER = 0x0000;                       // Interrupt Enable Register initialization
    IFR = 0x0000;                       // Interrupt Flag Register initialization

    InitGPIO();                         // GPIO Initialization
    InitAdcRegs();
    InitEPwm();
    initTimer0();                       // Timer0 initialization
    InitInterrupts();                   // Interrupt Initialization

    EALLOW;
    EINT;                               // Enable Global interrupt INTM
    ERTM;                               // Enable realtime emulation mode
    EDIS;

    // LEDs initially off
    GpioDataRegs.GPBSET.bit.GPIO34 = 1;

    // Gate enable
    GpioDataRegs.GPBSET.bit.GPIO50 = 1;

    #if TESTING == 0
    // Main loop
    while(1)
    {
        if(!(loopCnt++ % loopToggle)){      // LED TOGGLE at every 200k loops
             GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
        }
    }

    #else

    #endif
}


