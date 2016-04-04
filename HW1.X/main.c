#include <xc.h>
#include <plib.h>
#include <sys/attribs.h>

// DEVCFG0
#pragma config DEBUG = OFF
#pragma config JTAGEN = OFF             // JTAG Enable (JTAG Disabled)
#pragma config ICESEL = ICS_PGx1        // ICE/ICD Comm Channel Select (Communicate on PGEC1/PGED1)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)
// DEVCFG1
#pragma config FNOSC = PRIPLL
#pragma config FSOSCEN = OFF
#pragma config IESO = OFF
#pragma config POSCMOD = HS
#pragma config OSCIOFNC = OFF
#pragma config FPBDIV = DIV_1
#pragma config FCKSM = CSDCMD
//should add WDT stuff

// DEVCFG2
#pragma config FPLLIDIV = DIV_2
#pragma config FPLLMUL = MUL_24
#pragma config FPLLODIV = DIV_2
#pragma config UPLLIDIV = DIV_2
#pragma config UPLLEN = ON
// DEVCFG3
#pragma config USERID = 1
#pragma config PMDL1WAY = OFF
#pragma config IOL1WAY = OFF
#pragma config FUSBIDIO = ON
#pragma config FVBUSONIO = ON


#define SYS_FREQ (48000000)


void delay(void);

void main() {
   __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;
    
    // do your TRIS and LAT commands here
    TRISA = 0xFFCF; 
    TRISB = 0xFFFF;
__builtin_enable_interrupts();
   // SYSTEMConfigPerformance(48000000);
  
    while(1) {
       
        PORTAINV = 0x0010;
        //PORTAbits.RA4 = 1;
        delay();
        while(!PORTBbits.RB4){}
}
}

void delay(void) {
    int delaytime = 12000; //in hz, core timer freq is half sysfreq
    int starttime;
    starttime = _CP0_GET_COUNT();
    
    while ((int)_CP0_GET_COUNT()-starttime < delaytime){}
    
    // resets the core timer count
    //WriteCoreTimer( 0);
}

