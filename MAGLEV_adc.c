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

#ifndef BufferSize
#define BufferSize 200
#define BufferVariation 64
#define BufferResolution 50
#endif
int16   hallBuffer[BufferSize];
int     hallIndex = 0;

const Uint16 LU_SensorDistance[4096] = {
    #include "SensorDistanceCurve.dat"
};

// Expected hall sensor error margin.
int     count[32];

int i;
int md, mn;

int loop; // Has the buffer been filled at least once?

Uint16 Hall_A, Hall_B, Hall_C, Hall;

Uint16 duty = 0; // PWM signal for top switches //
Uint16 prevDuty = 0;
extern Uint16  pwmPeriod;
extern Uint16  DutyLimit;

extern int dir; // Magnet Polarity, TODO check which is up/down//
Uint16 potValue;

// Import Section, effectively

typedef struct PIDStruct {
    // Input
    double sp;
    double data;
    // Config
    double kp;
    double ki;
    double kd;
    double kM; // Memory Coefficient
    // Output
    double cv;
    // Processing
    double prevData;
    double prevI;
} PID;

// PID* initPID(double kp, double ki, double kd);
void initAllPIDs();
// void updatePID(PID* pid, double data, double sp);
// double getCV(PID* pid);
// void delPID(PID* pid);
void stepPIDs(double magDistance, double setpoint, int sp_mode, double currentCurrent, double* pwmControl);
int sp = 0;
double currentcurrent = 0; // in mA
double prevCurrent = 0;
double m = 0.1;

double duty_cv = 0;

// And now, functions!

int mean(int* arr, int len) {
    int32 out = 0;
    for (i = 0; i < len; i++) {
        out += arr[i];
    }
    return out/len;
}


int median(int* arr, int len) {
    int max = 0;
    int min = 0x7fff;
    for (i = 0; i < len; i++) {
        if (arr[i] >= max) {
            max = arr[i];
        }
        if (arr[i] <= min) {
            min = arr[i];
        }
    }


    int cnt_len = max-min+1;
    if (cnt_len > BufferVariation) {
        cnt_len = BufferVariation;
    }

    for (i = 0; i < cnt_len; i++) {
        count[i] = 0;
    }
    

    for (i = 0; i < len; i++) {
        count[arr[i]-min]++;
    }

    int target = len/2;
    for (i = 0; i < cnt_len; i++) {
        target -= count[i];
        if (target <= 0) {
            return min + i;
        }
    }
    return min + BufferVariation;
}

int16 debug = 0;

int16 coilInterferenceFunc(int16 x, int dir) {
    if (x < 434 || dir == 1) {
        return (int16) (34.0*x/1906);
    }
    return (int16) 19-33.0*x/1472;
}

extern double pow(double a, double b);
double shunt2current(int16 input) {
    return 0.136 * pow((double) input, 1.5);
}

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

    // Pot3
    AdcRegs.ADCSOC0CTL.bit.CHSEL    = 0x0B; // set SOC0 channel select to ADCINB3
    AdcRegs.ADCSOC0CTL.bit.TRIGSEL  = 5;    // set SOC0 start trigger on EPWM1A
    AdcRegs.ADCSOC0CTL.bit.ACQPS    = 10;    // set SOC0 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
    // ACQPS is the time taken to process input signals, specifically for applications where the thing you're reading is slow.
    // If you need to read a high frequency signal, use an OpAmp with a sufficient bandwidth to charge the ADC faster.

    // Pot2
    AdcRegs.ADCSOC1CTL.bit.CHSEL    = 0x03; // set SOC0 channel select to ADCINA3
    AdcRegs.ADCSOC1CTL.bit.TRIGSEL  = 5;    // set SOC0 start trigger on EPWM1A
    AdcRegs.ADCSOC1CTL.bit.ACQPS    = 10;    // set SOC0 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)

    // Pot1
    AdcRegs.ADCSOC2CTL.bit.CHSEL    = 0x0D; // set SOC0 channel select to ADCINB5
    AdcRegs.ADCSOC2CTL.bit.TRIGSEL  = 5;    // set SOC0 start trigger on EPWM1A
    AdcRegs.ADCSOC2CTL.bit.ACQPS    = 10;    // set SOC0 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)

    // Hall
    AdcRegs.ADCSOC3CTL.bit.CHSEL    = 0x04; // set SOC0 channel select to ADCINA4
    AdcRegs.ADCSOC3CTL.bit.TRIGSEL  = 5;    // set SOC0 start trigger on EPWM1A
    AdcRegs.ADCSOC3CTL.bit.ACQPS    = 10;    // set SOC0 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)

    // Current A
    AdcRegs.ADCSOC4CTL.bit.CHSEL    = 0x00; // Ch = ADCINA0
    AdcRegs.ADCSOC4CTL.bit.TRIGSEL  = 5;    // Same trigger
    AdcRegs.ADCSOC4CTL.bit.ACQPS    = 10;    // set SOC0 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)


    // Current B
    AdcRegs.ADCSOC5CTL.bit.CHSEL    = 0x08; // Ch = ADCINB0
    AdcRegs.ADCSOC5CTL.bit.TRIGSEL  = 5;    // Same trigger
    AdcRegs.ADCSOC5CTL.bit.ACQPS    = 10;    // set SOC0 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)


    // Current C
    AdcRegs.ADCSOC6CTL.bit.CHSEL    = 0x09; // Ch = ADCINB1
    AdcRegs.ADCSOC6CTL.bit.TRIGSEL  = 5;    // Same trigger
    AdcRegs.ADCSOC6CTL.bit.ACQPS    = 10;    // set SOC0 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)

    // Current C
    AdcRegs.ADCSOC7CTL.bit.CHSEL    = 0x07; // Ch = ADCINA7
    AdcRegs.ADCSOC7CTL.bit.TRIGSEL  = 5;    // Same trigger
    AdcRegs.ADCSOC7CTL.bit.ACQPS    = 10;    // set SOC0 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)


    for (i = 0; i < BufferSize; i++) {
        hallBuffer[i] = 0;
    }

    loop = 0;

    initAllPIDs();

    EDIS;
}


int16 dist;
Uint16 sensor_data;

__attribute__((ramfunc))
interrupt void  ISRadc(void)
{
    // entered every 0.1ms
    // ADC read

    tempADC[0]  = (int16)(AdcResult.ADCRESULT0  & 0xFFF);// - uOffsetCh[0]; // Pot1
    tempADC[1]  = (int16)(AdcResult.ADCRESULT1  & 0xFFF);// - uOffsetCh[0]; // Pot2
    tempADC[2]  = (int16)(AdcResult.ADCRESULT2  & 0xFFF);// - uOffsetCh[0]; // Pot3
    tempADC[3]  = (int16)(AdcResult.ADCRESULT3  & 0xFFF);// - uOffsetCh[0]; // Hall
    tempADC[4]  = (int16)(AdcResult.ADCRESULT4  & 0xFFF);// - uOffsetCh[0]; // IA
    tempADC[5]  = (int16)(AdcResult.ADCRESULT5  & 0xFFF);// - uOffsetCh[0]; // IB
    tempADC[6]  = (int16)(AdcResult.ADCRESULT6  & 0xFFF);// - uOffsetCh[0]; // IC
    tempADC[7]  = (int16)(AdcResult.ADCRESULT7  & 0xFFF);// - uOffsetCh[0]; // V Something




    // hallIndex is only 0 here after a full buffer has been written.
    loop = loop || (hallIndex == 0);

    int useful_len = loop ? BufferSize : hallIndex;

    // mn = mean(hallBuffer, useful_len);
    // md = median(hallBuffer, useful_len);

    // BLDC PWM
    sp = tempADC[0];

    sensor_data = tempADC[3]; // Reading hall sensor.
    sensor_data -= coilInterferenceFunc(prevDuty, dir); // Apply correction for interference from Coil.

    dist = LU_SensorDistance[sensor_data];
    // dir = dir && 2414 > tempADC[3] || 2410 > tempADC[3];

    dir = tempADC[1] > 2047;

    currentcurrent = 2100-((int16)tempADC[5-dir]);
    if (currentcurrent < 0) currentcurrent = 0;

    currentcurrent = 0.1 * currentcurrent + 0.9 * prevCurrent;

    prevCurrent = currentcurrent;
    // debug = currentcurrent;
    currentcurrent = (int16) shunt2current(currentcurrent);


    if (debug <= 0 ) {
        sp = 0;
        debug = 0;
    }
    else {
        debug--;
        if (debug % BufferResolution == 0) {
            hallBuffer[hallIndex] = currentcurrent;
            hallIndex = (hallIndex+1)%BufferSize;
        }
    }

    // SPModes:
    // 0: position
    // 1: velocity
    // 2: acceleration
    // 3: current
    stepPIDs((double) dist,
             (double)sp*2500.0/4095, // Revert 0 to sp
             3,
             (double) currentcurrent,
             &duty_cv);

    duty = (int16) duty_cv;

    // if (debug != 0) {
    //     duty = debug;
    //     debug = 0;
    // }
    
    
    if (duty > DutyLimit) {
        duty = DutyLimit;
    }
    
    if (duty < 0) {
        duty = 0;
    }

    duty_cv = (double) duty;


    if (!dir) {
        EPwm1Regs.CMPA.half.CMPA = duty;
        EPwm1Regs.CMPB = 0;
        EPwm2Regs.CMPA.half.CMPA = 0;
        EPwm2Regs.CMPB = pwmPeriod;  
    } else {
        EPwm2Regs.CMPA.half.CMPA = duty;
        EPwm2Regs.CMPB = 0;
        EPwm1Regs.CMPA.half.CMPA = 0;
        EPwm1Regs.CMPB = pwmPeriod;  
    }

    prevDuty = duty;
                                                                                       
    EPwm3Regs.CMPA.half.CMPA = 0;
    EPwm3Regs.CMPB = 0;

    AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;     // Clear ADCINT1 flag reinitialize for next SOC
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // Acknowledge interrupt to PIE
    return;
}
//===========================================================================
// No more.
//===========================================================================

