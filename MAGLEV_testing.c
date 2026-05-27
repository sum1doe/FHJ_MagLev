#include <ctype.h>
#include <string.h>
#include "F2806x_Device.h"     // DSP2833x Headerfile Include File
#include "F2806x_Examples.h"   // DSP2833x Examples Include File
#include "defines.h"

#include <math.h>

#include "MAGLEV_testing.h"

// TODO: Make these ramfuncs.

volatile int16 constantsI[] = {1,2,3,4,5,6,7,8,9,10};
volatile float constantsF[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};

int16 curveExecIMA(int16 x) {
    int i = 0;
    int16 out = x;
    for (i = 0; i < 5; i++) {
        out += constantsI[2*i];
        out *= constantsI[2*i+1];
    }
    return out;
}

float curveExecFMA(float x) {
    int i = 0;
    float out = x;
    for (i = 0; i < 5; i++) {
        out += constantsF[2*i];
        out *= constantsF[2*i+1];
    }
    return out;
}

int16 curveExecIP(int16 x) {
    return 18 * (int16) powf((float) x, 3.14159);
}

float curveExecFP(float x) {
    return 31.9123 * powf((float) x, 3.14159);
}
