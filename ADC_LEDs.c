#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lcd_buffer.h"
#include "lcd_driver.h"
#include "LED.h"
#include "ADC.h"
#include "serial.h"
#include "utils.h"
#include "sig_gen.h"
#include "SWT.h"
#include "ADC.h"
#include "utility.h"

#define VOLTAGE (float) read_ADC1()
#define CURRENT (float) read_ADC2()
#define RESISTANCE VOLTAGE-CURRENT

void initialise_Peripherals(void);




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

/*----------------------------------------------------------------------------
  Initialize the components we require on the board
 *----------------------------------------------------------------------------*/
void init_board(void){
  BTN_Init();  
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
			return " Volts\n";
		case 2:
			return " Amps\n";
		case 3:
			return " Ohms\n";
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

/*----------------------------------------------------------------------------
  Function shows current/voltage/resistance
 *----------------------------------------------------------------------------*/
void main_functionality(void){
	char* message = (char*)malloc(16*sizeof(char));
	unsigned int state = 1;
	float value = 0;
	float oldValue = 0;
	char* unit = malloc(6*sizeof(char)); // Max word length + 1 for null char (possible words Volts, Amps, Ohms)
	int last_write_length1 = 0, last_write_length2 = 0;
	unit = " Volts\n";
	
	while(1){
		if(check_state_changed(&state)){
			unit = get_units(state);
		}
		oldValue = read_value(state);
		value = mapValue(oldValue, 0,65536,0,10);
		
		sprintf(message,"%.2f Old value is %.2f", value, oldValue);
		
//	printf("%.2f %c\n", value, unit[0]);
		lcd_write_string(message, 1, 0,&last_write_length1);
		
		strcat(message, unit);
				
		send_String(USART3, message);
		lcd_clear_display();
		lcd_write_string(unit, 0, 0, &last_write_length2);
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
