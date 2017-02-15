/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    http://stm32f4-discovery.com/2014/04/stm32f429-discovery-gpio-tutorial-with-onboard-leds-and-button/
 * @version v1.11
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   Leds and button library for STM32F401 - , STM32F4 -, STM32F411 - and STM32F429 Discovery boards.
 *          Also works with Nucleo F411 and Nucleo F401 boards and STM324x9-EVAL boards
 *	
@verbatim
   ----------------------------------------------------------------------
    Copyright (C) Tilen Majerle, 2015
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.
     
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
   ----------------------------------------------------------------------
@endverbatim
 */
#ifndef DISCO_H
#define DISCO_H 1110
/**
 * @addtogroup STM32F4xx_Libraries
 * @{
 */

/**
 * @defgroup DISCO
 * @brief    Leds and buttons for STM32F4 Discovery, Nucleo and eval boards - http://stm32f4-discovery.com/2014/04/stm32f429-discovery-gpio-tutorial-with-onboard-leds-and-button/
 * @{
 *
 * Library supports all STM32F4 Discovery boards, All STM32F4 Nucleo boards and STM324x9 eval board.
 *	
 * \par Supported boards
 *			
 * - STM32F429 Discovery: (STM32F429ZI) - <code>DISCO_STM32F429_DISCOVERY</code>
 *   - Leds:
 *     - LED_GREEN   on PG13
 *     - LED_RED     on PG14
 *   - Button: (HIGH when pressed)
 *     - Blue button on PA0	
 * - NUCLEO-F401: (STM32F401RE) - <code>DISCO_NUCLEO_F401</code>
 * - NUCLEO-F411: (STM32F411RE) - <code>DISCO_NUCLEO_F411</code>
 *   - Led:
 *     - LED_GREEN   on PA5
 *   - Button: (LOW when pressed)
 *     - Blue button on PC13
 * - STM32F401-Discovery: (STM32F401VC) - <code>DISCO_STM32F401_DISCOVERY</code>
 * - STM32F411-Discovery: (STM32F411VE) - <code>DISCO_STM32F411_DISCOVERY</code>
 * - STM32F4-Discovery: (STM32F407VG) - <code>DISCO_STM32F4_DISCOVERY</code>
 *   - Leds:
 *     - LED_GREEN   on PD12
 *     - LED_ORANGE  on PD13
 *     - LED_RED     on PD14
 *     - LED_BLUE    on PD15
 *   - Button: (HIGH when pressed)
 *     - Blue button on PA0
 * - STM324x9-Eval (STM32F439NI) - <code>DISCO_STM324x9_EVAL</code>
 *   - Leds:
 *     - LED_GREEN   on PG6
 *     - LED_ORANGE  on PG7
 *     - LED_RED     on PG10
 *     - LED_BLUE    on PG12
 *   - Button: (HIGH when pressed)
 *     - Blue button on PA0
 *
 * \par Select your board
 *
 * To select your board, you have several options:
 *   - Add define for your board in defines.h file or
 *   - Add define for your board in compiler's global settings
 *     - For Keil uVision you have "Options for Target" and "C/C++" tab where you can set this.
 *
 * Imagine, we want to work with STM324x9-Eval board. Then, you can open <code>defines.h</code> file and add define:
 *
@verbatim
//Select STM324x9-Eval for DISCO library
#define DISCO_STM324x9_EVAL
@endverbatim
 * Or if you want STM32F429-Discovery, do this:
@verbatim
//Select STM32F429-Discovery for DISCO library
#define DISCO_STM32F429_DISCOVERY
@endverbatim
 *
 * \par Changelog
 *
@verbatim
 Version 1.11
  - March 18, 2015
  - Added support for STM324x9-Eval board
  
 Version 1.10
  - March 14, 2015
  - Fixed issue with pull resistors on boards

 Version 1.9
  - March 10, 2015
  - Added support for my new GPIO library
  - Added support for STM32F411-Discovery board

 Version 1.8
  - February 01, 2015
  - Added support for button OnPress and OnRelease events
 
 Version 1.7
  - December 02, 2014
  - Fixed bug with checking if led is on
 
 Version 1.6
  - November 28, 2014
  - Almost all functions are now defines, for faster execution		
 
 Version 1.5
  - November 06, 2014
  - Added function DISCO_SetLed()
 
 Version 1.4
  - Added support for Nucleo F411-RE board
 
 Version 1.3
  - Added support for STM32F401 Discovery board
 
 Version 1.2
  - Added support for Nucleo F401-RE board
 	
 Version 1.1
  - Check if LED is on or off

 Version 1.0
  - First release
@endverbatim
 *
 * \par Dependencies
 *
@verbatim
 - STM32F4xx
 - defines.h
 - TM GPIO
@endverbatim
 */
#include "stm32f4xx.h"
#include "defines.h"
#include "stm32f4_gpio.h"

/* Recognize correct board */
/* CooCox support */
#if defined(STM32F429_439xx) || defined(STM32F429ZI)
	/* STM32F429 Discovery support */
	#ifndef DISCO_STM32F429_DISCOVERY
		#define DISCO_STM32F429_DISCOVERY
	#endif 
#elif defined(STM32F407VG) || defined(STM32F401VC)// || defined(STM32F40_41xxx)
	/* STM32F4 and STM32F401 Discovery support */
	#ifndef DISCO_STM32F4_DISCOVERY
		#define DISCO_STM32F4_DISCOVERY
	#endif
#elif defined (STM32F401xx) || defined(STM32F401RE) || defined(STM32F401RB)
	/* Nucleo F401RE board support */
	#ifndef DISCO_NUCLEO_F401
		#define DISCO_NUCLEO_F401
	#endif
#elif defined (STM32F411xx) || defined(STM32F411RE) || defined(STM32F411RB)
	/* Nucleo F411RE board support */
	#ifndef DISCO_NUCLEO_F411
		#define DISCO_NUCLEO_F411
	#endif
#endif

/* STM32F429 Discovery */
#if defined(DISCO_STM324x9_EVAL)
	#define LED_GREEN					GPIO_PIN_6
	#define LED_ORANGE					GPIO_PIN_7
	#define LED_RED						GPIO_PIN_10
	#define LED_BLUE					GPIO_PIN_12
	#define LED_ALL						LED_GREEN | LED_RED | LED_ORANGE | LED_BLUE
	
	#define DISCO_SWAP_LOGIC
	
	#define DISCO_LED_PORT			GPIOG
	#define DISCO_LED_PINS			LED_GREEN | LED_RED | LED_ORANGE | LED_BLUE

	#define DISCO_BUTTON_PORT		GPIOA
	#define DISCO_BUTTON_PIN			GPIO_PIN_0
	#define DISCO_BUTTON_PRESSED		1
	#define DISCO_BUTTON_PULL		GPIO_PuPd_DOWN
#elif defined(DISCO_STM32F429_DISCOVERY)
	#define LED_GREEN					GPIO_PIN_13
	#define LED_RED						GPIO_PIN_14
	#define LED_ORANGE					0
	#define LED_BLUE					0
	#define LED_ALL						LED_GREEN | LED_RED
	
	#define DISCO_LED_PORT			GPIOG
	#define DISCO_LED_PINS			LED_GREEN | LED_RED

	#define DISCO_BUTTON_PORT		GPIOA
	#define DISCO_BUTTON_PIN			GPIO_PIN_0
	#define DISCO_BUTTON_PRESSED		1
	#define DISCO_BUTTON_PULL		GPIO_PuPd_DOWN
/* STM32F4 & STM32F401 Discovery */
#elif defined(DISCO_STM32F4_DISCOVERY) || defined(DISCO_STM32F401_DISCOVERY) || defined(DISCO_STM32F411_DISCOVERY)
	#define LED_GREEN					GPIO_PIN_12
	#define LED_ORANGE					GPIO_PIN_13
	#define LED_RED						GPIO_PIN_14
	#define LED_BLUE					GPIO_PIN_15
	#define LED_ALL						LED_GREEN | LED_RED | LED_ORANGE | LED_BLUE
	
	#define DISCO_LED_PORT			GPIOD
	#define DISCO_LED_PINS			LED_GREEN | LED_RED | LED_ORANGE | LED_BLUE

	#define DISCO_BUTTON_PORT		GPIOA
	#define DISCO_BUTTON_PIN			GPIO_PIN_0
	#define DISCO_BUTTON_PRESSED		1
	#define DISCO_BUTTON_PULL		GPIO_PuPd_DOWN
/* Nucleo F401-RE & F411-RE */
#elif defined(DISCO_NUCLEO_F401) || defined(DISCO_NUCLEO_F411)
	#define LED_GREEN					GPIO_PIN_5
	#define LED_RED						0
	#define LED_ORANGE					0
	#define LED_BLUE					0
	#define LED_ALL						LED_GREEN
	
	#define DISCO_LED_PORT			GPIOA
	#define DISCO_LED_PINS			LED_GREEN

	#define DISCO_BUTTON_PORT		GPIOC
	#define DISCO_BUTTON_PIN			GPIO_PIN_13
	#define DISCO_BUTTON_PRESSED		0
	#define DISCO_BUTTON_PULL		GPIO_PuPd_UP
/* STM324x9 EVAL board */
#else
	#error "stm32f4_disco.h: Please select your board. Open stm32f4_disco.h and follow instructions!!"
#endif

/**
 * @defgroup DISCO_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Configures LED pins as outputs
 * @param  None
 * @retval None
 */
void DISCO_LedInit(void);

/**
 * @brief  Configures Button pin as input
 * @param  None
 * @retval None
 */
void DISCO_ButtonInit(void);

/**
 * @brief  Turns on LED on board
 * @note   STM32F4x9-Eval board uses inverse logic for leds
 * @param  led: LED you want to turn on
 *            - LED_RED: Red led
 *            - LED_GREEN: Green led
 *            - LED_BLUE: Blue led
 *            - LED_ORANGE: Orange led
 *            - LED_ALL: All leds
 * @retval None
 */
#ifndef DISCO_SWAP_LOGIC
	#define DISCO_LedOn(led)        GPIO_SetPinHigh(DISCO_LED_PORT, (led))
#else
	#define DISCO_LedOn(led)        GPIO_SetPinLow(DISCO_LED_PORT, (led))
#endif

/**
 * @brief  Turns off LED on board
 * @note   STM32F4x9-Eval board uses inverse logic for leds
 * @param  led: LED you want to turn off
 *            - LED_RED: Red led
 *            - LED_GREEN: Green led
 *            - LED_BLUE: Blue led
 *            - LED_ORANGE: Orange led
 *            - LED_ALL: All leds
 * @retval None
 */
#ifndef DISCO_SWAP_LOGIC
	#define DISCO_LedOff(led)       GPIO_SetPinLow(DISCO_LED_PORT, (led))
#else
	#define DISCO_LedOff(led)       GPIO_SetPinHigh(DISCO_LED_PORT, (led))
#endif

/**
 * @brief  Toggles LED on board
 * @param  led: LED you want to toggle
 *            - LED_RED: Red led
 *            - LED_GREEN: Green led
 *            - LED_BLUE: Blue led
 *            - LED_ORANGE: Orange led
 *            - LED_ALL: All leds
 * @retval None
 */
#define DISCO_LedToggle(led)        GPIO_TogglePinValue(DISCO_LED_PORT, (led))

/**
 * @brief  Checks if led is on
 * @note   STM32F4x9-Eval board uses inverse logic for leds
 * @param  led: Led you want to checking
 *            - LED_RED: Red led
 *            - LED_GREEN: Green led
 *            - LED_BLUE: Blue led
 *            - LED_ORANGE: Orange led
 *            - LED_ALL: All leds
 * @retval 1 if led is on or 0 if not
 */
#ifndef DISCO_SWAP_LOGIC
#define DISCO_LedIsOn(led)          GPIO_GetOutputPinValue(DISCO_LED_PORT, (led))
#else
#define DISCO_LedIsOn(led)          !GPIO_GetOutputPinValue(DISCO_LED_PORT, (led))
#endif

/**
 * @brief  Sets led value
 * @param  led: LED you want to set value
 *            - LED_RED: Red led
 *            - LED_GREEN: Green led
 *            - LED_BLUE: Blue led
 *            - LED_ORANGE: Orange led
 *            - LED_ALL: All leds
 * @param  state: Set or clear led 
 *            - 0: led is off
 *            - > 0: led is on
 * @retval None
 */
#define DISCO_SetLed(led, state)    ((state) ? DISCO_LedOn(led): DISCO_LedOff(led))

/**
 * @brief  Checks if user button is pressed
 * @param  None
 * @retval Button status
 *            - 0: Button is not pressed
 *            - > 0: Button is pressed
 */
#define DISCO_ButtonPressed()       ((GPIO_GetInputPinValue(DISCO_BUTTON_PORT, DISCO_BUTTON_PIN) == 0) != DISCO_BUTTON_PRESSED)

/**
 * @brief  Checks if button was pressed now, but was not already pressed before
 * @param  None
 * @retval Button on pressed value
 *           - 0: In case that button has been already pressed on last call or was not pressed at all yet
 *           - > 0: Button was pressed, but state before was released
 */
uint8_t DISCO_ButtonOnPressed(void);

/**
 * @brief  Checks if button was released now, but was already pressed before
 * @param  None
 * @retval Button on released value
 *           - 0: In case that button has been already released on last call or was not released at all yet
 *           - > 0: Button was released, but state before was pressed
 */
uint8_t DISCO_ButtonOnReleased(void);

/**
 * @}
 */
 
/**
 * @}
 */
 
/**
 * @}
 */

#endif
