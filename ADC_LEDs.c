#include <stdio.h>
#include <stdlib.h>

#include "lcd_buffer.h"
#include "lcd_driver.h"
#include "LED.h"
#include "ADC.h"
#include "serial.h"
#include "utils.h"
#include "sig_gen.h"
#include "SWT.h"
#include "ADC.h"

#define VOLTAGE read_ADC1()
#define CURRENT read_ADC2()

void initialise_Peripherals(void);

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

void init_board(void){
  BTN_Init();  
	ADC1_init();
	lcd_init(LCD_LINES_TWO, LCD_CURSOR_OFF, LCD_CBLINK_OFF, 128);
	serial_init();
}

/*----------------------------------------------------------------------------
  Function shows current/voltage/resistance
 *----------------------------------------------------------------------------*/
void main_functionality(void){
	char* message = (char*)malloc(16*sizeof(char));
		unsigned int state = 1;

	while(1){
		if(SWT_Check(0)){
			state =1;
		}
		else if(SWT_Check(1)){
			state = 2;
		}
		else if(SWT_Check(2)){
			state = 3;
		}		
		if(state == 1){
			sprintf(message,"%d \"V\"", VOLTAGE);
			lcd_write_string(message, 1, 0);
			lcd_write_string("\"Voltage\"", 0, 0);
		
		}
		else if (state == 2)
		{
			sprintf(message,"%d \"A\"", CURRENT);
			lcd_write_string("\"Current\"",0,0);
			lcd_write_string(message, 1, 0);
		}
	  else
		{
		  sprintf(message,"%.4f \"Ohms\"", (float)((float)VOLTAGE/(float)CURRENT));
			lcd_write_string("\"Resistance\"",0,0);
			lcd_write_string(message, 1, 0);
		}			
	}
}


/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
	
  SystemCoreClockUpdate();                      /* Get Core Clock Frequency   */
  if (SysTick_Config(SystemCoreClock / 1000)) { /* SysTick 1 msec interrupts  */
    while (1);                                  /* Capture error              */
  }

	init_board();

	lcd_clear_display();
	lcd_write_string("You're a", 0, 0);
	lcd_write_string("Multimeter, Harry", 1, 0);
	printf("You're a multimeter, Harry");
	Delay(100);
	
	main_functionality();

}

