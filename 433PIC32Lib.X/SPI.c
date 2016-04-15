#include "xc.h"
#include "sys/attribs.h"

void spi1_set(char channel,char voltage){
//result = (result << 24) | num3; concat code
char configbits = 0b111;
char pad = 0b0000;
configbits = (channel<<1)|configbits;
configbits = (configbits<<4)|voltage;
configbits = (configbits<<12)|pad;

spi1_write(0b1111111111110000);

}
void spi1_start(void){
    //PORTAbits.RA4 = 1;
   
    //all defaults are 0
    SPI1CON = 0;
    SPI1BUF;
    SPI1BRG = 0x300; //baud rate to 10khz based on 6Mhz Pbclock
    SPI1STATbits.SPIROV = 0;
    SPI1CONbits.CKE = 1;
    SPI1CONbits.MODE32 = 0;
    SPI1CONbits.MODE16 = 1; //set for 16bit transfer
    SPI1CONbits.MSTEN = 1;
    SPI1CONbits.ON = 1; //turn on SPI1
}

void spi1_write(unsigned char data){
    PORTAbits.RA4 = 1;
    
    SPI1BUF = data;
    
    while(!SPI1STATbits.SPIRBF){} //wait to receive byte in future
    
    PORTAbits.RA4 = 0;
}
void spi1_stop(void){
 
    SPI1CON = 0;
}