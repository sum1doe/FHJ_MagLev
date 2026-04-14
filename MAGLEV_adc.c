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

void InitAdcRegs(void);
interrupt void  ISRadc(void);

int16   tempADC[14];

Uint16 Hall_A, Hall_B, Hall_C, Hall;

Uint16 duty = 0; // PWM signal for top switches //
extern Uint16  pwmPeriod;

extern int dir; // Direction of rotation;  CW - 1, CCW - 0//
Uint16 potValue;

void InitAdcRegs(void)
{
    // Configure ADC
    EALLOW;

    // Control
    AdcRegs.ADCCTL1.bit.TEMPCONV = 0;       // Internal temp sensor disabled. (ADCINA5 is normally working)
    AdcRegs.ADCCTL1.bit.VREFLOCONV = 0;     // Internal VrefLo sense disabled (ADCINB5)
    AdcRegs.ADCCTL1.bit.INTPULSEPOS = 1;    // ADCINT1 triggered after AdcResults latch
    AdcRegs.ADCCTL1.bit.ADCREFSEL = 0;      // Internal Vref
    AdcRegs.ADCCTL1.bit.ADCREFPWD = 1;      // Internal ref buffer circuit powered
    AdcRegs.ADCCTL1.bit.ADCBGPWD = 1;       // Bandgap buffer circuit powered
    AdcRegs.ADCCTL1.bit.ADCPWDN = 1;        // Analog circuitry powered up
    AdcRegs.ADCCTL1.bit.ADCENABLE = 1;      // Enable ADC
    AdcRegs.ADCCTL1.bit.RESET = 0;          // No reset

    AdcRegs.ADCCTL2.bit.CLKDIV2EN = 1;      // 0: ADCCLK = SYSCLK, 1: ADCCLK = SYSCLK/2 or /4
    AdcRegs.ADCCTL2.bit.ADCNONOVERLAP = 1;  // Enable non-overlap mode
    AdcRegs.ADCCTL2.bit.CLKDIV4EN = 0;      // 0: ADCCLK = SYSCLK or /2, 1: ADCCLK = SYSCLK or /4

    // Interrupt configuration
    AdcRegs.INTSEL1N2.bit.INT1E     = 1;    // Enabled ADCINT1
    AdcRegs.INTSEL1N2.bit.INT1CONT  = 0;    // Disable ADCINT1 Continuous mode
    AdcRegs.INTSEL1N2.bit.INT1SEL   = 2;    // setup EOC2 to trigger ADCINT1 to fire

    AdcRegs.SOCPRICTL.bit.ONESHOT = 0;      // 0: one shot mode disabled
    AdcRegs.SOCPRICTL.bit.SOCPRIORITY = 0x10;   // All SOCs are in high priority mode, arbitrated by SOC number

    AdcRegs.ADCINTSOCSEL1.all = 0;          // No ADCINT triggers SOC
    AdcRegs.ADCINTSOCSEL2.all = 0;          // No ADCINT triggers SOC

    AdcRegs.ADCSOCFRC1.all = 0;             // 0: No forced SOC
    AdcRegs.ADCSOCOVFCLR1.all = 1;          // 1: clear all SOCx overflow flags

    AdcRegs.ADCSOC0CTL.bit.CHSEL    = 0x0B; // set SOC0 channel select to ADCINB3
    AdcRegs.ADCSOC0CTL.bit.TRIGSEL  = 5;    // set SOC0 start trigger on EPWM1A
    AdcRegs.ADCSOC0CTL.bit.ACQPS    = 6;    // set SOC0 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)

    AdcRegs.ADCSOC1CTL.bit.CHSEL    = 0x03; // set SOC0 channel select to ADCINA3
    AdcRegs.ADCSOC1CTL.bit.TRIGSEL  = 5;    // set SOC0 start trigger on EPWM1A
    AdcRegs.ADCSOC1CTL.bit.ACQPS    = 6;    // set SOC0 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)

    AdcRegs.ADCSOC2CTL.bit.CHSEL    = 0x0D; // set SOC0 channel select to ADCINB5
    AdcRegs.ADCSOC2CTL.bit.TRIGSEL  = 5;    // set SOC0 start trigger on EPWM1A
    AdcRegs.ADCSOC2CTL.bit.ACQPS    = 6;    // set SOC0 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)

    AdcRegs.ADCSOC1CTL.bit.CHSEL    = 0x04; // set SOC0 channel select to ADCINA3
    AdcRegs.ADCSOC1CTL.bit.TRIGSEL  = 5;    // set SOC0 start trigger on EPWM1A
    AdcRegs.ADCSOC1CTL.bit.ACQPS    = 6;    // set SOC0 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)

    EDIS;
}

__attribute__((ramfunc))
interrupt void  ISRadc(void)
{
    // ADC read

    tempADC[0]  = (int16)(AdcResult.ADCRESULT0  & 0xFFF);// - uOffsetCh[0];
    tempADC[1]  = (int16)(AdcResult.ADCRESULT1  & 0xFFF);// - uOffsetCh[0];
    tempADC[2]  = (int16)(AdcResult.ADCRESULT2  & 0xFFF);// - uOffsetCh[0];

    // BLDC PWM
    potValue = tempADC[0];
    duty = (Uint16)((float)potValue/4095*4500);
    if(duty > pwmPeriod) duty = pwmPeriod;
    else if(duty < 10) duty = 0;

    EPwm1Regs.CMPA.half.CMPA = duty;
    EPwm1Regs.CMPB = 0;
    EPwm2Regs.CMPA.half.CMPA = 0;
    EPwm2Regs.CMPB = pwmPeriod;
    EPwm3Regs.CMPA.half.CMPA = 0;
    EPwm3Regs.CMPB = 0;

    AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;     // Clear ADCINT1 flag reinitialize for next SOC
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // Acknowledge interrupt to PIE

    return;
}
//===========================================================================
// No more.
//===========================================================================

