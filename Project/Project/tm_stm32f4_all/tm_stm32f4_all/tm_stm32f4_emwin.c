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
#include "stm32f4_emwin.h"

/* Private variables */
static STMPE811_TouchData TS_Data;
static GUI_PID_STATE TS_State;
static __IO uint8_t MemoryEnabled = 0;
__IO uint32_t EMWIN_LCD_DRIVER_CB_CALLED = 0;

/* External ISR handlers */
extern void LTDC_ISR_Handler(void);
extern void DMA2D_ISR_Handler(void);

EMWIN_Result_t EMWIN_Init(void) {
	/* Initialize CRC for emWin */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
	
	/* Initialize delay */
	DELAY_Init();

	/* Initialize LCD */
	ILI9341_Init();
	
	/* Initialize touch screen */
	if (STMPE811_Init() != STMPE811_State_Ok) {
		/* Return touch error */
		return EMWIN_Result_TouchError;
	}
	
	/* Initialize GUI */
	if (GUI_Init() != 0) {
		/* Return GUI error */
		return EMWIN_Result_GUIError;
	}
	
	/* Set LCD default orientation */
	TS_Data.orientation = STMPE811_Orientation_Portrait_2;
	
	/* Return OK */
	return EMWIN_Result_Ok;
}

void EMWIN_MemoryEnable(void) {
	/* Set layer 2 as view layer for LCD */
	/* EMWIN works on layer 1 and data will be transfered from layer 1 to layer 2 */
	/* after EMWIN finishes drawings using @ref EMWIN_Exec() function */
	ILI9341_SetLayer1Opacity(0);
	ILI9341_SetLayer2Opacity(255);
	ILI9341_SetLayer2();
	
	/* Memory is enabled */
	MemoryEnabled = 1;
}

void EMWIN_MemoryDisable(void) {
	/* Set layer 2 as view layer for LCD */
	/* EMWIN works on layer 1 and data will be transfered from layer 1 to layer 2 */
	/* after EMWIN finishes drawings using @ref EMWIN_Exec() function */
	ILI9341_SetLayer2Opacity(0);
	ILI9341_SetLayer1Opacity(255);
	ILI9341_SetLayer1();
	
	/* Memory is enabled */
	MemoryEnabled = 0;
}

uint32_t EMWIN_Exec(void) {
	int exec;
		
	/* Execute pending tasks */
	exec = GUI_Exec();
	
	/* If anything has been done and memory is enabled */
	if (EMWIN_LCD_DRIVER_CB_CALLED && MemoryEnabled) {
		/* Clear flag */
		//EMWIN_LCD_DRIVER_CB_CALLED = 0;
		
		/* Copy content from layer1 (EMWIN drawing layer) to layer2 (visible layer) */
		ILI9341_Layer1To2();
	}
	
	/* Return GUI status */
	return exec;
}

EMWIN_Result_t EMWIN_Rotate(EMWIN_Rotate_t rotation) {	
#if EMWIN_ROTATE_LCD == 0
	/* Return error */
	return EMWIN_Result_Error;	
#else
	uint8_t result;
	STMPE811_TouchData td;
	
	/* Try to rotate LCD */
	switch (rotation) {
		case EMWIN_Rotate_0:
			result = GUI_SetOrientation(0);
			td.orientation = STMPE811_Orientation_Portrait_2;
			break;
		case EMWIN_Rotate_90:
			result = GUI_SetOrientation(GUI_SWAP_XY | GUI_MIRROR_X);
			td.orientation = STMPE811_Orientation_Landscape_2;
			break;
		case EMWIN_Rotate_180:
			result = GUI_SetOrientation(GUI_MIRROR_X | GUI_MIRROR_Y);
			td.orientation = STMPE811_Orientation_Portrait_1;
			break;
		case EMWIN_Rotate_270:
			result = GUI_SetOrientation(GUI_SWAP_XY | GUI_MIRROR_Y);
			td.orientation = STMPE811_Orientation_Landscape_1;
			break;
		default:
			break;
	}
	
	/* Rotation was successfull? */
	if (result == 0) {
		/* Save new orientation */
		TS_Data.orientation = td.orientation;
		
		/* Return OK */
		return EMWIN_Result_Ok;
	}
	
	/* Return error */
	return EMWIN_Result_Error;
#endif
}

EMWIN_Result_t EMWIN_UpdateTouch(void) {
	static uint8_t millis = 0, last = 0;
	
	/* If we need to update it */
	if (++millis >= EMWIN_UPDATE_TOUCH_MILLIS) {
		/* Reset counter */
		millis = 0;
		
		/* Get data from touch */
		STMPE811_ReadTouch(&TS_Data);
		
		/* Get data */
		TS_State.x = TS_Data.x;
		TS_State.y = TS_Data.y;
		
		/* Pressed */
		TS_State.Pressed = (TS_Data.pressed == STMPE811_State_Pressed);
		
		/* Set layer */
		TS_State.Layer = 0;
		
		/* Update only if we have difference */
		if (
			TS_State.Pressed != last ||
			last
		) {
			/* Save new state */
			last = TS_State.Pressed;
			
			/* Update */
			GUI_TOUCH_StoreStateEx(&TS_State);
			
			/* Execute */
			GUI_Exec();
		}
	}
	
	/* Return OK */
	return EMWIN_Result_Ok;
}

/* LTDC IRQ Handler */
void LTDC_IRQHandler(void) {
	LTDC_ISR_Handler();
}

/* DMA2D IRQ Handler */
void DMA2D_IRQHandler(void) {
	DMA2D_ISR_Handler();
}

