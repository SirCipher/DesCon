/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */
 #include "graphic_driver_ILI9320.h"

/**
------------------------------------------------------------
	GRAPHIC Public Functions
------------------------------------------------------------
*/
GRAPHIC_Result GRAPHICLCDDriver_Init(GRAPHIC_Options_t* LCD_Options) {
	/* Init pins used */
	ILI9320_InitPins();
	/* Send configuration sequence to ILI9320 LCD */
	ILI9320_InitSequence();
	
	/* Set options for library */
	LCD_Options->DefaultHeight = ILI9320_HEIGHT;
	LCD_Options->DefaultWidth = ILI9320_WIDTH;
	LCD_Options->PixelsCount = ILI9320_PIXELS;
	
	/* Fill with white color */
	GRAPHICLCDDriver_Fill(LCD_Options, GRAPHIC_COLOR_WHITE);
	
	/* Return OK */
	return GRAPHIC_OK;
}

GRAPHIC_Result GRAPHICLCDDriver_Rotate(GRAPHIC_Options_t* LCD_Options, GRAPHIC_Orientation_t orientation) {
	/* Return OK */
	return GRAPHIC_OK;
}

GRAPHIC_Result GRAPHICLCDDriver_Fill(GRAPHIC_Options_t* LCD_Options, uint32_t color) {
	volatile uint32_t index = 0;
	
	/* Set area to fill */
	ILI9320_WriteRegister(0x0050, 0x0000);	/* Top X */
	ILI9320_WriteRegister(0x0051, 0x00EF);	/* Bottom X */
	ILI9320_WriteRegister(0x0052, 0x0000);	/* Top Y */
	ILI9320_WriteRegister(0x0053, 0x013F);	/* Bottom Y */
	
	/* CS low */
	ILI9320_CS_LOW;
	/* GGRAM */
	ILI9320_WriteCommand(0x22);
	
	/* Fill area */
	for (index = 0; index < ILI9320_PIXELS; index++) {
		ILI9320_WriteData(color);
	}
	
	/* CS high */
	ILI9320_CS_HIGH;
	
	/* Return OK */
	return GRAPHIC_OK;
}

GRAPHIC_Result GRAPHICLCDDriver_DrawPixel(GRAPHIC_Options_t* LCD_Options, uint16_t x, uint16_t y, uint32_t color) {
	uint16_t tmp;
	if (LCD_Options->Orientation == GRAPHIC_Orientation_Portrait_1) {
		/* Default */
		ILI9320_WriteRegister(0x0020, x);
		ILI9320_WriteRegister(0x0021, y);
		ILI9320_WriteRegister(0x0050, x);
		ILI9320_WriteRegister(0x0052, y);
		ILI9320_WriteRegister(0x0051, x);
		ILI9320_WriteRegister(0x0053, y);
	} else if (LCD_Options->Orientation == GRAPHIC_Orientation_Portrait_2) {
		/* 180� Rotation */
		x = ILI9320_WIDTH - x - 1;
		y = ILI9320_HEIGHT - y - 1;
		ILI9320_WriteRegister(0x0020, x);
		ILI9320_WriteRegister(0x0021, y);
		ILI9320_WriteRegister(0x0050, x);
		ILI9320_WriteRegister(0x0052, y);
		ILI9320_WriteRegister(0x0051, x);
		ILI9320_WriteRegister(0x0053, y);
	} else if (LCD_Options->Orientation == GRAPHIC_Orientation_Landscape_1) {
		/* 90� Rotation */
		tmp = x;
		x = y;
		y = ILI9320_HEIGHT - tmp - 1;
		
		ILI9320_WriteRegister(0x0020, x);
		ILI9320_WriteRegister(0x0021, y);
		ILI9320_WriteRegister(0x0050, x);
		ILI9320_WriteRegister(0x0052, y);
		ILI9320_WriteRegister(0x0051, x);
		ILI9320_WriteRegister(0x0053, y);
	} else {
		/* 270� Rotation */
		tmp = x;
		x = ILI9320_WIDTH - y - 1;
		y = tmp;
		
		ILI9320_WriteRegister(0x0020, x);
		ILI9320_WriteRegister(0x0021, y);
		ILI9320_WriteRegister(0x0050, x);
		ILI9320_WriteRegister(0x0052, y);
		ILI9320_WriteRegister(0x0051, x);
		ILI9320_WriteRegister(0x0053, y);
	}
	
	/* Set pixel color */
	ILI9320_WriteRegister(0x0022, color);
	
	/* Return OK */
	return GRAPHIC_OK;
}

/**
------------------------------------------------------------
	Private functions
------------------------------------------------------------
*/
void ILI9320_InitPins(void) {
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/* Clock enable */
	RCC_AHB1PeriphClockCmd(	ILI9320_DATA_RCC |	/* Clock for DATA port */
							ILI9320_WR_RCC |	/* Clock for WR port */
							ILI9320_CS_RCC |	/* Clock for CS port */
							ILI9320_RS_RCC |	/* Clock for RS port */
							ILI9320_RD_RCC |	/* Clock for RD port */
							ILI9320_RST_RCC,	/* Clock for RST port */
	
							ENABLE);
	
	/* Common settings */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	
	/* Data pins */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_All;
	GPIO_Init(ILI9320_DATA_PORT, &GPIO_InitStruct);
	
	/* WR pin */
	GPIO_InitStruct.GPIO_Pin = ILI9320_WR_PIN;
	GPIO_Init(ILI9320_WR_PORT, &GPIO_InitStruct);
	
	/* CS pin */
	GPIO_InitStruct.GPIO_Pin = ILI9320_CS_PIN;
	GPIO_Init(ILI9320_CS_PORT, &GPIO_InitStruct);
	
	/* RS pin */
	GPIO_InitStruct.GPIO_Pin = ILI9320_RS_PIN;
	GPIO_Init(ILI9320_RS_PORT, &GPIO_InitStruct);
	
	/* RD pin */
	GPIO_InitStruct.GPIO_Pin = ILI9320_RD_PIN;
	GPIO_Init(ILI9320_RD_PORT, &GPIO_InitStruct);	
	
	/* RST pin */
	GPIO_InitStruct.GPIO_Pin = ILI9320_RST_PIN;
	GPIO_Init(ILI9320_RST_PORT, &GPIO_InitStruct);
	
	ILI9320_CS_HIGH;
	ILI9320_WR_HIGH;
}

void ILI9320_WriteCommand(uint16_t command) {
	/* Write command */
	ILI9320_RS_LOW;
	/* ILI9320 will read */
	ILI9320_RD_HIGH;
	/* Set command */
	ILI9320_DATA_PORT->ODR = command;
	/* Blink WR pin */
	ILI9320_WR_LOW;
	/* Small delay */
	ILI9320_Delay(1);
	/* WR back high */
	ILI9320_WR_HIGH;
}

void ILI9320_WriteData(uint16_t data) {
	/* Write data */
	ILI9320_RS_HIGH;
	/* Set command */
	ILI9320_DATA_PORT->ODR = data;
	/* Blink WR pin */
	ILI9320_WR_LOW;
	/* Small delay */
	ILI9320_Delay(1);
	/* WR back high */
	ILI9320_WR_HIGH;
}

void ILI9320_WriteRegister(uint16_t reg, uint16_t val) {
	/* Set CS low */  
	ILI9320_CS_LOW;
	/* Send register */
	ILI9320_WriteCommand(reg);         
	/* Send data */
	ILI9320_WriteData(val); 
	/* Set CS high */
	ILI9320_CS_HIGH; 
}

void ILI9320_InitSequence(void) {
	/* Reset ILI9320 */
  	ILI9320_RST_LOW;
	/* Small delay */
	ILI9320_Delay(1000);
	/* Set Reset high */
  	ILI9320_RST_HIGH;
	
	ILI9320_WriteRegister(0x00E5, 0x8000);        
	ILI9320_WriteRegister(0x0000, 0x0001);        
	ILI9320_WriteRegister(0x0001, 0x0100);
	ILI9320_WriteRegister(0x0002, 0x0700);
	ILI9320_WriteRegister(0x0003, 0x1030);
	ILI9320_WriteRegister(0x0004, 0x0000);
	ILI9320_WriteRegister(0x0008, 0x0202);        
	ILI9320_WriteRegister(0x0009, 0x0000);        
	ILI9320_WriteRegister(0x000A, 0x0000);
	ILI9320_WriteRegister(0x000C, 0x0000);        
	ILI9320_WriteRegister(0x000D, 0x0000);        
	ILI9320_WriteRegister(0x000F, 0x0000);              
	ILI9320_WriteRegister(0x0010, 0x0000);        
	ILI9320_WriteRegister(0x0011, 0x0007);        
	ILI9320_WriteRegister(0x0012, 0x0000);        
	ILI9320_WriteRegister(0x0013, 0x0000);        
	ILI9320_Delay(50000);
	ILI9320_WriteRegister(0x0010, 0x17B0);        
	ILI9320_WriteRegister(0x0011, 0x0007);        
	ILI9320_Delay(10000);
	ILI9320_WriteRegister(0x0012, 0x013A);        
	ILI9320_Delay(10000);
	ILI9320_WriteRegister(0x0013, 0x1A00);        
	ILI9320_WriteRegister(0x0029, 0x000c);                
	ILI9320_Delay(10000);      
	ILI9320_WriteRegister(0x0030, 0x0000);        
	ILI9320_WriteRegister(0x0031, 0x0505);        
	ILI9320_WriteRegister(0x0032, 0x0004);        
	ILI9320_WriteRegister(0x0035, 0x0006);        
	ILI9320_WriteRegister(0x0036, 0x0707);        
	ILI9320_WriteRegister(0x0037, 0x0105);        
	ILI9320_WriteRegister(0x0038, 0x0002);        
	ILI9320_WriteRegister(0x0039, 0x0707);        
	ILI9320_WriteRegister(0x003C, 0x0704);        
	ILI9320_WriteRegister(0x003D, 0x0807);         
	ILI9320_WriteRegister(0x0050, 0x0000);
	ILI9320_WriteRegister(0x0051, 0x00EF);
	ILI9320_WriteRegister(0x0052, 0x0000);
	ILI9320_WriteRegister(0x0053, 0x013F);
	ILI9320_WriteRegister(0x0060, 0x2700);
	ILI9320_WriteRegister(0x0061, 0x0001);
	ILI9320_WriteRegister(0x006A, 0x0000);
	ILI9320_WriteRegister(0x0021, 0x0000);        
	ILI9320_WriteRegister(0x0020, 0x0000);           
	ILI9320_WriteRegister(0x0080, 0x0000);        
	ILI9320_WriteRegister(0x0081, 0x0000);
	ILI9320_WriteRegister(0x0082, 0x0000);
	ILI9320_WriteRegister(0x0083, 0x0000);
	ILI9320_WriteRegister(0x0084, 0x0000);        
	ILI9320_WriteRegister(0x0085, 0x0000);
	ILI9320_WriteRegister(0x0090, 0x0010);        
	ILI9320_WriteRegister(0x0092, 0x0000);
	ILI9320_WriteRegister(0x0093, 0x0003);
	ILI9320_WriteRegister(0x0095, 0x0110);
	ILI9320_WriteRegister(0x0097, 0x0000);        
	ILI9320_WriteRegister(0x0098, 0x0000);      
	ILI9320_WriteRegister(0x0007, 0x0173);        
	ILI9320_Delay(50000);
}

void ILI9320_Delay(volatile uint32_t ticks) {
	while (ticks--) {
	
	}
}

