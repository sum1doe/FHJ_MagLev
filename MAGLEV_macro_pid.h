#ifndef MAGLEV_macro_pid_h_
#define MAGLEV_macro_pid_h_

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
void initAllPIDs();

#define updatePID(pid, Data, SP)\
    pid.prevData=pid.data;\
    pid.data=Data;\
    pid.sp=SP;

// I know it's bad practice, sorry...
// The \ here is to escape the newline, allowing multiline defines
#define getCV(pid)\
    pid.k * (doP(pid) + doI(pid) - doD(pid))

#define doP(pid) \
    pid.kp * (pid.sp - pid.data)

#define doI(pid) \
    pid.prevI = pid.kM * (pid.sp - pid.data) + (1-pid.kM) * pid.prevI

#define doD(pid) \
    pid.kd * (pid.data-pid.prevData)

// You only need to implement StepPIDs yourself, wherever.

#endif
