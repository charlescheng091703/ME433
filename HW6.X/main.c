#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "i2c_master_noint.h"

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

void blink(void);
void write_i2c(unsigned char address, unsigned char registr, unsigned char value);
unsigned char read_i2c(unsigned char address, unsigned char registr);

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
    
    i2c_master_setup();
    
    __builtin_enable_interrupts();
    
    unsigned char send_address = SLAVE_ADDRESS << 1;
    unsigned char rcvd;
    while (1) {
        blink();
        rcvd = read_i2c(send_address, GPIO);
        if (rcvd & 0b1 == 0b1) {
            write_i2c(send_address, OLAT, 0b10000000);
        }
        else {
            write_i2c(send_address, OLAT, 0b00000000);
        }
    }
}

void blink(void) {
    LATAbits.LATA4 = !LATAbits.LATA4;
    _CP0_SET_COUNT(0);
    while (_CP0_GET_COUNT() < 3000000) {}
}

void write_i2c(unsigned char address, unsigned char registr, unsigned char value) {
    i2c_master_start();
    i2c_master_send(address);
    i2c_master_send(registr);
    i2c_master_send(value);
    i2c_master_stop();
}

unsigned char read_i2c(unsigned char address, unsigned char registr) {
    unsigned char rcvd;
    i2c_master_start();
    i2c_master_send(address);
    i2c_master_send(registr);
    i2c_master_restart();
    i2c_master_send(address | 1);
    rcvd = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    return rcvd;
}