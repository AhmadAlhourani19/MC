#include <inc/tm4c1294ncpdt.h>
#include "int_handler.h"
#include <stdint.h>

void Timer0AHandler(void)
{
    TIMER0_ICR_R |= (1 << 0); // clear time-out flag
    i++; // increment after timeout
}
