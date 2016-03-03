	
/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:   TimersInterrupts.C
 *      Author: Farid Mabrouk
 *      Purpose: RTX example program
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2011 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <RTL.h>
#include <LPC23xx.H>                    /* LPC23xx definitions               */

	
extern	void LED_Init(void);
extern	void LED_On (unsigned int num);
extern	void LED_Off (unsigned int num);

extern OS_TID  t_SMotor1;                /* start task      */
extern OS_TID	t_SMotor2;                 /* start task      */
extern OS_TID	t_SMotor3;                 /* start task      */

/*----------------------------------------------------------------------------
 *        Timer0 Interrupt for MOTOR1
 *---------------------------------------------------------------------------*/
	void T0_IRQHandler (void) __irq  {
	
	int i; 
		
	LED_On (0x07);                                /*simulating a PULSE sent to the stepper motor DRIVER*/
	
	for(i=0; i<120; i++)                          /* PULSE is ON for 10 Microseconds */
	{ ; }
	
	LED_Off (0x07);
  
	isr_evt_set (0x0011, t_SMotor1);              /* send event signal to clock task  */
	
	T0IR          = 1;                            /* Clear interrupt flag       */
	VICVectAddr   = 0;                            /* Dummy write to signal end of interrupt */
}
	
/*----------------------------------------------------------------------------
 *        Timer1 Interrupt for MOTOR2
 *---------------------------------------------------------------------------*/
	void T1_IRQHandler (void) __irq  {
	
	int i; 
		
	LED_On (0x06);                               /*simulating a PULSE sent to the stepper motor DRIVER*/
	
	for(i=0; i<120; i++)                         /* PULSE is ON for 10 Microseconds */
	{ ; }
	
	LED_Off (0x06);
	
	isr_evt_set (0x0111, t_SMotor2);              /* send event signal to clock task  */
	
	T1IR          = 2;                           /* Clear interrupt flag       */
	VICVectAddr   = 0; 
}
/*----------------------------------------------------------------------------
 *        Timer2 Interrupt for MOTOR3
 *---------------------------------------------------------------------------*/

void T2_IRQHandler (void) __irq  {
	
	int i; 
	
	LED_On (0x05);                                  /* simulating a PULSE sent to the stepper motor DRIVER*/
	
	for(i=0; i<120; i++)                            /* PULSE is ON for 10 Microseconds */
	{  ; }
	
	LED_Off (0x05);
  
	isr_evt_set (0x1111, t_SMotor3);                /* send event signal to clock task  */

	
	T2IR          = 4;                              /* Clear interrupt flag             */
	VICVectAddr   = 0; 
}