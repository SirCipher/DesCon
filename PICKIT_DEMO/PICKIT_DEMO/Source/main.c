/*----------------------------------------------------------------------------
 * Name:    main.c
 * Author: 	Wil Bennett
 *----------------------------------------------------------------------------
 * This code demonstrates how to interface with some of the devices on
 * the PICkit Serial I2C Demo Board
 *----------------------------------------------------------------------------*/
 
#include <stdio.h>
//#include <math.h>
//#include "STM32F4xx.h"
#include "stm32f4_discovery.h"
#include "main.h"
#include "LCD.h"
#include "I2C.h"
#include "SENSORS.h"

//Global variables
volatile uint32_t msTicks = 0;          //	counts 1ms timeTicks
		
/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {

	//Local variables
	char line1[25];
	char line2[25];
	double	temperature;
	int	adcReading;
	
	//Startup functions, runs once +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
	
		SystemCoreClockUpdate();                      				// Update Core Clock Frequency 
		if (SysTick_Config(SystemCoreClock / 1000)) while (1);//Setup SysTick 1 msec interrupts
	
		LED_Init();		//Configure the GPIO ports for the LEDs
		LCD_Init();		//Initialise the LCD screen
		Init_I2C3();	//Initialise the STM32F4's internal I2C peripheral
		
		I2C_Temp_Init();	//Set up basic configuration of temp sensor
		I2C_GPIO_Init();	//Set up basic configuration of GPIO device
			
		Totally_Excessive_Startup_Routine();		
		
		Delay_Millis(500);
	//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
	
	while(1){	//Main execution loop, repeats forever-+-+-+-+-+-+-+-+-+-+-+-+
		
		adcReading	=	(int)I2C_ADC_Read();	//Fetch an ADC reading
		temperature	=	I2C_Temp_Read();	//Fetch a temperature reading
		
		//Display ADC reading on LEDs	(Resolution shifted from 12 to 8 bits to fit)
		I2C_GPIO_Write((uint8_t)(adcReading>>4));
		LED_Write((uint8_t)(adcReading>>4));
		
		//Write temperature to the LCD
		sprintf(line1,"  Temp: %.1f",temperature);
		LCD_GotoXY(0,0);
		LCD_PutS(line1);LCD_PutChar(0xB2);LCD_PutS("C    ");
		
		//Write ADC reading to the LCD
		sprintf(line2,"   ADC: %04u        ",adcReading);
		LCD_GotoXY(0,1);
		LCD_PutS(line2);

	}//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		
}//End of main function


/*----------------------------------------------------------------------------
  SysTick_Handler - called every 1ms
 ----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
	//Increment the millisecond counter
		msTicks++; 
}

/*----------------------------------------------------------------------------
  delays number of tick Systicks (happens every 1 ms)
 *----------------------------------------------------------------------------*/
void Delay_Millis (uint32_t dlyTicks) {                                              
  uint32_t curTicks;

  curTicks = msTicks;
   while ((msTicks - curTicks) < dlyTicks);
}

/*----------------------------------------------------------------------------
  Initialise LEDs
 ----------------------------------------------------------------------------*/
void	LED_Init(void){
	
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);// Enable the GPIO_LED Clock
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 	| GPIO_Pin_9 	| GPIO_Pin_10 | GPIO_Pin_11	|
																GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_SetBits(GPIOD,0x0000);
}

/*----------------------------------------------------------------------------
  Function maps an input byte onto the LEDs
 ----------------------------------------------------------------------------*/
void LED_Write(uint8_t inByte){
	uint16_t shiftByte	=	(uint16_t)inByte;
	shiftByte	=	shiftByte<<8;
	GPIO_SetBits(GPIOD,shiftByte&0xFF00);
	shiftByte	=	~shiftByte;	//Flip all bits
	GPIO_ResetBits(GPIOD,shiftByte&0xFF00);
}


/*----------------------------------------------------------------------------
  ...
 ----------------------------------------------------------------------------*/
void Totally_Excessive_Startup_Routine(void)	{
		uint16_t x	=	0;//Initialise X
		I2C_GPIO_Direction(0x00); //Set I2C GPIO to all outputs
		LCD_GotoXY(0,0);LCD_PutS("D&C PICkit Demo ");//LCD Message
		for(x=0xFF;x!=0xFE00;x=x<<1){LED_Write(x>>8);Delay_Millis(TESR_SPEED);}//Board LEDs UP
		for(x=0;x<=15;x++){LCD_GotoXY(x,1);LCD_PutChar(246);Delay_Millis(TESR_SPEED/2);}//LCD Chars UP
		for(x=0xFF;x!=0xFE00;x=x<<1){I2C_GPIO_Write(x>>8);Delay_Millis(TESR_SPEED);}//I2C LEDs UP
		for(x=0;x<=15;x++){LCD_GotoXY(x,1);LCD_PutChar(247);}//ARROW FLIP
		for(x=0x0FF0;x!=0x7;x=x>>1){I2C_GPIO_Write(x>>4);Delay_Millis(TESR_SPEED);}//I2C LEDs DOWN
		for(x=16;x>0;x--){LCD_GotoXY(x-1,1);LCD_PutChar(32);Delay_Millis(TESR_SPEED/2);}//LCD Chars DOWN
		for(x=0x0FF0;x!=0x7;x=x>>1){LED_Write(x>>4);Delay_Millis(TESR_SPEED);}//Board LEDs DOWN
}

