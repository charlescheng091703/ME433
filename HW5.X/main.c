#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "spi.h"
#include <math.h>

// DEVCFG0
#pragma config DEBUG = OFF // disable debugging
#pragma config JTAGEN = OFF // disable jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // disable flash write protect
#pragma config BWP = OFF // disable boot write protect
#pragma config CP = OFF // disable code protect

// DEVCFG1
#pragma config FNOSC = FRCPLL // use fast frc oscillator with pll
#pragma config FSOSCEN = OFF // disable secondary oscillator
#pragma config IESO = OFF // disable switching clocks
#pragma config POSCMOD = OFF // primary osc disabled
#pragma config OSCIOFNC = OFF // disable clock output
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // disable clock switch and FSCM
#pragma config WDTPS = PS1048576 // use largest wdt value
#pragma config WINDIS = OFF // use non-window mode wdt
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz fast rc internal oscillator
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations

unsigned short construct_bits_to_send(int channel, float voltage);
float sin_function(float t);
float triangle_function(float t);

int main() {

    __builtin_disable_interrupts(); // disable interrupts while initializing things

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    TRISAbits.TRISA4 = 0;
    LATAbits.LATA4 = 0;
    TRISBbits.TRISB4 = 1;
    
    initSPI();
    
    __builtin_enable_interrupts();
    
    unsigned char got_back;
    unsigned short p; 
    float v;
    float t = 0.0;
    while (1) {
        v = sin_function(t);
        p = construct_bits_to_send(0, v);
        LATAbits.LATA0 = 0; // bring CS low
        got_back = spi_io(p >> 8);
        got_back = spi_io(p);
        LATAbits.LATA0 = 1; // bring CS high
        
        v = triangle_function(t);
        p = construct_bits_to_send(1, v);
        LATAbits.LATA0 = 0; // bring CS low
        got_back = spi_io(p >> 8);
        got_back = spi_io(p);
        LATAbits.LATA0 = 1; // bring CS high
        
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT() < 240000) {} // 0.01 second
        t += 0.01;
    }
}

unsigned short construct_bits_to_send(int channel, float voltage) {
    unsigned short p; 
    unsigned short x = 4096/3.3*voltage;
    p = channel << 15;
    p = p | (0b111 << 12);
    p = p | (x & 0b111111111111);
    
    return p;
}

float sin_function(float t) {
    return 1.65*sin(2*3.1416*2*t) + 1.65;
}

float triangle_function(float t) {
    float t1 = t - floor(t);
    if (t1 <= 0.5) {
        return 6.6*t1;
    }
    else {
        return -6.6*t1 + 6.6;
    }
}