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
#include <stdlib.h>
#include "STM32F4xx.h"
#include "LED.h"
#include "SWT.h"
#include "LCD.h"
#include "ADC.h"
#include "math.h"

#include "utility.h"



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
 

void Board_init(){
	LED_Init();
  BTN_Init();   
  SWT_Init();	
  LCD_Initpins();	
	LCD_DriverOn();
	LCD_Init();	
	ADC1_init();
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

	char* message = (char*)malloc(16*sizeof(char));
	
	Delay(10);
	Board_init();
  
	
	LCD_DriverOn();
	LCD_On(1);
	Delay(20);
	LCD_Clear();
	LCD_GotoXY(0,0);
	LCD_PutS("Dank Multimeter");
	LCD_GotoXY(0,1);
	LCD_PutS("Press Truth Btn");
	while(!BTN_Get())
	{
		
	}	
	LCD_Clear();
	LCD_GotoXY(2,1);
	LCD_PutS("Bush did 911");
	Delay(150);
	
	
	
	LCD_Clear();
	

	while(1){
		LCD_GotoXY(0,0);
		sprintf(message,"%d can't melt",read_ADC1());
		
		LCD_GotoXY(0,0);
		LCD_PutS("                ");
		LCD_GotoXY(0,0);
		LCD_PutS(message);
		LCD_GotoXY(2,1);
		LCD_PutS("steel Memes");
		
	}
		
}

