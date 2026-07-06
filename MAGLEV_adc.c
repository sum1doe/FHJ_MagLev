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
// #include "MAGLEV_pid.h"
#include "MAGLEV_macro_pid.h"
// #include "MAGLEV_curves.h"

int16   tempADC[14];

// To change size of generated 
const int16 LU_SensorDistance[4096] = {
    #include "Curves/SensorDistanceCurve.dat"
};

// TODO Recheck size.
const Uint16 LU_CoilInterference[1024] = {
    #include "Curves/CoilInterference.dat"
};

const float LU_ShuntToCurrent[1024] = {
    #include "Curves/ShuntToCurrent.dat"
};

const int16 LU_AccelToCurrent[4096] = {
    #include "Curves/AccelToCurrent.dat"
};

#if DEBUG

// Flag debug as made so that CALIBRATION doesn't mind.
#define debug_deffed

int16 debug = 0;
int dbchan = 0;

#ifndef BufferSize
#define BufferSize 200
#define BufferVariation 64
#endif

int BufferResolution = 50;
int16   hallBuffer[BufferSize];
int     hallIndex = 0;
#endif


int i;
int md, mn;

int loop; // Has the buffer been filled at least once?

Uint16 Hall_A, Hall_B, Hall_C, Hall;

int16 duty = 0; // PWM signal for top switches //
int16 prevDuty = 0;
extern Uint16  pwmPeriod;
extern Uint16  DutyLimit;

extern int dir; // Magnet Polarity, TODO check which is up/down//
Uint16 potValue;

int sp = 0;
int prevSP = 0;

double currentcurrent = 0; // in mA
double prevCurrent = 0;
double m = 0.1;

double duty_cv = 0;

int16 dist;
int16 prevdist;

#define distanceFilter 2 // of 100

PID position = {};
PID velocity = {};
PID acceleration = {};
PID current = {};

double velSP;
double curSP;

Uint16 sensor_data;

// And now, functions declarations

void InitAdcRegs(void);
#if !CALIBRATION
interrupt void  ISRadc(void);
#else
interrupt void  ISRadccalibration(void);
#endif

// Calibration Mode conditional defines

#if CALIBRATION
extern PID current;

double data[200] = {};
int16 count[200] = {};

#ifndef debug_deffed
int16 debug = 0;
#endif
#endif


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

    #if DEBUG
    for (i = 0; i < BufferSize; i++) {
        hallBuffer[i] = 0;
    }
    #endif

    loop = 0;

#if CALIBRATION
    initPID(&current, CUR_K, CUR_P, CUR_I, CUR_D);
    i = 0;
#else
    initAllPIDs();
#endif

    EDIS;
}


#if !CALIBRATION
__attribute__((ramfunc))
#endif
interrupt void ISRadc(void)
{
    // SGPIO6(); // Macro includes checking for DEBUG mode.
    SGPIO7();
    // entered every 0.1ms
    // ADC read
	
	SETDEBUG(dbchan, 0, 1);

    tempADC[0]  = (int16)(AdcResult.ADCRESULT0  & 0xFFF);// - uOffsetCh[0]; // Pot1
    tempADC[1]  = (int16)(AdcResult.ADCRESULT1  & 0xFFF);// - uOffsetCh[0]; // Pot2
    tempADC[2]  = (int16)(AdcResult.ADCRESULT2  & 0xFFF);// - uOffsetCh[0]; // Pot3
    tempADC[3]  = (int16)(AdcResult.ADCRESULT3  & 0xFFF);// - uOffsetCh[0]; // Hall
    tempADC[4]  = (int16)(AdcResult.ADCRESULT4  & 0xFFF);// - uOffsetCh[0]; // IA
    tempADC[5]  = (int16)(AdcResult.ADCRESULT5  & 0xFFF);// - uOffsetCh[0]; // IB
    tempADC[6]  = (int16)(AdcResult.ADCRESULT6  & 0xFFF);// - uOffsetCh[0]; // IC
    tempADC[7]  = (int16)(AdcResult.ADCRESULT7  & 0xFFF);// - uOffsetCh[0]; // V Something


	SETDEBUG(dbchan, 1, 1);

    // hallIndex is only 0 here after a full buffer has been written.
    // loop = loop || (hallIndex == 0);
    // int useful_len = loop ? BufferSize : hallIndex;

    // mn = mean(hallBuffer, useful_len);
    // md = median(hallBuffer, useful_len);

    // BLDC PWM
    sp = tempADC[0];
    sp = sp * 0.02 + 0.98 * prevSP;
    prevSP = sp;

	SETDEBUG(dbchan, 2, sp);

    sensor_data = tempADC[3]; // Reading hall sensor.
    // debug = coilInterferenceFunc(prevCurrent, dir);

    // Old:
    // sensor_data += coilInterferenceFunc(prevCurrent, dir); // Apply correction for interference from Coil.
    sensor_data += LU_CoilInterference[(int16)prevCurrent];

	SETDEBUG(dbchan, 3, sensor_data);

    dist = LU_SensorDistance[sensor_data]; 
	SETDEBUG(dbchan, 4, dist);
    dir = dir && 2410 > tempADC[3] || 2414 > tempADC[3];
	SETDEBUG(dbchan, 5, dir);

    // dir = tempADC[1] > 2047;

    currentcurrent = 2100-((int16)tempADC[5-dir]);
    if (currentcurrent < 0) currentcurrent = 0;
    // debug = currentcurrent;

    // SETDEBUG(dbchan, 50, 1)
    // old
    // currentcurrent = shunt2current(currentcurrent);

    currentcurrent = LU_ShuntToCurrent[(int) currentcurrent];
    // SETDEBUG(dbchan, 51, 1)

    currentcurrent = 0.05 * currentcurrent + 0.95 * prevCurrent;
    
    prevCurrent = currentcurrent;
    
	SETDEBUG(dbchan, 6, currentcurrent);
	


    // PID Tuning Feedback
	
	#if DEBUG
		if (dbchan == 9) {
			hallIndex = 0;
		}
		else if (dbchan == 10) {
			BufferResolution = debug;
		}

		if (dbchan == 7) {
			if (debug++ % BufferResolution == 0) {
				hallBuffer[hallIndex] = sensor_data;
				hallIndex = (hallIndex+1)%BufferSize;
			}
		}

		if (dbchan == 8) {
			if (debug++ % BufferResolution == 0) {
				hallBuffer[hallIndex] = currentcurrent;
				hallIndex = (hallIndex+1)%BufferSize;
			}
		}

	#endif
    
	SETDEBUG(dbchan, 60, 1);

    // SPModes:
    // 0: position
    // 1: velocity
    // 2: acceleration
    // 3: current


    // stepPIDs((double) dist,
    //          (((double)sp/4095.0)*300.0+100.0), // Revert 0 to sp
    //          0,
    //          (double) currentcurrent,
    //          &duty_cv);

    // Local stepPID:
    // Distance filter
    // updatePID on position
    // get velSP from position
    // if in range, scale by LU_AccelToCurrent
    // pass velSP to current PID
    // get cv from current
    // clamp 0 < cv < 2500
    // set duty_cv = cv

    dist = (prevdist*(100-distanceFilter)+dist*distanceFilter)/100;
    prevdist = dist;
    
	// SETDEBUG(dbchan, 62, 1); // takes 487ns asof PID rewrite

    updatePID(position, dist, sp);
    velSP = getCV(position);

    if (velSP > 500) {
        velSP *= LU_AccelToCurrent[Mag2SensorOffset - dist];
    }
	// SETDEBUG(dbchan, 65, 1); // 122ns asof PID 

    updatePID(current, currentcurrent, velSP);
    curSP = getCV(current);

    if (0 < curSP) {
        curSP = 0;
    }
    if (curSP < 2500) {
        curSP = 2500;
    }
    duty_cv = curSP;


	SETDEBUG(dbchan, 69, 1); // 288ns asof PID, to 70

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

	// SETDEBUG(dbchan, 70, 1); // 277ns to 71 asof PID

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
    
	// SETDEBUG(dbchan, 71, 1); // 178ns to end asof PID

    prevDuty = duty;
                                                                                       
    EPwm3Regs.CMPA.half.CMPA = 0;
    EPwm3Regs.CMPB = 0;

    AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;     // Clear ADCINT1 flag reinitialize for next SOC
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // Acknowledge interrupt to PIE

	CGPIO6();
    CGPIO7();

    return;
}


// Testing done with 9955 ohm resistor.
#if CALIBRATION
__attribute__((ramfunc))
interrupt void ISRadccalibration(void)
{   
    SGPIO6();
    tempADC[0]  = (int16)(AdcResult.ADCRESULT0  & 0xFFF);// - uOffsetCh[0]; // Pot1
    tempADC[1]  = (int16)(AdcResult.ADCRESULT1  & 0xFFF);// - uOffsetCh[0]; // Pot2
    tempADC[2]  = (int16)(AdcResult.ADCRESULT2  & 0xFFF);// - uOffsetCh[0]; // Pot3
    tempADC[3]  = (int16)(AdcResult.ADCRESULT3  & 0xFFF);// - uOffsetCh[0]; // Hall
    tempADC[4]  = (int16)(AdcResult.ADCRESULT4  & 0xFFF);// - uOffsetCh[0]; // IA
    tempADC[5]  = (int16)(AdcResult.ADCRESULT5  & 0xFFF);// - uOffsetCh[0]; // IB
    tempADC[6]  = (int16)(AdcResult.ADCRESULT6  & 0xFFF);// - uOffsetCh[0]; // IC
    tempADC[7]  = (int16)(AdcResult.ADCRESULT7  & 0xFFF);// - uOffsetCh[0]; // V Something

    // Change SP
    i++;
    if (!(i % 10000)) {
        i=0;
        sp += 10;

        if (sp > 10) {
            sp = 0;
        }
    }

    sp = debug;
    // Do current PID
    updatePID(&current, (double) tempADC[6], sp);
    duty = getCV(&current);

    if (i < 100) {
        duty = 0;
    }

    // Adjust Duty
    EPwm1Regs.CMPA.half.CMPA = duty;
    EPwm1Regs.CMPB = 0;
    EPwm2Regs.CMPA.half.CMPA = 0;
    EPwm2Regs.CMPB = pwmPeriod;                                        
    EPwm3Regs.CMPA.half.CMPA = 0;
    EPwm3Regs.CMPB = 0;

    AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;     // Clear ADCINT1 flag reinitialize for next SOC
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // Acknowledge interrupt to PIE

    CGPIO6();
    return;
}
#endif


//===========================================================================
// No more.
//===========================================================================

