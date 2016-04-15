#ifndef SPI_H__
#define SPI_H__
// Header file for i2c_master_noint.c
// helps implement use I2C1 as a master without using interrupts


void sp1_set(char channel, char voltage);
void spi1_start(void);
void spi1_write(void);
void spi1_stop(void);

#endif