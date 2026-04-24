///////////////////////////////////
//
//    BLDC Driver for FHJ
//    (c) Park 2026
//
///////////////////////////////////

#include <ctype.h>
#include <string.h>
#include "F2806x_Device.h"     // DSP2833x Headerfile Include File
#include "F2806x_Examples.h"   // DSP2833x Examples Include File
#include "defines.h"

void InitInterrupts(void);                  // Function prototype
void initTimer(void);
interrupt void timerISR(void);

extern void stepPIDs(double magDistance, double setpoint, int sp_mode, double currentCurrent, double* pwmControl);

extern interrupt void  ISRadc(void);
extern Uint16 tempADC[];

extern Uint16 duty;

Uint32  eQepPosCntRead, eQepPosCntReadRaw;
char    eQepDirection;
float   fRpm = 0, fRpmRaw = 0, fPosEoffset = 0;
int32   diffPos, diffPosRaw;
int32   pPosVal;

Uint16 dir = 1;                               // Direction of rotation;
// CCW - 1 (-), CW - 0 (+)

void InitInterrupts(void)
{
    // This is needed to write to EALLOW protected register
    EALLOW;

    PieVectTable.TINT0 = &timerISR;             // Set Timer0 interrupt service routine
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;          // Enable Timer0 interrupt in PIE Group 1
    IER |= M_INT1;                              // Enable CPU INT1

    PieVectTable.ADCINT1 = &ISRadc;
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;          // Enable PIE Group 1 INT1:ADC
    IER |= M_INT1;                              // Enable CPU INT1

    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;          // Enable the PIE
    PieCtrlRegs.PIEACK.all = 0xFFFF;            // Enables PIE to drive a pulse into the CPU

    // This is needed to disable write to EALLOW protected registers
    EDIS;
}

extern Uint16   duty;
extern double   currentcurrent;
Uint16 DutyLimit = EPWM_TIMER_TBPRD;

extern int16 hallBuffer[50];
extern int16 debug;

extern int16 dist;
extern int sp;
extern double currentcurrent;
extern double duty_cv;

// Timer0 interrupt service routine
__attribute__((ramfunc))
interrupt void timerISR(void)
{
    EINT;
    CpuTimer0.InterruptCount++;
    // entered every 1ms

    if ((tempADC[7] < MinVoltage || currentcurrent > MaxCurrent) && DutyLimit > 0) {
        // debug = currentcurrent;
        DutyLimit--;
    }
    else if (tempADC[7] > RecoverVoltage && currentcurrent < MaxCurrent && DutyLimit < EPWM_TIMER_TBPRD) {
        DutyLimit+=1;
    }

    if(!(CpuTimer0.InterruptCount % 1)){ 
        stepPIDs((double) dist,
            (((double)sp/4095.0)*300.0+100.0), // Revert 0 to sp
            0,
            (double) currentcurrent,
            &duty_cv);
    }

    // Clear interrupt flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    DINT;
}

void initTimer0(void)
{
    Uint32 PeriodInClocks;

    // Configure and start Timer0
    EALLOW;
    CpuTimer0.CPUFreqInMHz = 80;    // 80 Mhz Clock Frequency
    CpuTimer0.PeriodInUSec = 1000;  // interrupt every 1 milliseconds
    PeriodInClocks = (Uint32) (CpuTimer0.CPUFreqInMHz * CpuTimer0.PeriodInUSec);

    CpuTimer0Regs.PRD.all = PeriodInClocks-1;    // Set period to 400 clocks (200us at 200 MHz)
    CpuTimer0Regs.TPR.all = 0;
    CpuTimer0Regs.TPRH.all = 0;

    CpuTimer0Regs.TCR.bit.TRB = 1;      // Reload Timer0 counter
    CpuTimer0Regs.TCR.bit.SOFT = 0;
    CpuTimer0Regs.TCR.bit.FREE = 0;
    CpuTimer0Regs.TCR.bit.TSS = 0;      // Start Timer0
    CpuTimer0Regs.TCR.bit.TIE = 1; // Enable the Timer Interrupt
    EDIS;
}

//===========================================================================
// No more.
//===========================================================================



