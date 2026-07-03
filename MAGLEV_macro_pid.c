#include <ctype.h>
#include "defines.h"
#include "MAGLEV_macro_pid.h"

// const int16 LU_AccelToCurrent[4096] = {
//     #include "Curves/AccelToCurrent.dat"
// };

void initPID(PID* pid, double k, double kp, double ki, double kd) {
    pid->k = k;
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->kM = 0.1; // Effectively timescale for I coefficient.

    pid->prevData = 0;
    pid->prevI = 0;
}

extern PID position;
extern PID velocity;
extern PID acceleration;
extern PID current;

void initAllPIDs() {
    // Refer to PID_VALS.h to configure these.
    initPID(&position, POS_K, POS_P, POS_I, POS_D);
    initPID(&velocity, VEL_K, VEL_P, VEL_I, VEL_D);
    initPID(&acceleration, ACC_K, ACC_P, ACC_I, ACC_D);
    initPID(&current, CUR_K, CUR_P, CUR_I, CUR_D);
}

