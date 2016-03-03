/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:   PulseControl.C
 *      Author: Farid Mabrouk
 *      Purpose: RTX example program
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2011 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <RTL.h>
#include <LPC23xx.H>                    /* LPC23xx definitions               */


/* Function that initializes LEDs                                             */
void LED_Init(void) {
	
      PINSEL10 = 0;                         /* Disable ETM interface, enable LEDs */
      FIO2DIR  = 0x000000FF;                /* P2.0..7 defined as Outputs         */
      FIO2MASK = 0x00000000;
}
/* Function that turns on requested LED                                       */
void LED_On (unsigned int num) {
  FIO2SET = (1 << num);
}

/* Function that turns off requested LED                                      */
void LED_Off (unsigned int num) {
  FIO2CLR = (1 << num);
}