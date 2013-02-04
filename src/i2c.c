/*
* Copyright (C)
* Originally by: D. Dang, Texas Instruments Inc. December 2010
* Modified by: Alan Barr 2011
* Compiler: mspgcc
*/

#include "i2c.h"
#include <stdio.h>
#define printNum(num,format) printf("%s:%d  s" #num"=%"#format"\r\n",__FILE__,__LINE__,num);
static char i2cPolling = 0;                                             //Polling i2c 
static int i2cRXByteCtr = 0;
static const char * pI2cTxData;                                         // Pointer to TX data
static volatile char * pI2cRxData;                                      // Pointer to i2cRX data
static char i2cTXByteCtr = 0;
static char i2cRX = 0;

void i2cSetupPins(void)
{
    P1SEL |= BIT6 + BIT7;                                               // Assign I2C pins to USCI_B0
    P1SEL2|= BIT6 + BIT7;                                               // Assign I2C pins to USCI_B0
}

void i2cSetupTx(const char address)
{
    dint();
    i2cRX = 0;
    IE2 &= ~UCB0RXIE;  
    while (UCB0CTL1 & UCTXSTP);                                         // Ensure stop condition got sent// Disable i2cRX interrupt
    UCB0CTL1 |= UCSWRST;                                                // Enable SW reset
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;                               // I2C Master, synchronous mode
    UCB0CTL1 = UCSSEL_2 + UCSWRST;                                      // Use SMCLK, keep SW reset
    UCB0BR0 = 12;                                                       // fSCL = SMCLK/12 = ~100kHz
    UCB0BR1 = 0;
    UCB0I2CSA = address;                                                
    UCB0CTL1 &= ~UCSWRST;                                               // Clear SW reset, resume operation
    UCB0I2CIE |= UCNACKIE;
    IE2 |= UCB0TXIE;                                                    // Enable TX interrupt
}

void i2cSetupRx(const char address)
{
    dint();
    i2cRX = 1;
    IE2 &= ~UCB0TXIE;  
    UCB0CTL1 |= UCSWRST;                                                // Enable SW reset
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;                               // I2C Master, synchronous mode
    UCB0CTL1 = UCSSEL_2 + UCSWRST;                                      // Use SMCLK, keep SW reset
    UCB0BR0 = 12;                                                       // fSCL = SMCLK/12 = ~100kHz
    UCB0BR1 = 0;
    UCB0I2CSA = address;                                                
    UCB0CTL1 &= ~UCSWRST;                                               // Clear SW reset, resume operation
    UCB0I2CIE |= UCNACKIE;
    IE2 |= UCB0RXIE;                                                    // Enable i2cRX interrupt
}

void i2cTransmit(const char * data, const int numberOfBytes)
{
    pI2cTxData = data;                                                  // TX array start address
    i2cTXByteCtr = numberOfBytes;                                       // Load TX byte counter
    printf("got here%s: %d\r\n",__FILE__,__LINE__);
    while (UCB0CTL1 & UCTXSTP);                                         // Ensure stop condition got sent
    UCB0CTL1 |= UCTR + UCTXSTT;                                         // I2C TX, start condition
    printf("got here%s: %d\r\n",__FILE__,__LINE__);
    __bis_SR_register(CPUOFF + GIE);                                    // Enter LPM0 w/ interrupts
    printf("got here%s: %d\r\n",__FILE__,__LINE__);
}

void i2cReceive(volatile char * data, const int numberOfBytes)
{
    pI2cRxData = data;                                                  // Start of i2cRX bufferz
    i2cRXByteCtr = numberOfBytes;                                       // Load i2cRX byte counter
    printf("got here%s: %d\r\n",__FILE__,__LINE__);
    while (UCB0CTL1 & UCTXSTP);                                         // Ensure stop condition got sent
    UCB0CTL1 |= UCTXSTT;                                                // I2C start condition
    while (UCB0CTL1 & UCTXSTT);
    __bis_SR_register(CPUOFF + GIE);                                    // Enter LPM0 w/ interrupts
}

void i2cPoll(char address)
{   
    int ctr;
    for(ctr=0;ctr<2000;ctr++);
    /*
    do                          //i2cPolling for write complete
    {
        i2cPolling = 1;
        Setup_TX(address);
        i2cTransmit(0,1);       //DEBUG 
    }   
    while(i2cPolling == 2);
    i2cPolling = 0; 
    */
}

/* void i2cReadByte(char address,char* byteAddress, char *byte){ */
  
/*   /\* Move Temperature Sensor pointer to temperature register *\/ */
/*   unsigned char versionAddress=MPU6050_RA_WHO_AM_I; */
/*   unsigned char version=-1; */
/*   i2cSetupTx(MPU6050_DEFAULT_ADDRESS); */
/*   i2cTransmit(&versionAddress,1); */
/*   i2cPoll(MPU6050_DEFAULT_ADDRESS); */
/*   /\*Receive 1 Byte of temperature data*\/ */
/*   i2cSetupRx(MPU6050_DEFAULT_ADDRESS); */
/*   i2cReceive(&version, 1); */
/*   UCB0CTL1 |= UCSWRST; */
  
/*   printf("verion=%x\n",version); */
  
/* } */

interrupt(USCIAB0RX_VECTOR) USCIAB0RX_ISR(void)
{   
    if ((UCB0STAT & UCNACKIFG) == UCNACKIFG && i2cPolling == 1)
    {
        UCB0STAT &= ~UCNACKIFG;
        UCB0CTL1 |= UCTXSTP;
        while(UCB0CTL1 & UCTXSTP);
        i2cPolling = 2;
        __bic_SR_register_on_exit(CPUOFF); 
    
    }
    else
    {
        UCB0CTL1 |= UCTXSTP;
        while(UCB0CTL1 & UCTXSTP);
        while(1);
    }
}



interrupt(USCIAB0TX_VECTOR) USCIAB0TX_ISR(void)
{
 
    if(i2cRX == 1)                                                          // Master Recieve?
    {
        i2cRXByteCtr--;                                                     // Decrement i2cRX byte counter
        if (i2cRXByteCtr > 0)
        {
            *pI2cRxData++ = UCB0RXBUF;                                      // Move i2cRX data to address pI2cRxData
        }
        else if(i2cRXByteCtr == 0)
        {
                IE2 &= ~ UCB0TXIE;      //DEBUG disable interrupt
                IFG2 &= ~UCB0TXIFG; 
                UCB0CTL1 |= UCTXSTP;   
                *pI2cRxData++ = UCB0RXBUF;                                  // I2C stop condition

		while (UCB0STAT & UCBBUSY);

                UCB0CTL1 |= UCSWRST;                                        // Move final i2cRX data to pI2cRxData
                 __bic_SR_register_on_exit(CPUOFF);     
        }
    }
    else
    {                                                                       // Master Transmit
      printNum(i2cTXByteCtr,d);
        if (i2cTXByteCtr)                                                   // Check TX byte counter
        {
          UCB0TXBUF = *pI2cTxData++;                                        // Load TX buffer
          i2cTXByteCtr--;                                                   // Decrement TX byte counter
        }
	printNum(i2cTXByteCtr,d);
        if(i2cTXByteCtr)
        {
            UCB0CTL1 |= UCTXSTP;                                            // I2C stop condition
            IFG2 &= ~UCB0TXIFG; 
            __bic_SR_register_on_exit(CPUOFF);                              // Exit LPM0
        }
    }

}

    
