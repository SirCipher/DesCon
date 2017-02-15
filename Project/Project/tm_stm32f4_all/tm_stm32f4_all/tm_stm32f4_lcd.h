/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    
 * @version v1.0
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   Library template 
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
#ifndef LIBRARY_H
#define LIBRARY_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup STM32F4xx_Libraries
 * @{
 */

/**
 * @defgroup LCD
 * @brief    Library description here
 * @{
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
 - STM32F4xx RCC
 - defines.h
@endverbatim
 */

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_ltdc.h"
#include "defines.h"
#include "stm32f4_sdram.h"
#include "stm32f4_gpio.h"
#include "stm32f4_fonts.h"
#include "stm32f4_dma2d_graphic.h"

/**
 * @defgroup LCD_Macros
 * @brief    Library defines
 * @{
 */

#if defined(USE_LCD_STM324x9_EVAL) || defined(DISCO_STM324x9_EVAL)

/* Check define */
#ifndef USE_LCD_STM324x9_EVAL
#define USE_LCD_STM324x9_EVAL
#endif

/* Set pixel settings */
#define LCD_PIXEL_WIDTH        640
#define LCD_PIXEL_HEIGHT       480

#else

/* STM32F429-Discovery */
#define LCD_PIXEL_WIDTH        240
#define LCD_PIXEL_HEIGHT       320

#endif

/* Frame settings */
#define LCD_FRAME_BUFFER       (SDRAM_START_ADR)
#define LCD_BUFFER_OFFSET      ((uint32_t)(LCD_PIXEL_WIDTH * LCD_PIXEL_HEIGHT * 2))

/**
 * @}
 */
 
/**
 * @defgroup LCD_Typedefs
 * @brief    Library Typedefs
 * @{
 */

typedef enum _LCD_Result_t {
	LCD_Result_Ok = 0x00,
	LCD_Result_Error,
	LCD_Result_SDRAM_Error
} LCD_Result_t;

/**
 * @}
 */

/**
 * @defgroup LCD_Functions
 * @brief    Library Functions
 * @{
 */

LCD_Result_t LCD_Init(void);
LCD_Result_t LCD_Fill(uint32_t color);
LCD_Result_t LCD_DisplayOn(void);
LCD_Result_t LCD_DisplayOff(void);
uint16_t LCD_GetWidth(void);
uint16_t LCD_GetHeight(void);
uint32_t LCD_GetFrameBuffer(void);

LCD_Result_t LCD_SetXY(uint16_t X, uint16_t Y);
LCD_Result_t LCD_SetFont(FontDef_t* Font);
LCD_Result_t LCD_SetColors(uint32_t Foreground, uint32_t Background);
LCD_Result_t LCD_Putc(char c);
LCD_Result_t LCD_Puts(char* str);

/**
 * @defgroup LCD_GRAPHIC_Functions
 * @brief    Library Functions
 * @{
 */

LCD_Result_t LCD_DrawPixel(uint16_t X, uint16_t Y, uint32_t color);
uint32_t LCD_GetPixel(uint16_t X, uint16_t Y);
LCD_Result_t LCD_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color);
LCD_Result_t LCD_DrawRectangle(uint16_t x0, uint16_t y0, uint16_t Width, uint16_t Height, uint32_t color);
LCD_Result_t LCD_DrawFilledRectangle(uint16_t x0, uint16_t y0, uint16_t Width, uint16_t Height, uint32_t color);
LCD_Result_t LCD_DrawRoundedRectangle(uint16_t x0, uint16_t y0, uint16_t Width, uint16_t Height, uint16_t r, uint32_t color);
LCD_Result_t LCD_DrawFilledRoundedRectangle(uint16_t x0, uint16_t y0, uint16_t Width, uint16_t Height, uint16_t r, uint32_t color);
LCD_Result_t LCD_DrawCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color);
LCD_Result_t LCD_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color);

/**
 * @}
 */

/**
 * @defgroup LCD_LAYER_Functions
 * @brief    Library Functions
 * @{
 */
 
LCD_Result_t LCD_SetLayer1(void);
LCD_Result_t LCD_SetLayer2(void);
LCD_Result_t LCD_SetLayer1Opacity(uint8_t opacity);
LCD_Result_t LCD_SetLayer2Opacity(uint8_t opacity);
LCD_Result_t LCD_ChangeLayers(void);
LCD_Result_t LCD_Layer2To1(void);
LCD_Result_t LCD_Layer1To2(void);

/**
 * @}
 */
 
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
