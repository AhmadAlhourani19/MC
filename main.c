#include <inc/tm4c1294ncpdt.h>
#include <stdio.h>
#include <stdint.h>
#include "int_handler.h"

#include "inc/hw_types.h" // required for sysctl.h
#include "driverlib/sysctl.h" // required for SysCtlClockFreqSet

/* global variables */
volatile unsigned char ucDirection = CLOCKWISE;
volatile unsigned long speed1 = 0;
volatile unsigned char timerOn = 0;
volatile unsigned long i = 0;

void configPortM();
void configPortD();
void configTimer0A();
void configTestTimer1A();
void configUART7(void);
void configPortC(void);

char direction[20] = "Clockwise"; // Define and initialize direction
char speed[20] = "0"; // Define and initialize speed
char teamName[20] = "The Boys"; // Define and initialize team name

void main(){
    configPortM();
    configPortD();
    configTimer0A();
    configTestTimer1A();
    configUART7();
    configPortC();

    UART7_SendSpecialCommand(0x80, 255); // Set backlight to 100%
    UART7_DR_R = 1234557;

    while(1) { // busy waiting
        if ((TIMER1_RIS_R & (1 << 0)) != 0) { // wait for timeout
            TIMER1_ICR_R |= (1 << 0); // clear timeout flag
            UART7_SendCommand(CLEAR_DISPLAY, 0x01);  // Clear the display
            char buffer2[20] = "";
            sprintf(buffer2, "%s", speed);           // Use the updated speed value
            writeToLCD(teamName, " ", direction, buffer2);  // Write to LCD
        }
    }
}

void configPortM() // PE(0):in+irq, PE(1):in, PE(3..2):out
{
    SYSCTL_RCGCGPIO_R |= (1 << 11); // enable clock port M
    while (!(SYSCTL_PRGPIO_R & (1 << 11))); // wait before accessing port

    GPIO_PORTM_DEN_R |= 0x000F; // enable pins
    GPIO_PORTM_DIR_R |= 0x000C; // PortM(3..2) output
    GPIO_PORTM_DATA_R |= 0x000C; // PortM(3..2) switch on LEDs

    GPIO_PORTM_IS_R &= ~0x01; // sense to "edge-sensitive"
    GPIO_PORTM_IBE_R &= ~0x01; // trigger set to "single edge"
    GPIO_PORTM_IEV_R |= 0x01; // interrupt event to rising edge
    GPIO_PORTM_ICR_R |= 0x01; // clear interrupt PORTM(0)
    GPIO_PORTM_IM_R |= 0x01; // unmask PORTM(0) = S1

    NVIC_EN2_R |= (1 << (72 - 64)); // enable PortM interrupt in NVIC
}

void configPortD() // PD(0): input
{
    SYSCTL_RCGCGPIO_R |= (1 << 3); // enable clock port D
    while (!(SYSCTL_PRGPIO_R & (1 << 3))); // wait before accessing port

    GPIO_PORTD_AHB_DEN_R |= 0x0001; // enable pin
    GPIO_PORTD_AHB_DIR_R &= ~0x01; // PortD(0) input

    GPIO_PORTD_AHB_IS_R &= ~0x01; // sense to "edge-sensitive"
    GPIO_PORTD_AHB_IBE_R &= ~0x01; // trigger set to "single edge"
    GPIO_PORTD_AHB_IEV_R |= 0x01; // interrupt event to rising edge
    GPIO_PORTD_AHB_ICR_R |= 0x01; // clear interrupt PORTD(0)
    GPIO_PORTD_AHB_IM_R |= 0x01; // unmask PORTD(0)

    NVIC_EN0_R |= (1 << 3); // enable PortD interrupt (Int#3/Vec#19) in NVIC
}

void configTimer0A()
{
    /* Configure timer 0 */
    SYSCTL_RCGCTIMER_R |= (1 << 0); // system clock on timer 0
    while (!(SYSCTL_PRTIMER_R & 0x01)); // wait for timer 0 activation
    TIMER0_CTL_R &= ~0x0001; // disable timer 0 by clearing bit 0
    TIMER0_CFG_R = 0x04; // 16 bit mode
    TIMER0_TAMR_R = 0x02; // periodic mode, match disabled
    TIMER0_TAPR_R = 1 - 1; // prescaler
    TIMER0_TAILR_R = 65536 - 1; // highest ILV value

    TIMER0_IMR_R = 0x01; // enable Timer0A timeout interrupt
    NVIC_EN0_R |= (1 << 19); // enable Timer0A interrupt in NVIC

    TIMER0_CTL_R |= 0x0001; // enable Timer 0A by setting bit 0
}

void configTestTimer1A()
{
    SYSCTL_RCGCTIMER_R |= (1 << 1); // system clock on timer 1
    while (!(SYSCTL_PRTIMER_R & 0x02)); // wait for timer 1 activation
    TIMER1_CTL_R &= ~0x0001; // disable timer 1
    TIMER1_CFG_R = TIMER_CFG_32_BIT_TIMER; // 32-bit mode
    TIMER1_TAMR_R = TIMER_TAMR_TAMR_PERIOD; // periodic mode, match disabled
    TIMER1_TAILR_R = 1 * 16000000 - 1; // count from highest value downwards
    TIMER1_CTL_R |= 0x0001; // enable Timer 1A, start the stopwatch
}

void configPortC(void){
    SYSCTL_RCGCGPIO_R |= (1<<2); // Enable clock for Port C
    while(!(SYSCTL_PRGPIO_R & (1<<2))); // Wait for clock to stabilize

    GPIO_PORTC_AHB_DEN_R |= 0x20; // Enable functionality on PC4 and PC5
    GPIO_PORTC_AHB_DIR_R |= 0x20; // PC5 as o/p and PC4 as i/p
    GPIO_PORTC_AHB_AFSEL_R |= 0x20; // Enable alternate function on PC4 and PC5
    GPIO_PORTC_AHB_PCTL_R |= 0x00100000; // Set PC4 and PC5 to UART7
}

void configUART7(void){
    SYSCTL_RCGCUART_R |= 0x80;          // Switch on clock for UART7
    while(!(SYSCTL_PRUART_R & 0x80));   // Check for stable clock
    UART7_CTL_R &= ~0x01;               // Disable UART7 for configuration
    UART7_IBRD_R = 104;                 // Set integer baud rate divisor
    UART7_FBRD_R = 11;                  // Set fractional baud rate divisor
    UART7_LCRH_R = 0x60;                // Serial format 8N1 (8 data bits, no parity, 1 stop bit)
    UART7_CTL_R |= 0x301;               // Enable UART7, TXE, RXE
    UART7_ICR_R = 0xE7FF;               // Clear all interrupts
    UART7_IM_R |= (1 << 5);             // Enable UART7 Tx interrupt
    NVIC_EN1_R = (1 << (60 - 32));      // Enable interrupt 60 in NVIC (UART7);
}
