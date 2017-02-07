/*----------------------------------------------------------------------------
 * Name:    LCD.c
 * Author: 	Jon Emery & Dave Hunter [Modified by Wil Bennett 2014]
 *----------------------------------------------------------------------------
 * Provides high-level functions for all aspects of interfacing with the LCD
 *----------------------------------------------------------------------------*/
 
#include "STM32F4xx.h"
//#include "stm32f4_discovery.h"
#include "main.h"
#include "LCD.h"

//These declarations represent the pin number in the respective port (port is not noted here)
const unsigned long lcd_mask[] = {1UL << 0, 1UL << 1, 1UL << 2, 1UL << 3, 1UL << 4, 1UL << 5, 1UL << 6, 1UL << 7};
const unsigned long lcd_RS    = 1UL << 0;
const unsigned long lcd_RdnWr = 1UL << 1;
const unsigned long lcd_E     = 1UL << 2;

/*----------------------------------------------------------------------------
  initialize LCD Pins
 *----------------------------------------------------------------------------*/
void LCD_GPIO_Config (void) {

	/* Pin list for LCD ----------------------------------------------------
	PORT A
		LCD.DRIVERIC						PA15		//OUTPUT
	PORT B
		LCD.RS									PB0			//OUTPUT
		LCD.RW									PB1			//OUTPUT
		LCD.E										PB2			//OUTPUT
	PORT D
		LCD.D0									PD0			//OUTPUT
		LCD.D1									PD1			//OUTPUT
		LCD.D2									PD2			//OUTPUT
		LCD.D3									PD3			//OUTPUT
		LCD.D4									PD4			//OUTPUT
		LCD.D5									PD5			//OUTPUT
		LCD.D6									PD6			//OUTPUT
		LCD.D7									PD7			//OUTPUT
	----------------------------------------------------------------------*/
	
	//Create GPIO structures
	GPIO_InitTypeDef 		GPIO_InitStructurePortA;
	GPIO_InitTypeDef 		GPIO_InitStructurePortB;
	GPIO_InitTypeDef 		GPIO_InitStructurePortD;
	
  //Enable GPIO clocks
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	//PORT B OUTPUT SETUP
	GPIO_InitStructurePortB.GPIO_Pin = 	GPIO_Pin_0	|	GPIO_Pin_1	|	GPIO_Pin_2;
	GPIO_InitStructurePortB.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructurePortB.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructurePortB.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructurePortB.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructurePortB);
	
	//PORT D OUTPUT SETUP
	GPIO_InitStructurePortD.GPIO_Pin = 	GPIO_Pin_0 	| GPIO_Pin_1	|	GPIO_Pin_2		|
																			GPIO_Pin_3 	| GPIO_Pin_4	|	GPIO_Pin_5 		|
																			GPIO_Pin_6 	|	GPIO_Pin_7;
	GPIO_InitStructurePortD.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructurePortD.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructurePortD.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructurePortD.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructurePortD);

  // PORT A LCD Driver IC Enable Line
  GPIO_InitStructurePortA.GPIO_Pin = 	GPIO_Pin_15;
	GPIO_InitStructurePortA.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructurePortA.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructurePortA.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructurePortA.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructurePortA);
	
	return;
}

/*----------------------------------------------------------------------------
  Initialize LCD MCU, exactly as specified in datasheet
 *----------------------------------------------------------------------------*/
void LCD_Init (void) {
	
	//Set up the GPIO pins
	LCD_GPIO_Config();	
	
	//Make sure everything's off
	LCD_DataClear();
	LCD_Eoff();
	LCD_RSoff();
	LCD_RWoff();
	
	//Enable the driver for the three control lines
	LCD_DriverOn();
	
	Delay_Millis(20);
	LCD_Write( 0x30 );//B0011XXXX
	Delay_Millis(10);
	LCD_Write( 0x30 );//B0011XXXX
	Delay_Millis(1);
	LCD_Write( 0x30 );//B0011XXXX
	
	
	LCD_Write( 0x38 );//Function set - 8 bit mode, 2 lines, 5x8 font
	LCD_Write( 0x08 );//Display OFF
	LCD_Write( 0x01 );//Display CLEAR
	LCD_Write( 0x06 );//Shift cursor RIGHT, do not shift entire display
	
	Delay_Millis(5);
	
	LCD_On();
}


/*----------------------------------------------------------------------------
  Function that turns on LCD E Signal
 *----------------------------------------------------------------------------*/
void LCD_Eon (void) {

    GPIO_SetBits(GPIOB,lcd_E);
}

/*----------------------------------------------------------------------------
  Function that turns off LCD E Signal
 *----------------------------------------------------------------------------*/
void LCD_Eoff (void) {

	GPIO_ResetBits(GPIOB,lcd_E);
}


/*----------------------------------------------------------------------------
  Function that turns on LCD RS Signal
 *----------------------------------------------------------------------------*/
void LCD_RSon (void) {

    GPIO_SetBits(GPIOB,lcd_RS);
}

/*----------------------------------------------------------------------------
  Function that turns off LCD RS Signal
 *----------------------------------------------------------------------------*/
void LCD_RSoff (void) {

    GPIO_ResetBits(GPIOB,lcd_RS);
}

/*----------------------------------------------------------------------------
  Function that turns on LCD RW Signal   Read notWrite
 *----------------------------------------------------------------------------*/
void LCD_RWon (void) {

    GPIO_SetBits(GPIOB,lcd_RdnWr);
}

/*----------------------------------------------------------------------------
  Function that turns off LCD RW Signal   Read notWrite
 *----------------------------------------------------------------------------*/
void LCD_RWoff (void) {

    GPIO_ResetBits(GPIOB,lcd_RdnWr);
}


/*----------------------------------------------------------------------------
  Function that outputs value to LCD Data Signals
 *----------------------------------------------------------------------------*/
void LCD_DataWrite(unsigned int value) {
	
	LCD_RWoff();	//Ensures it's always in WRITE mode
	GPIO_SetBits(GPIOD,(uint8_t)value);// Set Data Bits
}
/*----------------------------------------------------------------------------
  Function that clears value on LCD Data Signals
 *----------------------------------------------------------------------------*/
void LCD_DataClear() {
  
	// Clear Data Bits
	GPIO_ResetBits(GPIOD,0xFF);
}

/*----------------------------------------------------------------------------
  Function that turns on requested LCD
 *----------------------------------------------------------------------------*/
void LCD_On (void) {
	
	LCD_RSoff();
	LCD_RWoff();
  LCD_Write( 0x0C );//Display ON
  
}

/*----------------------------------------------------------------------------
  Function that turns off requested LCD
 *----------------------------------------------------------------------------*/
void LCD_Off (void) {
	
	LCD_RSoff();
	LCD_RWoff();
	LCD_Write( 0x08 );//Display OFF
  
}

/*----------------------------------------------------------------------------
  Function that Enables LCD driver
 *----------------------------------------------------------------------------*/
void LCD_DriverOn (void) {

    GPIO_SetBits(GPIOA,1UL << 15);
}

/*----------------------------------------------------------------------------
  Function that disables LCD driver
 *----------------------------------------------------------------------------*/
void LCD_DriverOff (void) {

    GPIO_ResetBits(GPIOA,1UL << 15);
}


/*----------------------------------------------------------------------------
  Function that turns on requested LCD
 *----------------------------------------------------------------------------*/
void LCD_Clk (void) {

  LCD_Eon();
	Delay_Millis(1);
	LCD_Eoff();
}


/*----------------------------------------------------------------------------
  Function that Writes a byte to the LCD - 8 bit mode
 *----------------------------------------------------------------------------*/
void LCD_Write (unsigned int value) {
	
  LCD_DataClear();
	LCD_DataWrite(value);
	LCD_Clk();
	Delay_Millis(1);
}

/*----------------------------------------------------------------------------
	Function that turns the display on, sets cursor on and blinking 
 *----------------------------------------------------------------------------*/
void LCD_cursor_on (void) {
	
	LCD_RSoff();
	LCD_Write(0x03);
	Delay_Millis(10);
}

/*----------------------------------------------------------------------------
  Function that Clears the LCD Screen
 *----------------------------------------------------------------------------*/
void LCD_Clear (void) {

  LCD_RSoff();
	LCD_RWoff();
	LCD_Write(0x01);
	Delay_Millis(10);
}


/*----------------------------------------------------------------------------
  Function that Writes a Character to the LCD
 *----------------------------------------------------------------------------*/
void LCD_PutChar (unsigned int c) {

  LCD_RSon();
	LCD_Write(c);
	Delay_Millis(1);
}


/*----------------------------------------------------------------------------
  Function that Writes a string to the LCD
 *----------------------------------------------------------------------------*/
void LCD_PutS (const char * s) {
	int i = 0;
  LCD_RSon();
	while(*s&&i<=15){
		LCD_Write(*s++);
		i++;
	}
	Delay_Millis(1);
}


/*----------------------------------------------------------------------------
  Function that Sets the Cursor Postion on the LCD
 *----------------------------------------------------------------------------*/
void LCD_GotoXY (unsigned int x, unsigned int y) {

	LCD_RWoff();
  LCD_RSoff();
	// x is 0 to 15
	// y is 0 or 1 for line
	if( y == 0 )
		LCD_Write( 0x80 | (x & 0x3F));
	else
		if( y == 1 )
			LCD_Write( 0xC0 | (x & 0x3F));

	Delay_Millis(1);
}

