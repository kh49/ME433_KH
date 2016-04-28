#include "xc.h"
#include "sys/attribs.h"

void spi1_set(unsigned char channel,unsigned char voltage){
//result = (result << 24) | num3; concat code
    channel = (channel<<3) | 0b111;
    channel = (unsigned int) channel;
    voltage = (unsigned int) voltage;
    unsigned int data = (channel<<12)|(voltage<<4);
    spi1_write(data);
    //spi1_write(0b0011101010010000);

}
void spi1_start(void){
    //PORTAbits.RA4 = 1;
   
    //all defaults are 0
    SPI1CON = 0;
    SPI1BUF;
    SPI1BRG = 0x4799; //baud rate to 10khz based on 6Mhz Pbclock at 299 for debug
    SPI1STATbits.SPIROV = 0;
    SPI1CONbits.CKE = 1;
    SPI1CONbits.MODE32 = 0;
    SPI1CONbits.MODE16 = 1; //set for 16bit transfer
    SPI1CONbits.MSTEN = 1;
    SPI1CONbits.ON = 1; //turn on SPI1
}

void spi1_write(unsigned int data){
    //PORTAbits.RA4 = 1; //for debugging
    SPI1STATbits.SPIROV = 0;
    
    SPI1BUF = data;
    
    while(SPI1STATbits.SPIBUSY){} //wait to receive byte in future
    SPI1BUF;
    //PORTAbits.RA4 = 0;
}
void spi1_stop(void){
 
    SPI1CON = 0;
}