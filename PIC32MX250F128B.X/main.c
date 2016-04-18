#include <xc.h>
#include <plib.h>
#include <sys/attribs.h>
#include <math.h>
#include <mathf.h>

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
#pragma config FPBDIV = DIV_8 //temporarilly set at 8 so PBUS is at 6Mhz for debugging
#pragma config FCKSM = CSDCMD
#pragma config WDTPS = PS1048576
#pragma config WINDIS = OFF
#pragma config FWDTEN = OFF
#pragma config FWDTWINSZ = WINSZ_25

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
#define CS LATBbits.LATB15       // chip select pin
#define pi 3.14159265358979323846



 void delay(int time);
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
    TRISB = 0b0001111111110011;

    __builtin_enable_interrupts();
   // SYSTEMConfigPerformance(48000000);
    RPB13Rbits.RPB13R = 0b0011;
    SDI1Rbits.SDI1R = 0b0000; //A1
    PORTAbits.RA4 = 0;
    CS = 1;
    unsigned char x = 0; //sine counter
    unsigned char y = 0; //triangle counter
    char pressed = 0;
    spi1_start();
    char counter = 0;
    char m = 100; //(triangle wave frequency is 1000/2m)
    unsigned char voltage = 0;
    unsigned char channel = 0;
    
    while(1) {
        x++;
        x = x%100;
        y++;
        y = y%200;

       //delay (6000); //total delay of 24000 per cycle (1000/s)
       PORTAINV = 0x0010;
       //PORTAbits.RA4 = 1;
//        delay(12000);
//        if(!PORTBbits.RB4){
//            pressed = 1;
//        }
//        if (pressed){
//            counter = counter +5;
       
       
            CS = 0;
            channel = 0b1;
            voltage = floor(100*sin((x*2*pi)/100)+100);
           
            //char voltage = 0b10101001;
            spi1_set(channel,voltage);
            delay(6000);
            CS = 1;
            delay(6000);
            
            CS = 0;
          
            channel = 0b0;
            //voltage = 2*(m - abs((y)- m)); this is an upright triangle
            voltage = y;
            spi1_set(channel,voltage);
            //spi1_set(0b1,0b10101001);
            delay(6000);
            
            CS = 1;
            delay(6000);
            
//            pressed = 0;
//            delay(24000000); //1 second delay
//        
//                            }
            }
}
void delay(int time) {
    int delaytime = time; //in hz, core timer freq is half sysfreq
    int starttime;
    starttime = _CP0_GET_COUNT();
    
    while ((int)_CP0_GET_COUNT()-starttime < delaytime){}
    
    // resets the core timer count
    //WriteCoreTimer( 0);
}

