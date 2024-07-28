#include <inc/tm4c1294ncpdt.h>
#include "int_handler.h"
#include <stdint.h>

// This template will always work for port interrupts
void IntPortMHandler(void) // IRQ number 4, vector number 20
{
    // PORT E Layout: LED3, LED2, S1, S0: pins (3:0)
    GPIO_PORTM_DATA_R &= ~0x000C; // LEDs off, used to measure latency
    GPIO_PORTM_ICR_R = 0x0001; // clear interrupt source S0

    if (GPIO_PORTM_DATA_R & 0x0002) // poll S1
    { // counterclockwise direction
        ucDirection = COUNTERCLOCKWISE;
        GPIO_PORTM_DATA_R = GPIO_PORTM_DATA_R | (1 << 3); // LED 3 on
    }
    else // clockwise direction
    {
        ucDirection = CLOCKWISE;
        GPIO_PORTM_DATA_R = GPIO_PORTM_DATA_R | (1 << 2); // LED 2 on
    }
}
