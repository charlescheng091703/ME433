#ifndef UART__H__
#define UART__H__

#include <xc.h>                     // processor SFR definitions
#include <sys/attribs.h>            // __ISR macro

#define PIC32_USER PORTDbits.RD4     // USER button on the NU32 board
#define PIC32_SYS_FREQ 48000000    // 48 million Hz

void UART1_Startup(void);
void ReadUART1(char * string, int maxLength);
void WriteUART1(const char * string);

#endif // UART__H__