#include <xc.h>
#include <plib.h>
// DEVCFG0
#pragma config JTAGEN = ON             // JTAG Enable (JTAG Disabled)
#pragma config ICESEL = ICS_PGx1        // ICE/ICD Comm Channel Select (Communicate on PGEC1/PGED1)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)

#pragma config FNOSC = PRIPLL
#pragma config FSOSCEN = OFF
#pragma config IESO = OFF
#pragma config POSCMOD = XT
#pragma config OSCIOFNC = OFF
#pragma config FPBDIV = DIV_1
#pragma config FPLLIDIV = DIV_2
#pragma config FPLLMUL = MUL_24
#pragma config FPLLODIV = DIV_2
#pragma config UPLLIDIV = DIV_2
#pragma config UPLLEN = ON

#define SYS_FREQ (48000000)


void delay(void);

void main() {
    SYSTEMConfigPerformance(48000000);
  TRISA = 0xFFCF;   
    while(1) {
       
        PORTAINV = 0x0010;
        //PORTAbits.RA4 = 1;
        delay();
}
}

void delay(void) {
    int delaytime = 24000; //in hz
    int starttime;
    starttime = ReadCoreTimer();
    
    while ((int)ReadCoreTimer()-starttime < delaytime){}
}

