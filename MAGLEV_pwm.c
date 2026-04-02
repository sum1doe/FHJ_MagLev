///////////////////////////////////
//
//    BLDC Driver for FHJ
//    (c) Park 2026
//
///////////////////////////////////

#include <math.h>

#include "F2806x_Device.h"     // DSP2833x Headerfile Include File
#include "F2806x_Examples.h"   // DSP2833x Examples Include File
#include "F2806x_EPwm.h"

void InitEPwm(void);

#define EPWM_TIMER_TBPRD  4500                  // PWM Period register 10kHz

// PWM related variables
Uint16  pwmPeriod = 4500, pwmPeriod1, pwmPeriod2, pwmPeriod3, pwmPeriod4, pwmPeriodHalf;
Uint16  pwmCmd1 = 2250, pwmCmd2 = 2250, pwmCmd3 = 2250, pwmCmd4 = 2250;

void InitEPwm(void)
{
    // PWM1
    // PWM Period calculation
    // SYSCLKFREQ/SW_FREQ/2, 90MHz/10kHz/2 = 4500
    pwmPeriod1 = pwmPeriod;
    pwmPeriodHalf = pwmPeriod/2;
    pwmCmd1 = pwmPeriod1 >> 1;      // 50% duty

    // Setup TBCLK
    EPwm1Regs.TBPRD = pwmPeriod1;                   // Set timer period 801 TBCLKs
    EPwm1Regs.TBCTR = 0x0000;                       // Clear counter

    // Set Compare values
    EPwm1Regs.CMPA.half.CMPA = pwmPeriod1 >> 1;     // Set compare A value
    EPwm1Regs.CMPB = pwmPeriod1 >> 1;               // Set Compare B value

    // Setup counter mode
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;  // Count up
    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE; // Phase loading disabled
    EPwm1Regs.TBCTL.bit.PRDLD = TB_SHADOW;
    EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1; // TBCLK = SYSCLKOUT
    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // load on CTR = Zero
    EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; // load on CTR = Zero// Set actions

    EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;              // Set PWM1A on event A, up count
    EPwm1Regs.AQCTLA.bit.CAD = AQ_SET;            // Clear PWM1A on event A, down count
    EPwm1Regs.AQCTLB.bit.CBU = AQ_CLEAR;              // Set PWM1B on event A, up count
    EPwm1Regs.AQCTLB.bit.CBD = AQ_SET;            // Clear PWM1B on event A, down count

    // Interrupt where we will change the Compare Values
    EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;       // Select INT on Zero event
    EPwm1Regs.ETSEL.bit.INTEN = 0;                  // Disable INT
    EPwm1Regs.ETPS.bit.INTPRD = ET_1ST;             // Generate INT on 1st event

    EPwm1Regs.ETSEL.bit.SOCAEN = 1;                 // Enable SOC on A group
    EPwm1Regs.ETSEL.bit.SOCASEL = 1;                // Select SOC from CPMA on zero
    EPwm1Regs.ETPS.bit.SOCAPRD = 1;                 // Generate pulse on 1st event

    // PWM2
    // PWM Period calculation
    // SYSCLKFREQ/SW_FREQ/2, 90MHz/10kHz/2 = 4500
    pwmPeriod2 = pwmPeriod;
    pwmPeriodHalf = pwmPeriod/2;
    pwmCmd2 = pwmPeriod2 >> 1;      // 50% duty

    // Setup TBCLK
    EPwm2Regs.TBPRD = pwmPeriod2;                   // Set timer period 801 TBCLKs
    EPwm2Regs.TBCTR = 0x0000;                       // Clear counter

    // Set Compare values
    EPwm2Regs.CMPA.half.CMPA = pwmPeriod2 >> 1;     // Set compare A value
    EPwm2Regs.CMPB = pwmPeriod2 >> 1;               // Set Compare B value

    // Setup counter mode
    EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;  // Count up
    EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE; // Phase loading disabled
    EPwm2Regs.TBCTL.bit.PRDLD = TB_SHADOW;
    EPwm2Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1; // TBCLK = SYSCLKOUT
    EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // load on CTR = Zero
    EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO; // load on CTR = Zero// Set actions

    EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR;              // Set PWM2A on event A, up count
    EPwm2Regs.AQCTLA.bit.CAD = AQ_SET;            // Clear PWM2A on event A, down count
    EPwm2Regs.AQCTLB.bit.CBU = AQ_CLEAR;              // Set PWM2B on event A, up count
    EPwm2Regs.AQCTLB.bit.CBD = AQ_SET;            // Clear PWM2BA on event A, down c

    // Interrupt where we will change the Compare Values
    EPwm2Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;       // Select INT on Zero event
    EPwm2Regs.ETSEL.bit.INTEN = 0;                  // Disable INT
    EPwm2Regs.ETPS.bit.INTPRD = ET_1ST;             // Generate INT on 1st event
 }




