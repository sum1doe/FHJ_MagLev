///////////////////////////////////
//
//    MagLev Project for FHJ
//    (c) de Beer 2026
//
///////////////////////////////////

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "F2806x_Device.h"     // DSP2833x Headerfile Include File
#include "F2806x_Examples.h"   // DSP2833x Examples Include File
#include "defines.h"
#include "MAGLEV_pid.h"
#include "MAGLEV_curves.h"

#ifndef DataBuffer
#define DataBuffer 5
#endif

double dataBuf[DataBuffer];
int dataIndex = -1;

double cv = 0;

int16   tempADC[14];
int16 LU_AccelToCurrent[4096] = {
    #include "Curves/AccelToCurrent.dat"
};

void initPID(PID* pid, double k, double kp, double ki, double kd) {
    pid->k = k;
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->kM = 0.1; // Effectively timescale for I coefficient.

    pid->prevData = 0;
    pid->prevI = 0;
}

__attribute__((ramfunc))
void updatePID(PID* pid, double data, double sp) {
    pid->prevData = pid->data;
    pid->data = data;
    pid->sp = sp;
}

__attribute__((ramfunc))
double getCV(PID* pid) {
    pid->prevI = pid->kM * (pid->sp - pid->data) + (1-pid->kM) * pid->prevI;
    pid->cv =   pid->k * (pid->kp * (pid->sp - pid->data) +
                pid->ki * (pid->prevI) * 10 - 
                pid->kd * (pid->data-pid->prevData));

    return pid->cv;
}

PID position = {};
PID velocity = {};
PID acceleration = {};
PID current = {};

void initAllPIDs() {
    // Refer to PID_VALS.h to configure these.
    initPID(&position, POS_K, POS_P, POS_I, POS_D);
    initPID(&velocity, VEL_K, VEL_P, VEL_I, VEL_D);
    initPID(&acceleration, ACC_K, ACC_P, ACC_I, ACC_D);
    initPID(&current, CUR_K, CUR_P, CUR_I, CUR_D);
}

#if DEBUG
extern int16 debug;
extern int dbchan;

#ifndef BufferSize
#define BufferSize 200
#define BufferVariation 64
#endif

extern int BufferResolution;
extern int16   hallBuffer;
extern int     hallIndex;
#endif

extern int16 debug;
__attribute__((ramfunc))
void stepPIDs(double magDistance, double setpoint, int sp_mode, double currentCurrent, double* pwmControl) {
    // Data Buffer Updates
	SETDEBUG(dbchan, 61, 1);
    
    dataIndex++;
    dataIndex %= dataIndex;
    dataBuf[dataIndex] = magDistance;

    // Pos, Vel, Acc calculations.
    int prevprevdist = dataBuf[(dataIndex+5-2)%DataBuffer];
    int prevdist = dataBuf[(dataIndex+5-1)%DataBuffer];
    int dist = magDistance;

    int prevvel = prevprevdist - prevdist;
    int vel = prevdist-dist;

    int acc = prevvel - vel;

	SETDEBUG(dbchan, 62, 1);

    // PID Control
    // Setpoint doesn't matter if getCV isn't called.
    updatePID(&position, magDistance, setpoint);
    double vel_sp = 0;
    if (sp_mode < 1) {                              //  Position Loop
        vel_sp = getCV(&position);
        // if (vel_sp > 0.2) {
        //     vel_sp = 0.2;
        // }
        SETDEBUG(dbchan, 63, 1);
        if (dist < 500) {
            // Mag2SensorOffset in defines.h

            // Old:
            // vel_sp *= acc2curr(Mag2SensorOffset - dist);

            vel_sp *= LU_AccelToCurrent[Mag2SensorOffset - dist];
        }
        SETDEBUG(dbchan, 64, 1);

    } else {
        vel_sp = setpoint;
    }

	SETDEBUG(dbchan, 65, 1);


    // double acc_sp = 0;
    // updatePID(&velocity, vel, vel_sp);
    // if (sp_mode < 2) {                              // Velocity Loop
    //     acc_sp = getCV(&velocity);
    // } else {
    //     acc_sp = setpoint;
    // }

    double curr_sp = 0;
    // updatePID(&acceleration, acc, acc_sp);
    // if (sp_mode < 3) {
    //     curr_sp = getCV(&acceleration);
    // } else {
    //     curr_sp = setpoint;
    // }

    curr_sp = vel_sp;
    
    // TODO Lookup Table for acceleration -> Current.
    // curr_sp = curr_sp * (-2300 <= curr_sp && curr_sp <= 2300) + 2300 * (curr_sp > 2300) - 2300 * (curr_sp < -2300);


    updatePID(&current, currentCurrent, curr_sp);
    cv = getCV(&current);
    // debug = cv;
	SETDEBUG(dbchan, 66, 1);


    if (cv > 2500) {
        cv = 2500;
    }
    else if (cv < 0) {
        cv = 0;
    }

    *pwmControl = cv;
    // Output value.
    // *pwmControl = current2duty(curr_sp);
    // *pwmControl = curr_sp;
	SETDEBUG(dbchan, 67, 1);

}
