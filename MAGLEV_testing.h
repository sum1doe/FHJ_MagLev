#ifndef MAGLEV_testing_h
#define MAGLEV_testing_h

volatile int16 constantsI[] = {1,2,3,4,5,6,7,8,9,10};
volatile float constantsF[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};

int16 curveExecIMA(int16 x);
float curveExecFMA(float x);

int16 curveExecIP(int16 x);
float curveExecFP(float x);

#endif