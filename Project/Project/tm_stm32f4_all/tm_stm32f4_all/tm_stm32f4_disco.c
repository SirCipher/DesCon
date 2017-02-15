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
#include "stm32f4_disco.h"

/* Button pressed status for onpressed/onreleased events */
static volatile uint8_t INT_DISCO_ButtonPressed = 0;

void DISCO_LedInit(void) {
	/* Set pins as output */
	GPIO_Init(DISCO_LED_PORT, LED_ALL, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High);
	
	/* Turn leds off */
	DISCO_LedOff(LED_ALL);
}

void DISCO_ButtonInit(void) {
	/* Set pin as input */
	GPIO_Init(DISCO_BUTTON_PORT, DISCO_BUTTON_PIN, GPIO_Mode_IN, GPIO_OType_PP, DISCO_BUTTON_PULL, GPIO_Speed_Low);
}

uint8_t DISCO_ButtonOnPressed(void) {
	/* If button is now pressed, but was not already pressed */
	if (DISCO_ButtonPressed()) {
		if (!INT_DISCO_ButtonPressed) {
			/* Set flag */
			INT_DISCO_ButtonPressed = 1;
			
			/* Return button onpressed */
			return 1;
		}
	} else {
		/* Clear flag */
		INT_DISCO_ButtonPressed = 0;
	}
	
	/* Button is not pressed or it was already pressed before */
	return 0;
}

uint8_t DISCO_ButtonOnReleased(void) {
	/* If button is now released, but was not already released */
	if (!DISCO_ButtonPressed()) {
		if (INT_DISCO_ButtonPressed) {
			/* Set flag */
			INT_DISCO_ButtonPressed = 0;
			
			/* Return button onreleased */
			return 1;
		}
	} else {
		/* Set flag */
		INT_DISCO_ButtonPressed = 1;
	}
	
	/* Button is not released or it was already released before */
	return 0;
}
