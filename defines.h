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

// RELEASE Flag determines nothing, yet.
// DEBUG Flag determines if various debug features should be activated. (slow)
// CALIBRATION Flag (not implemented) determines if the program should try gather data for curve determination instead.
// TESTING Flag determines if program foregoes normal execution and instead determines how long some functions take to execute


//         Uses Debug variable to select function.
#define TESTING 0
#if TESTING == 0
#define RELEASE 0
#define DEBUG 1
#define CALIBRATION 0 
#else

// Testing Override, before anything gets execed
#define RELEASE 0
#define DEBUG 0
#define CALIBRATION 0
#endif

// Debug function implementations.
#if DEBUG
#define UseGPIO
#define SETDEBUG(n,k, v) CGPIO6(); if (n==k) {debug=v; SGPIO6();} 
#define DBCHANGE(var, v) var = v;
#else
#define SETDEBUG(n,k, v)
#define DBCHANGE(var, v)
#endif

#ifdef UseGPIO
#define SGPIO6() GpioDataRegs.GPASET.bit.GPIO6 = 1;
#define CGPIO6() GpioDataRegs.GPACLEAR.bit.GPIO6 = 1;
#define SGPIO7() GpioDataRegs.GPASET.bit.GPIO7 = 1;
#define CGPIO7() GpioDataRegs.GPACLEAR.bit.GPIO7 = 1;
#else
#define SGPIO6();
#define CGPIO6();
#define SGPIO7();
#define CGPIO7();
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
