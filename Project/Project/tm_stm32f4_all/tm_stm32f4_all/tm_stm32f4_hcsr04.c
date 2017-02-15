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
#include "stm32f4_hcsr04.h"

uint8_t HCSR04_Init(HCSR04_t* HCSR04, GPIO_TypeDef* ECHO_GPIOx, uint16_t ECHO_GPIO_Pin, GPIO_TypeDef* TRIGGER_GPIOx, uint16_t TRIGGER_GPIO_Pin) {	
	/* Init Delay functions */
	DELAY_Init();
	
	/* Save everything */
	HCSR04->ECHO_GPIOx = ECHO_GPIOx;
	HCSR04->ECHO_GPIO_Pin = ECHO_GPIO_Pin;
	HCSR04->TRIGGER_GPIOx = TRIGGER_GPIOx;
	HCSR04->TRIGGER_GPIO_Pin = TRIGGER_GPIO_Pin;
	
	/* Initialize pins */	
	/* Trigger pin */
	GPIO_Init(HCSR04->TRIGGER_GPIOx, HCSR04->TRIGGER_GPIO_Pin, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_DOWN, GPIO_Speed_Medium);
	
	/* Echo pin */
	GPIO_Init(HCSR04->ECHO_GPIOx, HCSR04->ECHO_GPIO_Pin, GPIO_Mode_IN, GPIO_OType_PP, GPIO_PuPd_DOWN, GPIO_Speed_Medium);
	
	/* Trigger set to low */
	GPIO_SetPinLow(HCSR04->TRIGGER_GPIOx, HCSR04->TRIGGER_GPIO_Pin);
	
	/* Start measurement, check if sensor is working */
	if (HCSR04_Read(HCSR04) >= 0) {
		/* Sensor OK */
		return 1;
	}
	
	/* Sensor error */
	return 0;
}

float HCSR04_Read(HCSR04_t* HCSR04) {
	uint32_t time, timeout;
	/* Trigger low */
	GPIO_SetPinLow(HCSR04->TRIGGER_GPIOx, HCSR04->TRIGGER_GPIO_Pin);
	/* Delay 2 us */
	Delay(2);
	/* Trigger high for 10us */
	GPIO_SetPinHigh(HCSR04->TRIGGER_GPIOx, HCSR04->TRIGGER_GPIO_Pin);
	/* Delay 10 us */
	Delay(10);
	/* Trigger low */
	GPIO_SetPinLow(HCSR04->TRIGGER_GPIOx, HCSR04->TRIGGER_GPIO_Pin);
	
	/* Give some time for response */
	timeout = HCSR04_TIMEOUT;
	while (!GPIO_GetInputPinValue(HCSR04->ECHO_GPIOx, HCSR04->ECHO_GPIO_Pin)) {
		if (timeout-- == 0x00) {
			return -1;
		}
	}
	
	/* Start time */
	time = 0;
	/* Wait till signal is low */
	while (GPIO_GetInputPinValue(HCSR04->ECHO_GPIOx, HCSR04->ECHO_GPIO_Pin)) {
		/* Increase time */
		time++;
		/* Delay 1us */
		Delay(1);
	}
	
	/* Convert us to cm */
	HCSR04->Distance =  (float)time * HCSR04_NUMBER;
	
	/* Return distance */
	return HCSR04->Distance;
}
