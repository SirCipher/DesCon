#include "ADC.h"

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

void ADC2_init(void){
	RCC->APB2ENR  |= ((2UL <<  8) );         /* Enable ADC1 clock                */
	RCC->AHB1ENR  |= ((1UL <<  2) );  
	ADC2->CR1 |= (1UL << 11); // Discontinuous Mode on Injected channels
	ADC2->CR2 = 0x00; // reset control register 2
	ADC2->CR2 |= (1UL << 10) ; // discontinuous mode on regular channels
	ADC2->SQR1 = 0x01;								/* 1 conversion at a time */
	ADC2->SMPR1 = 0x0300; // R/W on SMP 13 56 cycles
	ADC2->SQR3 = 0x0f;								/* ADC_IN14 = 0x0e: ADC_IN15 = 0x0f */
	ADC2->CR2 |= (1UL << 0);
}
	
/* function to read ADC and retun value */
unsigned int read_ADC1 (void) {
	ADC1->CR2 |= (1UL << 30)	;		/* set SWSTART to 1 to start conversion */
	Delay(100);
	
	return (ADC1->DR << 4);
}

unsigned int read_ADC2 (void) {
	
	ADC2->CR2 |= (1UL << 30)	;		/* set SWSTART to 1 to start conversion */
	Delay(100);
	return (ADC2->DR);
}

void set_cont_ADC1(void){
	ADC1->CR2 |= (1UL << 30)	;		/* set SWSTART to 1 to start conversion */
	//Can wire to button to enable cont
	ADC1->CR2 |= (1UL << 1)	;		
}

/* function to set ADC to cont */
unsigned int read_cont_ADC1 (void) {
	//TODO: Inside spinlock, increment i to MAX_RETRY_ADC_CONT. Throw exception.
	while(!((ADC1->CR2 & (1 << 10))>0));
	return ADC1->DR;
}
