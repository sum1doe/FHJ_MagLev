///////////////////////////////////
//
//    BLDC Driver for FHJ
//    (c) Park 2026
//
///////////////////////////////////

#ifndef DEFINES_H_
#define DEFINES_H_


// Units in whatever the BoostXL board reads.
#define MinVoltage 850
#define RecoverVoltage 900

#define MaxCurrent 2300

#define EPWM_TIMER_TBPRD  4500                  // PWM Period register 10kHz

#define Mag2SensorOffset 698

#define RELEASE 1
#define DEBUG 1
#define CALIBRATION 0
#define TESTING 0

// Testing Override, before anything gets execed
#if TESTING == 1
#define RELEASE 0
#define DEBUG 0
#define CALIBRATION 0
#endif

// Debug function implementations.
#if DEBUG
#define UseGPIO
#define SETDEBUG(n,k, v) if (n==k) debug =v; SGPIO(); 
#define DBCHANGE(var, v) var = v;
#else
#define SETDEBUG(n,k, v)
#define DBCHANGE(var, v)
#endif

#ifdef UseGPIO
#define SGPIO() GpioDataRegs.GPASET.bit.GPIO6 = 1;
#define CGPIO() GpioDataRegs.GPACLEAR.bit.GPIO6 = 1;
#else
#define SGPIO();
#define CGPIO();
#endif

////////////////
//
//   PID Values 
//
////////////////

#define POS_K 0.0005
#define POS_P 3.5
#define POS_I 0
#define POS_D 0

#define VEL_K 1
#define VEL_P 1
#define VEL_I 0
#define VEL_D 0

#define ACC_K 1
#define ACC_P 1
#define ACC_I 0
#define ACC_D 0

#define CUR_K 1
#define CUR_P 25
#define CUR_I 0.005
#define CUR_D 100

//========================//


#define BS              0x08

#endif /* DEFINES_H_ */
