#include "F2806x_Device.h"
// #include <math.h>

// __attribute__((ramfunc))
// int16 coilInterferenceFunc(double x, int dir) {
//     if (dir == 0) {
//         return -(int16) (0.359427 * pow(x, 0.648402));
//     }
//     // return (int16) (0.109508 * pow(x, 0.797352));
//     return (int16) (x / 40);
// }

// __attribute__((ramfunc))
// double shunt2current(int16 input) {
//     return 0.136 * pow((double) input, 1.5);
// }

// __attribute__((ramfunc))
// double acc2curr(double position) {
//     if (position < 0) {
//         return 0;
//     }
//     return 17 + 28.0734 * pow(position/10, 1.29583);
// }

// Decent approximation from data gathered.
// https://www.wolframalpha.com/input?i2d=true&i=%7B%7B1%2C7%7D%2C%7B5%2C219%7D%2C%7B33%2C405%7D%2C%7B99%2C606%7D%2C%7B211%2C830%7D%2C%7B338%2C1024%7D%2C%7B456%2C1155%7D%2C%7B629%2C1374%7D%2C%7B816%2C1555%7D%2C%7B1088%2C1812%7D%2C%7B1346%2C2020%7D%2C%7B1749%2C2304%7D%2C%7B2228%2C2610%7D%2C%7B2515%2C2830%7D%7Dpower+fit 
// double current2duty(double current) {
//     return 55.6*sqrt(current);
// }