/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    http://stm32f4-discovery.com/2014/05/library-10-stmpe811-touch-screen-driver-for-stm32f429-discovery-board/
 * @version v1.2
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   STMPE811 Touch screen controller library
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
#ifndef STMPE811_H
#define STMPE811_H  120

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup STM32F4xx_Libraries
 * @{
 */

/**
 * @defgroup STMPE811
 * @brief    STMPE811 Touch screen controller library - http://stm32f4-discovery.com/2014/05/library-10-stmpe811-touch-screen-driver-for-stm32f429-discovery-board/
 * @{
 *
 * It can be used on STM32F429 Discovery board, or any other LCD with 240*320 pixels
 *
 * \par Default pinout
 *
@verbatim
STMPE811   STM32F4xx   Description

SCL        PA8         Clock pin for I2C
SDA        PC9         Data pin for I2C
@endverbatim
 * 
 * @note This pinout is used on STM32F429 Discovery board.
 *
 * \par Custom pinout
 *
 * If you want to change pinout, you can add lines below in your defines.h file
 *
@verbatim
//Use custom I2C settings. Check TM I2C library for corresponding pins
#define STMPE811_I2C             I2C3
#define STMPE811_I2C_PINSPACK    I2C_PinsPack_1
@endverbatim
 *
 * \par Changelog
 *
@verbatim
 Version 1.0
  - First release
  
 Version 1.2
  - January 27, 2015
  - Added support for last pressed state

 Version 1.1
  - August 22, 2014
  - Fixes because on some boards my library for these touch didn't work
@endverbatim
 *
 * \par Dependencies
 *
@verbatim
 * STM32F4xx
 - STM32F4xx RCC
 - STM32F4xx GPIO
 - defines.h
 - TM I2C
 - TM DELAY
@endverbatim
 */
 
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "defines.h"
#include "stm32f4_i2c.h"
#include "stm32f4_delay.h"

/**
 * @defgroup STMPE811_Macros
 * @brief    Library defines
 * @{
 */

/**
 * @brief  Default I2C used, on STM32F429-Discovery board 
 */
#ifndef STMPE811_I2C
#define STMPE811_I2C					I2C3
#define STMPE811_I2C_PINSPACK			I2C_PinsPack_1
#endif

/**
 * @brief  Default I2C clock for STMPE811
 */
#ifndef STMPE811_I2C_CLOCK
#define STMPE811_I2C_CLOCK				100000
#endif

/**
 * @}
 */
 
/**
 * @defgroup STMPE811_Typedefs
 * @brief    Library Typedefs
 * @{
 */
 
/**
 * @brief  Enum for set how to read x and y from controller
 * @note   You may need experimenting to get proper orientation to match your LCD
 */
typedef enum {
	STMPE811_Orientation_Portrait_1,  /*!< Portrait orientation mode 1 */
	STMPE811_Orientation_Portrait_2,  /*!< Portrait orientation mode 2 */
	STMPE811_Orientation_Landscape_1, /*!< Landscape orientation mode 1 */
	STMPE811_Orientation_Landscape_2, /*!< Landscape orientation mode 2 */
} STMPE811_Orientation_t;

/**
 * @brief  Enumeration for touch pressed or released
 */
typedef enum {
	STMPE811_State_Pressed,  /*!< Touch detected as pressed */
	STMPE811_State_Released, /*!< Touch detected as released/not pressed */
	STMPE811_State_Ok,       /*!< Result OK. Used on initialization */
	STMPE811_State_Error     /*!< Result error. Used on initialization */
} STMPE811_State_t;

/**
 * @brief  Main structure, which is passed into @ref STMPE811_ReadTouch function
 */
typedef struct {
	uint16_t x;                            /*!< X coordinate on LCD for touch */ 
	uint16_t y;                            /*!< Y coordinate on LCD for touch */
	STMPE811_State_t pressed;           /*!< Pressed touch status */
	STMPE811_State_t last_pressed;      /*!< Last pressed touch status */
	STMPE811_Orientation_t orientation; /*!< Touch screen orientation to match your LCD orientation */
} STMPE811_t;

/* Backward compatibility */
typedef STMPE811_t STMPE811_TouchData;

/**
 * @}
 */

/**
 * @defgroup STMPE811_Functions
 * @brief    Library Functions
 * @{
 */
 
/**
 * @brief  Initializes STMPE811 Touch driver
 * @param  None
 * @retval Member of @ref STMPE811_State_t
 */
STMPE811_State_t STMPE811_Init(void);

/**
 * @brief  Reads touch coordinates
 * @param  *structdata: Pointer to @ref STMPE811_t to store data into
 * @retval Touch status:
 *            - STMPE811_State_Pressed: Touch detected as pressed, coordinates valid
 *            - STMPE811_State_Released: Touch detected as not pressed, coordinates not valid
 */
STMPE811_State_t STMPE811_ReadTouch(STMPE811_t* structdata);

/**
 * @brief  Checks if touch data is inside specific rectangle coordinates
 * @param  sd: Pointer to @ref STMPE811_t to get data from
 * @param  xPos: Top-left X position of rectangle
 * @param  yPos: Top-left Y position of rectangle
 * @param  w: Rectangle width
 * @param  h: Rectangle height:
 * @retval Touch inside rectangle status:
 *            - 0: Touch is outside rectangle
 *            - > 0: Touch is inside rectangle
 * @note   Defined as macro for faster execution
 */ 
#define STMPE811_TouchInRectangle(sd, xPos, yPos, w, h)	(((sd)->x >= (xPos)) && ((sd)->x < (xPos + w)) && ((sd)->y >= (yPos)) && ((sd)->y < (yPos + h)))

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

