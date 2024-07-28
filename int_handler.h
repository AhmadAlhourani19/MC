#ifndef INT_HANDLER_H_
#define INT_HANDLER_H_

#include <stdbool.h>
#include <stdint.h>

#define CLOCKWISE 1 // direction of rotation
#define COUNTERCLOCKWISE 0
#define ENCODERFACTOR 16 // Adjust based on your encoder specifications
#define CARRIAGE_RETURN 0x0D
#define CLEAR_DISPLAY 0xFE

void IntPortMHandler(void);
void IntPortDHandler(void);
void Timer0AHandler(void);
void UART7_SendString(char *str);
void UART7_SendChar(char data);
void UART7_SendCommand(int cmd, int value);
void writeToLCD(char* row1, char* row2, char* row3, char* row4);
void UART7_SendSpecialCommand(uint8_t command, uint8_t value);
void UART7_SendInt(int value);
void ISRUART7(void);

extern volatile unsigned char ucDirection; // direction of rotation
extern volatile unsigned long speed1; // calculated speed
extern volatile unsigned long i; // timer overflow counter
extern volatile unsigned char timerOn; // timer on/off flag

extern char direction[20]; // Add this line
extern char speed[20]; // Add this line
extern char teamName[20]; // Add this line

#endif
