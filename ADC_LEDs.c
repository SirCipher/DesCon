#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "ADC.h"



#define VOLTAGE (float) read_ADC1()
#define CURRENT (float) read_ADC2()
#define RESISTANCE VOLTAGE/CURRENT


/*----------------------------------------------------------------------------
  Function that initializes Button pins
 *----------------------------------------------------------------------------*/
void init_GPIO(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12; // we want to configure all LED GPIO pins
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT; 		// we want the pins to be an output
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; 	// this sets the GPIO modules clock speed
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; 	// this sets the pin type to push / pull (as opposed to open drain)
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL; 	// this sets the pullup / pulldown resistors to be inactive
	GPIO_Init(GPIOD, &GPIO_InitStruct); 			// this finally passes all the values to the GPIO_Init function which takes care of setting the corresponding bits.
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;		  // we want to configure PA0
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN; 	  // we want it to be an input
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//this sets the GPIO modules clock speed
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;   // this sets the pin type to push / pull (as opposed to open drain)
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;   // this enables the pulldown resistor --> we want to detect a high level
	GPIO_Init(GPIOA, &GPIO_InitStruct);			  // this passes the configuration to the Init function which takes care of the low level stuff

	/* Tell system that you will use PD0 for EXTI_Line0 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
	
	/* PD0 is connected to EXTI_Line0 */
	EXTI_InitStruct.EXTI_Line = EXTI_Line0;
	/* Enable interrupt */
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	/* Interrupt mode */
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	/* Triggers on rising and falling edge */
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	/* Add to EXTI */
	EXTI_Init(&EXTI_InitStruct);

	/* Add IRQ vector to NVIC */
	/* PD0 is connected to EXTI_Line0, which has EXTI0_IRQn vector */
	NVIC_InitStruct.NVIC_IRQChannel = EXTI2_IRQn;
	/* Set priority */
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	/* Set sub priority */
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
	/* Enable interrupt */
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	/* Add to NVIC */
	NVIC_Init(&NVIC_InitStruct);
}

/* Set interrupt handlers */
/* Handle PD0 interrupt */
void EXTI0_IRQHandler(void) {
    /* Make sure that interrupt flag is set */
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
        /* Do your stuff when PD0 is changed */
        
					send_String(USART3, "FUCK YOU");

        
        /* Clear interrupt flag */
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

/*----------------------------------------------------------------------------
  Function that read Button pins
 *----------------------------------------------------------------------------*/
uint32_t BTN_Get(void) {

 return (GPIOA->IDR & (1UL << 0));
}

/*----------------------------------------------------------------------------
  Initialize the components we require on the board
 *----------------------------------------------------------------------------*/
void init_board(void){
  init_GPIO();  
	ADC1_init();
	ADC2_init();
	SWT_Init();
	lcd_init(LCD_LINES_TWO, LCD_CURSOR_OFF, LCD_CBLINK_OFF, 128);
	serial_init();
}


/*----------------------------------------------------------------------------
  Checks buttons and returns whether the state has changed or not
 *----------------------------------------------------------------------------*/
int check_state_changed(unsigned int *state){
	for(int i = 0; i < 3; i++){
		if(SWT_Check(i) && *state != i+1){
			*state = i+1;
			return 1;
		}
	}
	return 0;
}


/*----------------------------------------------------------------------------
  Returns the correct units based on state
 *----------------------------------------------------------------------------*/
char* get_units(int state){
	switch(state){
		case 1:
			return "Volts";
		case 2:
			return "Amps";
		case 3:
			return "Ohms";
	}
	return "\0";
}

/*----------------------------------------------------------------------------
  Returns the correct value dependant on state
 *----------------------------------------------------------------------------*/
float read_value(int state){
		switch(state){
			case 1:
				return VOLTAGE;
			case 2:
				return CURRENT;
			case 3:
				return RESISTANCE;
			default:
				return 0;
		}
}


float mapValue(float x, float in_min, float in_max, float out_min, float out_max){
	  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;	
}

void send_Char(USART_TypeDef* USARTx, uint8_t ch){
		while(!(USARTx->SR & USART_SR_TXE));
		USARTx->DR = ch;
}

void send_String(USART_TypeDef* USARTx, char * str){
	while(*str != 0){
		send_Char(USARTx, *str);
		str++;
	}
	send_Char(USARTx,'\n');
}

//char* constructString(const char* format, ...){
//	va_list args;
//	
//	va_start(args,format);
//	
//}

/*----------------------------------------------------------------------------
  Function shows current/voltage/resistance
 *----------------------------------------------------------------------------*/
void main_functionality(void){
	char* message = (char*)malloc(16*sizeof(char));
	char* message2 = (char*)malloc(100*sizeof(char));
	unsigned int state = 1;
	float value = 0;
	float rawValue = 0;
	char* unit = malloc(6*sizeof(char)); // Max word length + 1 for null char (possible words Volts, Amps, Ohms)
	int last_write_length1 = 0, last_write_length2 = 0;
	unit = "Volts";
	int c = 0;
	
	
	while(1){
		if(check_state_changed(&state)){ // We should only be changing units if the state has changed
			unit = get_units(state);
		}
		rawValue = read_value(state);
		value = mapValue(rawValue, 0,65536,0,10);

		sprintf(message,"%.2f", value);

//	printf("%.2f %c\n", value, unit[0]);
		lcd_write_string(message, 1, 0,&last_write_length1);
		
		
		sprintf(message2,"%s %s",message,unit);
		// strcat(message, unit);
				
		send_String(USART3, message2);
		lcd_write_string(unit, 0, 0, &last_write_length2);
	}
}



/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	int last_write_length1 = 0, last_write_length2 = 0;
  SystemCoreClockUpdate();                      /* Get Core Clock Frequency   */
  if (SysTick_Config(SystemCoreClock / 1000)) { /* SysTick 1 msec interrupts  */
    while (1);                                  /* Capture error              */
  }

	
	init_board(); //
	
	lcd_clear_display();
	lcd_write_string("You're a", 0, 0, &last_write_length1);
	lcd_write_string("Multimeter Harry", 1, 0, &last_write_length2);

	Delay(1000);
	lcd_clear_display();
	
	main_functionality();

}

