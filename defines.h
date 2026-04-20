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


#define EPWM_TIMER_TBPRD  4500                  // PWM Period register 10kHz

////////////////
//
//   PID Values 
//
////////////////

#define POS_P 1
#define POS_I 0
#define POS_D 0

#define VEL_P 1
#define VEL_I 0
#define VEL_D 0

#define ACC_P 1
#define ACC_I 0
#define ACC_D 0

#define CUR_P 10
#define CUR_I 50
#define CUR_D 0

//========================//


#define BS              0x08

#define STEP_FULL       1
#define STEP_HALF       2
#define STEP_MICRO8     3
#define STEP_MICRO16    4



#endif /* DEFINES_H_ */
