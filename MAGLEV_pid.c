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
#include "math.h"

#ifndef DataBuffer
#define DataBuffer 5
#endif

double dataBuf[DataBuffer];
int dataIndex = -1;

double cv = 0;

typedef struct PIDStruct {
    // Input
    double sp;
    double data;
    // Config
    double k;
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

void initPID(PID* pid, double k, double kp, double ki, double kd);
void updatePID(PID* pid, double data, double sp);
double getCV(PID* pid);
void delPID(PID* pid);

    
void initPID(PID* pid, double k, double kp, double ki, double kd) {
    pid->k = k;
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->kM = 0.1; // Effectively timescale for I coefficient.

    pid->prevData = 0;
    pid->prevI = 0;
}

void updatePID(PID* pid, double data, double sp) {
    pid->prevData = pid->data;
    pid->data = data;
    pid->sp = sp;
}

double getCV(PID* pid) {
    pid->prevI = pid->kM * (pid->sp - pid->data) + (1-pid->kM) * pid->prevI;
    pid->cv =   pid->k * (pid->kp * (pid->sp - pid->data) +
                pid->ki * (pid->prevI) * 10 + 
                pid->kd * (pid->data-pid->prevData));

    return pid->cv;
}

// Decent approximation from data gathered.
// https://www.wolframalpha.com/input?i2d=true&i=%7B%7B1%2C7%7D%2C%7B5%2C219%7D%2C%7B33%2C405%7D%2C%7B99%2C606%7D%2C%7B211%2C830%7D%2C%7B338%2C1024%7D%2C%7B456%2C1155%7D%2C%7B629%2C1374%7D%2C%7B816%2C1555%7D%2C%7B1088%2C1812%7D%2C%7B1346%2C2020%7D%2C%7B1749%2C2304%7D%2C%7B2228%2C2610%7D%2C%7B2515%2C2830%7D%7Dpower+fit 
double current2duty(double current) {
    return 55.6*sqrt(current);
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

extern int16 debug;

void stepPIDs(double magDistance, double setpoint, int sp_mode, double currentCurrent, double* pwmControl) {
    // Data Buffer Updates
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

    // PID Control
    // Setpoint doesn't matter if getCV isn't called.
    updatePID(&position, magDistance, setpoint);
    double vel_sp = 0;
    if (sp_mode < 1) {                              //  Position Loop
        vel_sp = getCV(&position);
    } else {
        vel_sp = setpoint;
    }

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

    if (cv > 2500) {
        cv = 2500;
    }
    if (cv < 0) {
        cv = 0;
    }

    *pwmControl = cv;
    // Output value.
    // *pwmControl = current2duty(curr_sp);
    // *pwmControl = curr_sp;
}
