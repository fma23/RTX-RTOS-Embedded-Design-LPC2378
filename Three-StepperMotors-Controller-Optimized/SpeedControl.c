/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:   SpeedControl.C
 *      Author: Farid Mabrouk
 *      Purpose: RTX example program
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2011 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <RTL.h>
#include <LPC23xx.H>                    /* LPC23xx definitions               */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* Motor1 Variables */
unsigned int M1SegmentA1; 
unsigned int M1SegmentB1;
unsigned int M1total_steps;
unsigned int M1MaxSpeed;                      /* in rad/sec     */
float        M1StepAngle;                     /* in rad         */
unsigned int M1Acceleration;
unsigned int M1Decceleration;

/* Motor2 Variables */
unsigned int M2SegmentA1; 
unsigned int M2SegmentB1;
unsigned int M2total_steps;
unsigned int M2MaxSpeed;                      /* in rad/sec     */
float        M2StepAngle;                     /* in rad         */
unsigned int M2Acceleration;
unsigned int M2Decceleration;

/*  Motor3 Variables */

unsigned int M3SegmentA1; 
unsigned int M3SegmentB1;
unsigned int M3total_steps;
unsigned int M3MaxSpeed;                      /* in rad/sec     */
float        M3StepAngle;                     /* in rad         */
unsigned int M3Acceleration;
unsigned int M3Decceleration;

unsigned int Frequency; 
/********************************************************************/

extern void serial_init (void);
extern char sendchar (char character);
extern void SendInteger (int integer);
extern void motors_parameters_init(void);
extern void motors_calculations(void);

extern void timer0_init(void);
extern void timer1_init(void);
extern void timer2_init(void);

OS_TID  t_SMotor1;                /* start task      */
OS_TID	t_SMotor2;                /* start task      */
OS_TID	t_SMotor3;                /* start task      */

/*----------------------------------------------------------------------------
 *        Optimized Function to calculate square root
 *---------------------------------------------------------------------------*/
float fastsqrt(float val) {
	
    long tmp = *(long *)&val;
    tmp -= 127L<<23;                  /* Remove IEEE bias from exponent (-2^23)          */
                                      /* tmp is now an appoximation to logbase2(val)     */
    tmp = tmp >> 1;                   /* divide by 2                                     */
    tmp += 127L<<23;                  /* restore the IEEE bias from the exponent (+2^23) */
    return *(float *)&tmp;
}

/*----------------------------------------------------------------------------
 *        Function to calculate C0 constant according the the used algorithm
 *---------------------------------------------------------------------------*/
float MCalculate_C0(float MStepAngle_par, float MAcceleration_par, unsigned int Frequency_par)
{
	  volatile float Motor_C0;
	
		/*C0 equation: C0=frequency*sqrt(2*motor_step_angle/angular_accel)*/
	  
  	Motor_C0=2*MStepAngle_par;
  	Motor_C0=Motor_C0/MAcceleration_par;
  	Motor_C0=fastsqrt(Motor_C0);
  	Motor_C0=Motor_C0*Frequency_par;
	 
	  return(Motor_C0);

}
/*----------------------------------------------------------------------------
 *        Function to calculate Cn constants according the the used algorithm
 *---------------------------------------------------------------------------*/
float MCalculate_CnUp(volatile float Cn_par, unsigned int Mstep_par ){
	
	   volatile float Mtemp0; 
	   volatile unsigned int  Mdenom; 
	
		/*Cn equation: Cn= (Cn-1)-(2*Cn-1/(4*step+1))*/	
	   Mdenom=(Mstep_par<<2)+1;
	   Mtemp0=(Cn_par+Cn_par)/Mdenom;
	   Mtemp0= Cn_par- Mtemp0;
	
	   return(Mtemp0);
	   
	}
/*----------------------------------------------------------------------------
 *        Function to calculate time dealy to the next step pluse 
 *---------------------------------------------------------------------------*/
unsigned int Calculate_PluseDelay(float Mtemp0_par){
		
	  volatile unsigned int delay;
	
	  /* normalization so that delays are obtained in Microseconds */ 
    Mtemp0_par=10*Mtemp0_par;	
	  delay=ceil(Mtemp0_par/12);
	
	  return(delay);                         /* Mdelay_constant=ceil(Mtemp2/12) */

	}
/*------------------------------------------------------------------------------------------------
*    Function to calculate Cn constants while speeding down according the the used algorithm
*-------------------------------------------------------------------------------------------------*/

float MCalculate_CnDwn(float Cn_par, unsigned int Mstep_par){	
	
		   volatile unsigned int Mdenom;
			 volatile float Mtemp0; 
	
		   /*Cn equation: Cn= (Cn-1)-(2*Cn-1/4*(step-total_steps)+1)*/	
   		 Mdenom= M1total_steps-Mstep_par;  
   		 Mdenom=(Mdenom<<2)-1;
   		 Mtemp0=(Cn_par+Cn_par)/Mdenom;
  		 Mtemp0=Cn_par+Mtemp0; 
  	
		   return(Mtemp0);	
	
	}
/*----------------------------------------------------------------------------
 *        Task SMotor1 
 *---------------------------------------------------------------------------*/
__task void SMotor1 (void) 
{
	volatile float Motor1_C0;
	volatile float Motor1_Cn;
	volatile unsigned int M1step=1;
	volatile unsigned int M1delay;
	
	tsk_lock();
	Motor1_C0=MCalculate_C0(M1StepAngle,M1Acceleration,Frequency);            /* Calculate C0      */
	Motor1_Cn=MCalculate_CnUp(Motor1_C0, M1step);                             /* Calculate Cn      */
	M1delay=Calculate_PluseDelay(Motor1_Cn);
	tsk_unlock();
	
	timer0_init();                                                             /* initialize timer0 */
	
	while(1)
   	{	  
			  T0TCR         = 1;                                                  /* Timer0 Enable     */
	      T0MR0         = M1delay;
			
			  M1step++;
			
				if (M1step <=M1SegmentA1)
				{	
        tsk_lock();	
				Motor1_Cn=MCalculate_CnUp(Motor1_Cn, M1step);                      /* Calculate Cn      */
			  M1delay  =Calculate_PluseDelay(Motor1_Cn);                         /* Calculate time delay to the next pulse */
				tsk_unlock();
				}
				else if (M1step>M1SegmentA1 && M1step <=M1SegmentB1)
				{
				/*constant speed*/
				}
				else if (M1step >M1SegmentB1)
				{		
				tsk_lock();
			  Motor1_Cn=MCalculate_CnDwn(Motor1_Cn, M1step);                     /* Calculate Cn      */
				M1delay=Calculate_PluseDelay(Motor1_Cn);                           /* Calculate time delay to the next pulse */        
				tsk_unlock();			
				}			
				
				if (M1step ==(M1total_steps-1))
				{
        T0MCR=0X00000004;                        /* stop on T0 */
				T0TCR         = 0;                       /* Timer0 disable */
				os_tsk_delete_self ();
				}
		
				os_evt_wait_and (0x0011,0xffff);	
     			
		}
		    
	}

/*----------------------------------------------------------------------------
 *        Task SMotor2
 *---------------------------------------------------------------------------*/
__task void SMotor2 (void) 
{
	
	volatile float Motor2_C0;
	volatile float Motor2_Cn;
	volatile unsigned int M2step=1;
	volatile unsigned int M2delay;
	
	tsk_lock();
	Motor2_C0=MCalculate_C0(M2StepAngle,M2Acceleration,Frequency);            /* Calculate C0      */
	Motor2_Cn=MCalculate_CnUp(Motor2_C0, M2step);                             /* Calculate Cn      */
	M2delay=Calculate_PluseDelay(Motor2_Cn);
	tsk_unlock();
	
	timer1_init();                                  /*initialize timer1 */
            
  while(1)
   	{	  
			  T1TCR         = 1;                             /* Timer1 Enable */
	      T1MR1         = M2delay;
			  
			  M2step++;
			
				if (M2step <=M2SegmentA1)
				{
				tsk_lock();
				Motor2_Cn=MCalculate_CnUp(Motor2_Cn, M2step);                      /* Calculate Cn      */
			  M2delay  =Calculate_PluseDelay(Motor2_Cn);                         /* Calculate time delay to the next pulse */	
				tsk_unlock();
				}
				else if (M2step>M2SegmentA1 && M2step <=M2SegmentB1)
				{
				/*constant speed*/
				}
				else if (M2step >M2SegmentB1)
				{
				tsk_lock();
				Motor2_Cn=MCalculate_CnDwn(Motor2_Cn, M2step);                     /* Calculate Cn      */
				M2delay=Calculate_PluseDelay(Motor2_Cn);                           /* Calculate time delay to the next pulse */
				tsk_unlock();
				}			
				
				if (M2step ==(M2total_steps-1))
				{
        T1MCR=0X00000004;                        /* stop on T1 */
				T1TCR         = 0;                       /* Timer0 disable */
				os_tsk_delete_self ();
				}

				os_evt_wait_and (0x0111,0xffff);			   
		}
		    
	}	
/*----------------------------------------------------------------------------
 *        Task SMotor3
 *---------------------------------------------------------------------------*/
__task void SMotor3 (void) 
{
	
	volatile float Motor3_C0;
	volatile float Motor3_Cn;
	volatile unsigned int M3step=1;
	volatile unsigned int M3delay;
	
	tsk_lock();
	Motor3_C0=MCalculate_C0(M1StepAngle,M1Acceleration,Frequency);            /* Calculate C0      */
	Motor3_Cn=MCalculate_CnUp(Motor3_C0, M3step);                             /* Calculate Cn      */
	M3delay=Calculate_PluseDelay(Motor3_Cn);
	tsk_unlock();
	
	timer2_init();                                        /*initialize timer1 */

  while(1)
   	{	  
			  T2TCR         = 1;                              /* Timer1 Enable */
	      T2MR2         = M3delay;
			  
			  M3step=M3step+1;
			
				if (M3step <=M3SegmentA1)
				{
				tsk_lock();
				Motor3_Cn=MCalculate_CnUp(Motor3_Cn, M3step);                      /* Calculate Cn      */
			  M3delay  =Calculate_PluseDelay(Motor3_Cn);                         /* Calculate time delay to the next pulse */	
				tsk_unlock();
				}
				else if (M3step>M3SegmentA1 && M3step <=M3SegmentB1)
				{
				/*constant speed*/
				}
				else if (M3step >M3SegmentB1)
				{
				tsk_lock();
				Motor3_Cn=MCalculate_CnDwn(Motor3_Cn, M3step);                     /* Calculate Cn      */
				M3delay=Calculate_PluseDelay(Motor3_Cn);                           /* Calculate time delay to the next pulse */
				tsk_unlock();
				}			
				
				if (M3step ==(M3total_steps-1))
				{
        T2MCR= 0x00000100;                       /* stop on T2 */
				os_tsk_delete_self ();
				}
				
				os_evt_wait_and (0x1111,0xffff);			   
		}
		    
	}	

/*----------------------------------------------------------------------------
 *        Task 1 'init': Initialize
 *---------------------------------------------------------------------------*/
__task void init (void) {

  FIO2DIR  = 0x000000FF;                                   /* P2.0..7 defined as Outputs       */
  FIO2MASK = 0x00000000;
  FIO2PIN  = 0x00000000;
  PINSEL10 = 0; 
	
	serial_init ();
	motors_parameters_init();
	motors_calculations();                                      /* defines speed profile */
	
  t_SMotor1= os_tsk_create(SMotor1,0);                        /* start task            */
  t_SMotor2= os_tsk_create(SMotor2,0);                        /* start task            */
	t_SMotor3= os_tsk_create(SMotor3,0);                        /* start task            */
	
  os_tsk_delete_self ();
}

/*----------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
 int main (void) {
	 
  U32 volatile start;
                                                        
  for (start = 0; start < 1000000; start++) { ; }   /* Wait for debugger connection 0.3s */
	
  os_sys_init (init);                               /* Initialize RTX and start init     */
	 
}

/*----------------------------------------------------------------------------
 * end of file
  *---------------------------------------------------------------------------*/


