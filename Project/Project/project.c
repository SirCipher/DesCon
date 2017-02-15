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
#include ".\stm32f4_all\stm32f4_all\stm32f4_usart.h"
#include "LED.h"
#include "SWT.h"
#include "LCD.h"
#include "ADC.h"
#include "math.h"
#include "serial.h"

#include "utility.h"

#define VOLTAGE read_ADC1()
#define CURRENT read_ADC2()


#include <string.h>


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
	ADC2_init();
	serial_init();
}


unsigned int GetWordLength(const char* str, int start){
	return 0;
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
	unsigned int state = 1;
	unsigned int count = 0;
	Delay(10);
	Board_init();
  
	
	LCD_DriverOn();
	LCD_On(1);
	Delay(20);
	LCD_Clear();
	LCD_PutSCenter("You're a",0);
	LCD_GotoXY(0,1);
	LCD_PutS("Multimeter Harry");
	while(!BTN_Get()){};
	while(BTN_Get()){};
	LCD_Clear();
	LCD_Clear();
	LCD_GotoXY(2,1);
	Delay(150);
	
		
	LCD_Clear();
	

	while(1){
		LCD_GotoXY(0,0);
		if(SWT_Check(0)){
			state =1;
			LCD_ClearRow1();
		}
		else if(SWT_Check(1)){
			state = 2;
			LCD_ClearRow1();
		}
		else if(SWT_Check(2)){
			state = 3;
			LCD_ClearRow1();
		}		
		if(state == 1){
			sprintf(message,"%d \"V\"",read_ADC1());
			LCD_PutSCenter("          ",1);
			LCD_PutSCenter(message,1);
			LCD_PutSCenter("\"Voltage\"",0);
		
		}
		else if (state == 2)
		{
			sprintf(message,"%d \"A\"",read_ADC2());
			LCD_PutSCenter("\"Current\"",0);
			LCD_PutSCenter("          ",1);
			LCD_PutSCenter(message,1);
		}
	  else
		{
		  sprintf(message,"%.4f \"Ohms\"", (float)((float)VOLTAGE/(float)CURRENT));
			LCD_PutSCenter("\"Resistance\"",0);
			LCD_ClearRow2();
			LCD_PutSCenter(message,1);
		}		

	}
		
}


void USART2_IRQHandler(void) {
    //Check if interrupt was because data is received
    if (USART_GetITStatus(USART2, USART_IT_RXNE)) {
        //Do your stuff here
	LCD_PutS("Tom is bae");        
        //Clear interrupt flag
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}

