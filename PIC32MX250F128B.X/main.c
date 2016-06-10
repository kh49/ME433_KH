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
#pragma config FPBDIV = DIV_1 //temporarilly set at 8 so PBUS is at 6Mhz for debugging
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
#define GYRO 0b1101011
#define CTRL1_XL 0x10
#define CTRL2_G 0x11
#define CTRL3_C 0x12
#define WHOAMI 0x0F
#define OUT_TEMP_L 0x20
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
    TRISB = 0b0001111001110011;
   i2c_master_setup();
    ANSELBbits.ANSB2 = 0; //SDA2 set to digital
    ANSELBbits.ANSB3 = 0; //SCL2 set to digital
 
    __builtin_enable_interrupts();
   // SYSTEMConfigPerformance(48000000);
    
    SPI1_init(); 
    LCD_init();
    LCD_clearScreen(0);
//    RPB13Rbits.RPB13R = 0b0011; //SDO
//    SDI1Rbits.SDI1R = 0b0000; //A1
    RPB7Rbits.RPB7R = 0b0101; //OC1
    RPB8Rbits.RPB8R = 0b0101; //OC2
    PORTAbits.RA4 = 1; //led init
    T2CONbits.TCKPS = 2; //timer 2 prescale = 1:4
    PR2 = 1999; //period = (PR2+1) * N * 12.5 ns = 100 us, 10 kHz
    TMR2 = 0;
    OC1RS = 1000;
    OC1R = 1000;
    OC2RS = 1000;
    OC2R = 1000;
    OC1CONbits.OCTSEL = 0; //select timer2
    OC2CONbits.OCTSEL = 0;
    OC1CONbits.OCM = 0b110; //set pwm mode
    OC2CONbits.OCM = 0b110;
    T2CONbits.ON = 1;
    OC1CONbits.ON = 1;
    OC2CONbits.ON = 1;
    
    //unsigned char x = 0; //sine counter
    //unsigned char y = 0; //triangle counter
    char pressed = 0; //for tracking button logic
    char counter = 0;
    char m = 100; //(triangle wave frequency is 1000/2m)
    unsigned char voltage = 0;
    unsigned char channel = 0;
    int bytes = 14;
    unsigned char i2cdata[bytes];
    unsigned char i2cdatatest;
    unsigned char i2cwhoami;
    short temp = 0;
    short accel_x = 0;
    short accel_y = 0;
    short accel_z = 0;
    unsigned short ax = 0;
    unsigned short ay = 0;
    unsigned short az = 0;
    char xg = 0;
    char yg = 0;
    char zg = 0;
    short gyro_x = 0;
    short gyro_y = 0;
    short gyro_z = 0;
    char i2cdatacount = 0;
    char textbuffer[20];
    char length = 0;
    char x = 10;
    char y = 24;
    
    i2cwhoami = i2c_master_read(GYRO,WHOAMI,0,0);
    i2c_master_write(GYRO,CTRL1_XL,0b10000000,0);
    i2c_master_write(GYRO,CTRL2_G,0b10000000,0);
    i2c_master_write(GYRO,CTRL3_C,0b00000100);
    //i2cdatatest = i2c_master_read(GYRO,CTRL1_XL,0,0);
//    i2c_master_write(GYRO,CTRL1_XL,0b10000000,1);
//    i2c_master_send(0b10000000);
//    i2c_master_send(0b00000100);
//    i2c_master_stop();
  
    
   
    CS = 1;
    
     //LCD_clearScreen(0);
     LCD_drawla(0,0,0b1111100000000000,0);
     
    while(1) {
       //LCD_char(28,32,30,0b1111100000000000);
        
        
       i2c_master_multiread(GYRO,OUT_TEMP_L,bytes,i2cdata);
//       //i2cdatatest = i2c_master_read(GYRO,0x28,0,0);
       temp = i2cdata[1];
       temp = (temp<<8)|i2cdata[0];
       temp = (unsigned short)temp;
       sprintf(textbuffer,"T:%d     ",temp);
       LCD_type(x,y,textbuffer,0b1111100000000000);
       y = y+8;
               
       gyro_x = i2cdata[3];
       gyro_x = (gyro_x<<8)|i2cdata[2];
       gyro_x = (unsigned short)gyro_x;
       sprintf(textbuffer,"Gx:%d     ",gyro_x);
       LCD_type(x,y,textbuffer,0b1111100000000000);
       y = y+8;
       
       gyro_y = i2cdata[5];
       gyro_y = (gyro_y<<8)|i2cdata[4];
       gyro_y = (unsigned short)gyro_y;
       sprintf(textbuffer,"Gy:%d     ",gyro_y);
       LCD_type(x,y,textbuffer,0b1111100000000000);
       y = y+8;
       
       gyro_z = i2cdata[7];
       gyro_z = (gyro_z<<8)|i2cdata[6];
       gyro_z = (unsigned short)gyro_z;
       sprintf(textbuffer,"Gz:%d     ",gyro_z);
       LCD_type(x,y,textbuffer,0b1111100000000000);
       y = y+8;
       
      
       accel_x = (i2cdata[9]<<8)|i2cdata[8];
       ax = accel_x + 32768;
       sprintf(textbuffer,"Ax:%u     ",ax);
       LCD_type(x,y,textbuffer,0b1111100000000000);
       y = y+8;
       
       
       accel_y = (i2cdata[11]<<8)|i2cdata[10];
       ay = accel_y + 32768;
       sprintf(textbuffer,"Ay:%u     ",ay);
       LCD_type(x,y,textbuffer,0b1111100000000000);
       y = y+8;
       
      
       accel_z = (i2cdata[13]<<8)|i2cdata[12];
       az = accel_z+32768;
       sprintf(textbuffer,"Az:%u     ",az);
       LCD_type(x,y,textbuffer,0b1111100000000000);
       y = 24;
        
       OC1RS = floor((ax/16.768));
       OC2RS = floor((ay/16.768));
       if(OC1RS>2999){
           OC1RS = 2000;
            }
       else if(OC1RS<1000){
           OC1RS = 0;
       }
       else{
           OC1RS = OC1RS - 1000;
       }
       
       
       if(OC2RS>2999){
           OC2RS = 2000;
            }
       else if(OC2RS<1000){
           OC2RS = 0;
       }
       else{
           OC2RS = OC2RS - 1000;
       }
        
       
    
//       OC1R = floor((gyro_x/3.2768 + 10000));
//       OC2R = floor((gyro_y/3.2768 + 10000));
//-------------SPI debugging for IMU------------------
       if (!PORTBbits.RB4){
           pressed = !pressed;
       }
//       CS = 0;
//            channel = counter;
//            //voltage = floor(100*sin((x*2*pi)/100)+100);
//            voltage = i2cwhoami;
//            //char voltage = 0b10101001;
//            spi1_set(channel,voltage);
//            //delay(6000);
//            CS = 1;
//       }
//       else {
//            CS = 0;
//            channel = counter;
//            //voltage = floor(100*sin((x*2*pi)/100)+100);
//            voltage = i2cdata[0];
//            //char voltage = 0b10101001;
//            spi1_set(channel,voltage);
//            //delay(6000);
//            CS = 1;
//            //++i2cdatacount %14;
//       }
//-------------------------------------------------------
       delay(960000);
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

