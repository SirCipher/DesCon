/*----------------------------------------------------------------------------
 * Name:    ADC_LEDs.c

reads ADC channel and displays upper 8 bits (of 12) on LEDs*/


#include <stdio.h>
#include "STM32F4xx.h"
#include "LED.h"



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

 return (GPIOA->IDR & (1UL << 0));
}

/* Function to intiialise ADC1    */

void ADC1_init(void) {
	
	RCC->APB2ENR  |= ((1UL <<  8) );         /* Enable ADC1 clock                */
	RCC->AHB1ENR  |= ((1UL <<  2) );         /* Enable GPIOC clock                */
	GPIOC->MODER = 0xffffffff;
	GPIOC->PUPDR = 0;
	ADC1->CR1 = 0x00;
	ADC1->CR1 |= (1UL << 11);
	ADC1->CR2 = 0x00;
	ADC1->CR2 |= (1UL << 10) ;					/* right alignement of 12 bits */
	ADC->CCR = 0x00;
	ADC1->SQR1 = 0x01;								/* 1 conversion at a time */
	ADC1->SMPR1 = 0x00;
	ADC1->SMPR1 = 0x0300;
	ADC1->SQR1 = 0x01;
	ADC1->SQR3 = 0x0e;								/* ADC_IN14 = 0x0e: ADC_IN15 = 0x0f */
	ADC1->CR2 |= (1UL << 0);
	
}
	
/* function to read ADC and retun value */
unsigned int read_ADC1 (void) {
	
	ADC1->CR2 |= (1UL << 30)	;		/* set SWSTART to 1 to start conversion */
	Delay(100);
	return (ADC1->DR);
}
/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
 
 uint32_t value = 0;

  SystemCoreClockUpdate();                      /* Get Core Clock Frequency   */
  if (SysTick_Config(SystemCoreClock / 1000)) { /* SysTick 1 msec interrupts  */
    while (1);                                  /* Capture error              */
  }

  LED_Init();
  BTN_Init();  
	ADC1_init();
 
  while(1) {                                    /* Loop forever               */
 
		value = read_ADC1();
		value = (value *16);
		GPIOD->ODR = value;
		Delay(100);

  }
  
}

