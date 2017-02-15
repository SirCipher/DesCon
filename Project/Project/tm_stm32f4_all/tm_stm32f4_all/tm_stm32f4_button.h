/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    http://stm32f4-discovery.com/2015/04/library-57-buttons-for-stm32f4xx/
 * @version v1.0
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   Buttons library for STM32F4xx devices
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
#ifndef BUTTON_H
#define BUTTON_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup STM32F4xx_Libraries
 * @{
 */

/**
 * @defgroup BUTTON
 * @brief    Button library for STM32F4xx devices - http://stm32f4-discovery.com/2015/04/library-57-buttons-for-stm32f4xx/
 * @{
 *
 * This library works with external buttons.
 * It can detect button on pressed, normal button press and long button press
 *
 * \par Changelog
 *
@verbatim
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
 - TM DELAY
 - stdlib.h
@endverbatim
 */

#include "stm32f4xx.h"
#include "defines.h"
#include "stm32f4_gpio.h"
#include "stm32f4_delay.h"
#include "stdlib.h"

/**
 * @defgroup BUTTON_Macros
 * @brief    Library defines
 * @{
 */

/* Number of maximal supported buttons */
#ifndef BUTTON_MAX_BUTTONS
#define BUTTON_MAX_BUTTONS        10
#endif

/* Number of milliseconds for normal press detection */
#ifndef BUTTON_NORMAL_PRESS_TIME
#define BUTTON_NORMAL_PRESS_TIME  100
#endif

/* Number of milliseconds for long press detection */
#ifndef BUTTON_LONG_PRESS_TIME
#define BUTTON_LONG_PRESS_TIME    1500
#endif

/**
 * @}
 */
 
/**
 * @defgroup BUTTON_Typedefs
 * @brief    Library Typedefs
 * @{
 */
 
/**
 * @brief  Button possible press types
 */
typedef enum {
	BUTTON_PressType_OnPressed = 0x00, /*!< Button pressed */
	BUTTON_PressType_Normal,           /*!< Normal press type, released */
	BUTTON_PressType_Long              /*!< Long press type */
} BUTTON_PressType_t;

/** 
 * @brief  Button private structure 
 */
typedef struct _BUTTON_t {
	GPIO_TypeDef* GPIOx;                          /*!< GPIOx PORT for button */
	uint16_t GPIO_Pin;                            /*!< GPIO pin for button */
	uint8_t GPIO_State;                           /*!< GPIO state for pin when pressed */
	void (*ButtonHandler)(BUTTON_PressType_t); /*!< Button function handler */
	uint32_t StartTime;                           /*!< Time when button was pressed */
	uint8_t LastStatus;                           /*!< Button status on last check */
	uint8_t State;                                /*!< Current button state */
	uint16_t PressNormalTime;                     /*!< Time in ms for normal press for button */
	uint16_t PressLongTime;                       /*!< Time in ms for long press for button */
} BUTTON_t;

/**
 * @}
 */

/**
 * @defgroup BUTTON_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes a new button to library
 * @note   This library uses @ref malloc() to allocate memory, so make sure you have enough heap memory
 * @param  *GPIOx: Pointer to GPIOx where button is located
 * @param  GPIO_Pin: GPIO pin where button is located
 * @param  ButtonState: Button state when it is pressed.
 *            - 0: Button is low when pressed
 *            - > 0: Button is high when pressed
 * @param  *ButtonHandler:
 * @retval Button creation status:
 *            - 0: Button was not created 
 *            - > 0: Button created and saved to library, button pointer is returned
 */
BUTTON_t* BUTTON_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t ButtonState, void (*ButtonHandler)(BUTTON_PressType_t));

/**
 * @brief  Sets press timing values
 * @param  *ButtonStruct: Pointer to @ref BUTTON_t structure for button used
 * @param  Normal: Time that button must be pressed to indicate normal press. Value is in milliseconds
 * @param  Normal: Time that button must be pressed to indicate long press. Value is in milliseconds
 * @retval Pointer to @ref BUTTON_t
 */
BUTTON_t* BUTTON_SetPressTime(BUTTON_t* ButtonStruct, uint16_t Normal, uint16_t Long);

/**
 * @brief  Updates buttons. This function have to be called periodically
 * @note   Function will automatically call callback functions for buttons
 * @param  None
 * @retval None
 */
void BUTTON_Update(void);

/**
 * @}
 */
 
/**
 * @}
 */
 
/**
 * @}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
