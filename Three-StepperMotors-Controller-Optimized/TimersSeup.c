/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    TimersSetup.C
 *      Author:  Farid Mabrouk
 *      Purpose: RTX example program
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2011 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <RTL.h>
#include <LPC23xx.H>                    /* LPC23xx definitions               */
#include <stdio.h>
#include <stdlib.h>

extern void T0_IRQHandler (void) __irq;
extern void T1_IRQHandler (void) __irq;
extern void T2_IRQHandler (void) __irq;

/*----------------------------------------------------------------------------
 *        Timer0 Setup for MOTOR1
 *---------------------------------------------------------------------------*/
void timer0_init(void)
	{
	/* setup the timer interrupt */
  T0MCR         = 3;                             /* Interrupt and Reset on MR0  */
	VICVectAddr4  = (unsigned long ) T0_IRQHandler;/* Set Interrupt Vector        */
	VICVectCntl4  = 15;                            /* use it for Timer0 Interrupt */
	VICIntEnable  = (1  << 4);
	}
/*----------------------------------------------------------------------------
 *        Timer1 Setup for MOTOR2
 *---------------------------------------------------------------------------*/
void timer1_init(void)
	{
	/* setup the timer interrupt */
  T1MCR         = 0x18;                             /* Interrupt and Reset on MR1  */
	VICVectAddr5  = (unsigned long ) T1_IRQHandler;   /* Set Interrupt Vector        */
	VICVectCntl5  = 0x25;                             /* use it for Timer1 Interrupt */
	VICIntEnable  = (1  << 5);
	}
	
/*----------------------------------------------------------------------------
 *        Timer2 Setup for MOTOR3
 *---------------------------------------------------------------------------*/
void timer2_init(void)
	{
	/* setup the timer interrupt */
	T2MCR          = 0x000000C0;                          /* Interrupt and Reset on MR2   */
	VICVectAddr26  = (unsigned long)T2_IRQHandler;        /* Set Interrupt Vector         */
	VICVectCntl26  = 6;                                   /* use it for Timer2 Interrupt  */
	VICIntEnable   = (1  << 26);
	}
/*-------------------------------------------------------------------------------------*/