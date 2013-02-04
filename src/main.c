/*
 * This file is part of the MSP430 hardware UART example.
 *
 * Copyright (C) 2012 Stefan Wendler <sw@kaltpost.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/******************************************************************************
 * Hardware UART example for MSP430.
 *
 * Stefan Wendler
 * sw@kaltpost.de
 * http://gpio.kaltpost.de
 *
 * Echos back each character received. Blinks green LED in main loop. Toggles
 * red LED on RX.
 *
 * Use /dev/ttyACM0 at 9600 Bauds (and 8,N,1).
 *
 * Note: RX-TX must be swaped when using the MSPg2553 with the Launchpad! 
 *       You could easliy do so by crossing RX/TX on the jumpers of the 
 *       Launchpad.
 ******************************************************************************/

#include <msp430.h>
#include <stdio.h>
#include "uart.h"
#include "i2c.h"
//#include "mpu6050.h"

#define printNum(num,format) printf("%s:%d  s" #num"=%"#format"\r\n",__FILE__,__LINE__,num);
int putchar(int c){
  uart_putc((char)c);
  return 0;
}
void uart_rx_isr(unsigned char c) {
	uart_putc(c);
	P1OUT ^= BIT0;		// toggle P1.0 (red led)
}
 
/**
 * Main routine
 */
int main(void)
{
    WDTCTL  = WDTPW + WDTHOLD; 	// Stop WDT
	BCSCTL1 = CALBC1_1MHZ;      // Set DCO
  	DCOCTL  = CALDCO_1MHZ;
 
	P1DIR  = BIT0 + BIT6; 		// P1.0 and P1.6 are the red+green LEDs	
	P1OUT  = BIT0 + BIT6; 		// All LEDs off

    uart_init();

	// register ISR called when data was received
    uart_set_rx_isr_ptr(uart_rx_isr);

    __bis_SR_register(GIE);

    uart_puts((char *)"\n\r***************\n\r");
    uart_puts((char *)"MSP430 harduart\n\r");
    uart_puts((char *)"***************\n\r\n\r");

    uart_puts((char *)"Starting example\r\n ");


    /************/
    /* i2c test */
    /************/

    /* i2cSetupPins(); */
        
    /* char versionAddress=MPU6050_RA_WHO_AM_I; */
    /* char version=-1; */
    /* i2cSetupTx(MPU6050_DEFAULT_ADDRESS); */
    /* printNum(0,d); */
    /* i2cTransmit(&versionAddress,1); */
    /* printNum(0,d); */
    /* i2cPoll(MPU6050_DEFAULT_ADDRESS); */
    /* /\*Receive 1 Byte of temperature data*\/ */
    /* i2cSetupRx(MPU6050_DEFAULT_ADDRESS); */
    /* i2cReceive(&version, 1); */
    /* UCB0CTL1 |= UCSWRST; */
	
    /* 	printf("verion=%x\n",version); */
	
    volatile unsigned long i;

    while(1) {

	   P1OUT ^= BIT6; 			// Toggle P1.6 output (green LED) using exclusive-OR
	   i = 50000;             	// Delay

	   do (i--);				// busy waiting (bad)
	   while (i != 0);
    } 
}

