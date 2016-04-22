#include <xc.h>
#include <plib.h>
#include <sys/attribs.h>
#include <math.h>


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
#define IODIR 0x00
#define IPOL 0x01
#define GPINTEN 0x02
#define DEFVAL 0x03
#define INTCON 0x04
#define IOCON 0x05
#define GPPU 0x06
#define INTF 0x07
#define INTCAP  0x08
#define GPIO    0x09
#define OLAT 0x0A
#define MCP23008 0b00100111
char counter = 1;

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
   i2c_master_setup();
    ANSELBbits.ANSB2 = 0; //SDA2 set to digital
    ANSELBbits.ANSB3 = 0; //SCL2 set to digital
 
    __builtin_enable_interrupts();
   // SYSTEMConfigPerformance(48000000);
    
    RPB13Rbits.RPB13R = 0b0011; //SDO
    SDI1Rbits.SDI1R = 0b0000; //A1
    PORTAbits.RA4 = 0;
   unsigned char x = 0; //sine counter
    unsigned char y = 0; //triangle counter
    char pressed = 0; //for tracking button logic
    spi1_start();
    char counter = 0;
    char m = 100; //(triangle wave frequency is 1000/2m)
    unsigned char voltage = 0;
    unsigned char channel = 0;
    unsigned char i2cdata = 0;
    
    i2c_master_start();
    i2c_master_send(MCP23008<<1);
    i2c_master_send(IOCON);
    i2c_master_send(0b00100000);
    i2c_master_restart();
    i2c_master_send(MCP23008<<1);
    i2c_master_send(IODIR);
    i2c_master_send(0b10000000);
    i2c_master_restart();
    i2c_master_send(MCP23008<<1);
    i2c_master_send(GPIO);
    i2c_master_restart();
    i2c_master_send(MCP23008<<1|1);
    i2cdata = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    
    CS = 1;
  
    
    while(1) {
        
    
        i2c_master_start();
        i2c_master_send(MCP23008<<1);
        i2c_master_send(GPIO);
        i2c_master_restart();
        i2c_master_send(MCP23008<<1|1);
        i2cdata = i2c_master_recv();
        i2c_master_ack(1); //ack no more read bytes
        if (i2cdata==0b10000000){
            i2c_master_restart();
            i2c_master_send(MCP23008<<1);
            i2c_master_send(OLAT);
            i2c_master_send(0b00000001);
        }
        else {
            i2c_master_restart();
            i2c_master_send(MCP23008<<1);
            i2c_master_send(OLAT);
            i2c_master_send(0b00000000);
        }
                
        i2c_master_stop();
////        
        
        
        
        x++;
        x = x%100;
        y++;
        y = y%200;

       //delay (6000); //total delay of 24000 per cycle (1000/s)
       PORTAINV = 0x0010;
       //PORTAbits.RA4 = 1;
//        delay(12000);
        if(!PORTBbits.RB4){
            counter = !counter;
        }
//        if (pressed){
//            counter = counter +5;
       
       
            CS = 0;
            channel = counter;
            //voltage = floor(100*sin((x*2*pi)/100)+100);
            voltage = i2cdata;
            //char voltage = 0b10101001;
            spi1_set(channel,voltage);
            delay(6000);
            CS = 1;
            delay(6000);
            
//            CS = 0;
//          
//            channel = !counter;
//            //voltage = 2*(m - abs((y)- m)); this is an upright triangle
//            voltage = y;
//            spi1_set(channel,voltage);
//            //spi1_set(0b1,0b10101001);
//            delay(6000);
//            
//            CS = 1;
//            delay(6000);
            
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

