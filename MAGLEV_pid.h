#ifndef MAGLEV_pid_h_
#define MAGLEV_pid_h_

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
void stepPIDs(double magDistance, double setpoint, int sp_mode, double currentCurrent, double* pwmControl);

#endif
