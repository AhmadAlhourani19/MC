#include <inc/tm4c1294ncpdt.h>
#include "int_handler.h"
#include <stdio.h>

volatile const char *txBuffer = 0;
volatile uint32_t txBufferSize = 0;
volatile uint32_t txBufferIndex = 0;

void UART7_SendString(char *str) {
    while(*str != '\0') {                  // Continue until the end of the string
        UART7_SendChar(*str);               // Send each character
        str++;
    }
}

void UART7_SendChar(char data) {
    while(UART7_FR_R & 0x20);            // Wait until TXFF is 0 (Transmit FIFO not full)
    UART7_DR_R = data;
}

void UART7_SendCommand(int cmd, int value) {
    UART7_SendChar(cmd);                // Command must be transmitted first followed by the desired value
    UART7_SendChar(value);
}

void UART7_SendSpecialCommand(uint8_t command, uint8_t value) {
    UART7_SendChar(command);
    UART7_SendChar(value);
}


// Print the rows to the LCD
void writeToLCD(char* row1, char* row2, char* row3, char* row4) {
    UART7_SendString(row1);                   // Send the first row string
    UART7_SendChar(CARRIAGE_RETURN);
    UART7_SendString(row2);                   // Send the second row string
    UART7_SendChar(CARRIAGE_RETURN);
    UART7_SendString(row3);                   // Send the third row string
    UART7_SendChar(CARRIAGE_RETURN);
    UART7_SendString(row4);                   // Send the fourth row string
    UART7_SendChar(CARRIAGE_RETURN);
}

// UART7 Interrupt Service Routine
void ISRUART7(void) {
    if (UART7_MIS_R & 0x20) { // Check if Tx interrupt is triggered
        UART7_ICR_R = 0x20;   // Clear the Tx interrupt

        // Update speed and direction in the ISR
        sprintf(speed, "%d", speed1);               // Example update to speed
        if (ucDirection) { // Direction is CLOCKWISE
            sprintf(direction, "Clockwise");  // Example update to direction
        } else {
            sprintf(direction, "Counterclockwise");  // Example update to direction
        }
    }
}


