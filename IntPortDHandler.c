#include <inc/tm4c1294ncpdt.h>
#include "int_handler.h"
#include <stdint.h>

void IntPortDHandler(void)
{
    GPIO_PORTD_AHB_ICR_R = 0x0001; // clear interrupt source S1

    if (timerOn == 0) { // timer is off
        TIMER0_CTL_R |= 0x0001; // enable Timer 0A
        timerOn = 1; // timer has now been started
        i = 0; // reset overflow counter
    } else {
        TIMER0_CTL_R &= ~0x0001; // disable timer 0
        uint32_t ticks = (0xFFFF - TIMER0_TAR_R);
        float cycleTime = (i * 65536 + ticks) / 16000000.0; // cycleTime in seconds
        float r = 0.0101; // measured motor radius is 1.01 cm
        float circ = 2 * 3.14159 * r; // motor circumference in m
        speed1 = (circ / cycleTime) * 3.6 * 16; // motor speed in km/h - multiply by factor 16
        //speed = ((float)(i * 65536 + TIMER0_TAR_R)) / 16000000.0; // calculate speed

        i = 0; // reset i
        timerOn = 0; // timer has now been stopped
    }
}

