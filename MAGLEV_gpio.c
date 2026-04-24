///////////////////////////////////
//
//    BLDC Driver for FHJ
//    (c) Park 2026
//
///////////////////////////////////

#include <ctype.h>
#include <string.h>
#include "F2806x_Device.h"
#include "F2806x_Examples.h"

void InitGPIO(void);

void InitGPIO(void)
{
    EALLOW;

    // PWM setup
    GpioCtrlRegs.GPAMUX1.bit.GPIO0      = 1;        // 0=GPIO               1=EPWM1A     2=rsvd       3=rsvd
    GpioCtrlRegs.GPAQSEL1.bit.GPIO0     = 0;        // No qualification for all group A GPIO 0-15
    GpioCtrlRegs.GPADIR.bit.GPIO0       = 0;        // 0 - Input ; 1 - Output;
    GpioCtrlRegs.GPAPUD.bit.GPIO0       = 1;        // 0-Pullup enabled, 1-Pullup disabled

    GpioCtrlRegs.GPAMUX1.bit.GPIO1      = 1;        // 0=GPIO               1=EPWM1B     2=rsvd       3=COMP1OUT
    GpioCtrlRegs.GPAQSEL1.bit.GPIO1     = 0;        // No qualification for all group A GPIO 0-15
    GpioCtrlRegs.GPADIR.bit.GPIO1       = 0;        // 0 - Input ; 1 - Output;
    GpioCtrlRegs.GPAPUD.bit.GPIO1       = 1;        // 0-Pullup enabled, 1-Pullup disabled

    GpioCtrlRegs.GPAMUX1.bit.GPIO2      = 1;        // 0=GPIO               1=EPWM2A     2=rsvd       3=rsvd
    GpioCtrlRegs.GPAQSEL1.bit.GPIO2     = 0;        // No qualification for all group A GPIO 0-15
    GpioCtrlRegs.GPADIR.bit.GPIO2       = 0;        // 0 - Input ; 1 - Output;
    GpioCtrlRegs.GPAPUD.bit.GPIO2       = 1;        // 0-Pullup enabled, 1-Pullup disabled

    GpioCtrlRegs.GPAMUX1.bit.GPIO3      = 1;        // 0=GPIO               1=EPWM2B     2=SPISOMIA   3=COMP2OUT
    GpioCtrlRegs.GPAQSEL1.bit.GPIO3     = 0;        // No qualification for all group A GPIO 0-15
    GpioCtrlRegs.GPADIR.bit.GPIO3       = 0;        // 0 - Input ; 1 - Output;
    GpioCtrlRegs.GPAPUD.bit.GPIO3       = 1;        // 0-Pullup enabled, 1-Pullup disabled

    GpioCtrlRegs.GPAMUX1.bit.GPIO4      = 1;        // 0=GPIO               1=EPWM3A     2=rsvd       3=rsvd
    GpioCtrlRegs.GPAQSEL1.bit.GPIO4     = 0;        // No qualification for all group A GPIO 0-15
    GpioCtrlRegs.GPADIR.bit.GPIO4       = 0;        // 0 - Input ; 1 - Output;
    GpioCtrlRegs.GPAPUD.bit.GPIO4       = 1;        // 0-Pullup enabled, 1-Pullup disabled

    GpioCtrlRegs.GPAMUX1.bit.GPIO5      = 1;        // 0=GPIO               1=EPWM3B     2=SPISIMOA   3=ECAP1
    GpioCtrlRegs.GPAQSEL1.bit.GPIO5     = 0;        // No qualification for all group A GPIO 0-15
    GpioCtrlRegs.GPADIR.bit.GPIO5       = 0;        // 0 - Input ; 1 - Output;
    GpioCtrlRegs.GPAPUD.bit.GPIO5       = 1;        // 0-Pullup enabled, 1-Pullup disabled

    GpioCtrlRegs.GPAMUX1.bit.GPIO6      = 1;        // 0=GPIO               1=EPWM3B     2=SPISIMOA   3=ECAP1
    GpioCtrlRegs.GPAQSEL1.bit.GPIO6     = 0;        // No qualification for all group A GPIO 0-15
    GpioCtrlRegs.GPADIR.bit.GPIO6       = 1;        // 0 - Input ; 1 - Output;
    GpioCtrlRegs.GPAPUD.bit.GPIO6       = 1;        // 0-Pullup enabled, 1-Pullup disabled

    // GPIO Mux configuration for SCI: GPIOs 28 and 29
    GpioCtrlRegs.GPAMUX2.bit.GPIO28     = 1;        // 0=GPIO, 1=SCIRXDA, 2=SDAA, 3=TZ2
    GpioCtrlRegs.GPAQSEL2.bit.GPIO28    = 0;        // No qualification for all group A GPIO 16-31
    GpioCtrlRegs.GPADIR.bit.GPIO28      = 0;        // 0 - Input ; 1 - Output;
    GpioCtrlRegs.GPAPUD.bit.GPIO28      = 0;        // 0-Pullup enabled, 1-Pullup disabled

    GpioCtrlRegs.GPAMUX2.bit.GPIO29     = 1;        // 0=GPIO, 1=SCITXDA, 2=SCLA, 3=TZ3
    GpioCtrlRegs.GPAQSEL2.bit.GPIO29    = 0;        // No qualification for all group A GPIO 16-31
    GpioCtrlRegs.GPADIR.bit.GPIO29      = 1;        // 0 - Input ; 1 - Output;
    GpioCtrlRegs.GPAPUD.bit.GPIO29      = 0;        // 0-Pullup enabled, 1-Pullup disabled

    // FHJ IO board
    // Hall sensor (C)
    GpioCtrlRegs.GPAMUX2.bit.GPIO20     = 0;        // 0=GPIO               1=EQEP1A     2=MDXA       3=COMP1OUT
    GpioCtrlRegs.GPAQSEL2.bit.GPIO20    = 0;        // No qualification for all group A GPIO 16-31
    GpioCtrlRegs.GPADIR.bit.GPIO20      = 0;        // 0 - Input ; 1 - Output;
    GpioCtrlRegs.GPAPUD.bit.GPIO20      = 0;        // 0-Pullup enabled, 1-Pullup disabled
    // Hall sensor (B)
    GpioCtrlRegs.GPAMUX2.bit.GPIO21     = 0;        // 0=GPIO               1=EQEP1B     2=MDRA       3=COMP2OUT
    GpioCtrlRegs.GPAQSEL2.bit.GPIO21    = 0;        // No qualification for all group A GPIO 16-31
    GpioCtrlRegs.GPADIR.bit.GPIO21      = 0;        // 0 - Input ; 1 - Output;
    GpioCtrlRegs.GPAPUD.bit.GPIO21      = 0;        // 0-Pullup enabled, 1-Pullup disabled
    // Hall sensor (A)
    GpioCtrlRegs.GPAMUX2.bit.GPIO23     = 0;        // 0=GPIO               1=EQEP1I     2=MFSXA      3=SCIRXDB
    GpioCtrlRegs.GPAQSEL2.bit.GPIO23    = 0;        // No qualification for all group A GPIO 16-31
    GpioCtrlRegs.GPADIR.bit.GPIO23      = 0;        // 0 - Input ; 1 - Output;
    GpioCtrlRegs.GPAPUD.bit.GPIO23      = 0;        // 0-Pullup enabled, 1-Pullup disabled

    // GATE_EN
    GpioCtrlRegs.GPBMUX2.bit.GPIO50     = 0;        // 0=GPIO               1=EQEP1A    2=MDXA       3=TZ1
    GpioCtrlRegs.GPBQSEL2.bit.GPIO50    = 0;        // No qualification for all group A GPIO 32-44
    GpioCtrlRegs.GPBDIR.bit.GPIO50      = 1;        // 0 - Input ; 1 - Output;
    GpioCtrlRegs.GPBPUD.bit.GPIO50      = 0;        // 0-Pullup enabled, 1-Pullup disabled

    // Encoder
    GpioCtrlRegs.GPBMUX2.bit.GPIO54     = 2;        // 0=GPIO               1=SPISIMOA  2=EQEP2A     3=HRCAP1
    GpioCtrlRegs.GPBQSEL2.bit.GPIO54    = 0;        // No qualification for all group A GPIO 32-44
    GpioCtrlRegs.GPBDIR.bit.GPIO54      = 1;        // 0 - Input ; 1 - Output;
    GpioCtrlRegs.GPBPUD.bit.GPIO54      = 0;        // 0-Pullup enabled, 1-Pullup disabled

    GpioCtrlRegs.GPBMUX2.bit.GPIO55     = 2;        // 0=GPIO               1=SPISOMIA  2=EQEP2B     3=HRCAP2
    GpioCtrlRegs.GPBQSEL2.bit.GPIO55    = 0;        // No qualification for all group A GPIO 32-44
    GpioCtrlRegs.GPBDIR.bit.GPIO55      = 1;        // 0 - Input ; 1 - Output;
    GpioCtrlRegs.GPBPUD.bit.GPIO55      = 0;        // 0-Pullup enabled, 1-Pullup disabled

    GpioCtrlRegs.GPBMUX2.bit.GPIO56     = 2;        // 0=GPIO               1=SPICLKA   2=EQEP2I     3=HRCAP3
    GpioCtrlRegs.GPBQSEL2.bit.GPIO56    = 0;        // No qualification for all group A GPIO 32-44
    GpioCtrlRegs.GPBDIR.bit.GPIO56      = 1;        // 0 - Input ; 1 - Output;
    GpioCtrlRegs.GPBPUD.bit.GPIO56      = 0;        // 0-Pullup enabled, 1-Pullup disabled

    // LED
    GpioCtrlRegs.GPBMUX1.bit.GPIO34     = 0;        // 0=GPIO               1=COMP2OUT  2=rsvd       3=COMP3OUT
    GpioCtrlRegs.GPBQSEL1.bit.GPIO34    = 0;        // No qualification for all group A GPIO 32-44
    GpioCtrlRegs.GPBDIR.bit.GPIO34      = 1;        // 0 - Input ; 1 - Output;
    GpioCtrlRegs.GPBPUD.bit.GPIO34      = 0;        // 0-Pullup enabled, 1-Pullup disabled

    // End of MUX initialization
    EDIS;
}


//===========================================================================
// No more.
//===========================================================================

