/*----------------------------------------------------------------------------
 * Name:    Blinky.c
 * Purpose: MOdification to Drive 2x16 LCD
 * Note(s): 
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2011 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/
 
 /* MODIFIED BY D. CHESMORE JANUARY 2013   */
 
#include <stdio.h>
#include "STM32F4xx.h"
#include "LED.h"
#include "SWT.h"
#include "LCD.h"



volatile uint32_t msTicks;                      /* counts 1ms timeTicks       */
/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
  msTicks++;
}

/*----------------------------------------------------------------------------
  delays number of tick Systicks (happens every 1 ms)
 *----------------------------------------------------------------------------*/
void Delay (uint32_t dlyTicks) {                                              
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}


/*----------------------------------------------------------------------------
  Function that initializes Button pins
 *----------------------------------------------------------------------------*/
void BTN_Init(void) {

  RCC->AHB1ENR  |= ((1UL <<  0) );              /* Enable GPIOA clock         */

  GPIOA->MODER    &= ~((3UL << 2*0)  );         /* PA.0 is input              */
  GPIOA->OSPEEDR  &= ~((3UL << 2*0)  );         /* PA.0 is 50MHz Fast Speed   */
  GPIOA->OSPEEDR  |=  ((2UL << 2*0)  ); 
  GPIOA->PUPDR    &= ~((3UL << 2*0)  );         /* PA.0 is no Pull up         */
}

/*----------------------------------------------------------------------------
  Function that read Button pins
 *----------------------------------------------------------------------------*/
 uint32_t BTN_Get(void) {

 return (GPIOA->IDR & (1UL<<0));
}


/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {

	
  /*uint32_t btns = 0;*/
  SystemCoreClockUpdate();                      /* Get Core Clock Frequency   */
  if (SysTick_Config(SystemCoreClock / 1000)) { /* SysTick 1 msec interrupts  */
    while (1);                                  /* Capture error              */
  }

  LED_Init();
  BTN_Init();   
  SWT_Init();	
  LCD_Initpins();	
	LCD_DriverOn();
	
	Delay(10);
	LCD_Init();

	LCD_DriverOn();
	LCD_On(1);
	Delay(20);
	LCD_Clear();
	LCD_GotoXY(0,0);
	LCD_PutS("Dig Multimeter");
	Delay(1000);
	LCD_Clear();
	LCD_GotoXY(4,1);
	LCD_PutS("Menu:");
	Delay(1000);
	LCD_Clear();
	LCD_GotoXY(0,0);
	LCD_PutS("R:S2 V:S3 I:S4");
	
/*	btns = SWT_Get();
	GPIOA->ODR = btns;
	Delay(50);
	if(btns){
		LCD_PutS("Key %d pressed\n");
		Delay(100);
	}
*/					
}

