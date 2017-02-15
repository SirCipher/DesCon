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
#include "stm32f4_rotary_encoder.h"

/* Return with status macro */
#define RETURN_WITH_STATUS(p, s)    (p)->Rotation = s; return s

void RE_Init(RE_t* data, GPIO_TypeDef* GPIO_A_Port, uint16_t GPIO_A_Pin, GPIO_TypeDef* GPIO_B_Port, uint16_t GPIO_B_Pin) {
	/* Save parameters */
	data->GPIO_A = GPIO_A_Port;
	data->GPIO_B = GPIO_B_Port;
	data->GPIO_PIN_A = GPIO_A_Pin;
	data->GPIO_PIN_B = GPIO_B_Pin;
	
	/* Set pin A as exti interrupt */
	EXTI_Attach(data->GPIO_A, data->GPIO_PIN_A, EXTI_Trigger_Rising_Falling);
	
	/* Set pin B as input */
	GPIO_Init(data->GPIO_B, data->GPIO_PIN_B, GPIO_Mode_IN, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_Low);
	
	/* Set default mode */
	data->Mode = RE_Mode_Zero;
	
	/* Set default */
	data->RE_Count = 0;
	data->Diff = 0;
	data->Absolute = 0;
	data->LastA = 1;
}

RE_Rotate_t RE_Get(RE_t* data) {
	/* Calculate everything */
	data->Diff = data->RE_Count - data->Absolute;
	data->Absolute = data->RE_Count;
	
	/* Check */
	if (data->RE_Count < 0) {
		RETURN_WITH_STATUS(data, RE_Rotate_Decrement);
	} else if (data->RE_Count > 0) {
		RETURN_WITH_STATUS(data, RE_Rotate_Increment);
	}
	
	RETURN_WITH_STATUS(data, RE_Rotate_Nothing);
}

void RE_SetMode(RE_t* data, RE_Mode_t mode) {
	/* Set mode */
	data->Mode = mode;
}

void RE_Process(RE_t* data) {
	uint8_t now_a;
	uint8_t now_b;
	
	/* Read inputs */
	now_a = GPIO_GetInputPinValue(data->GPIO_A, data->GPIO_PIN_A);
	now_b = GPIO_GetInputPinValue(data->GPIO_B, data->GPIO_PIN_B);
	
	/* Check difference */
	if (now_a != data->LastA) {
		data->LastA = now_a;
		
		if (data->LastA == 0) {
			/* Check mode */
			if (data->Mode == RE_Mode_Zero) {
				if (now_b == 1) {
					data->RE_Count--;
				} else {
					data->RE_Count++;
				}
			} else {
				if (now_b == 1) {
					data->RE_Count++;
				} else {
					data->RE_Count--;
				}
			}
		}
	}
}

