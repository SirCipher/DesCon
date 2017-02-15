/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    
 * @version v1.0
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   Brown-Out detection for STM32F4xx
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
#ifndef BOR_H
#define BOR_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup STM32F4xx_Libraries
 * @{
 */

/**
 * @defgroup BOR
 * @brief    Brown-Out detection for STM32F4xx
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
 - STM32F4xx FLASH
 - defines.h
@endverbatim
 */

#include "stm32f4xx.h"
#include "stm32f4xx_flash.h"
#include "defines.h"

/**
 * @defgroup BOR_Macros
 * @brief    Library defines
 * @{
 */
/* Macros here */
/**
 * @}
 */
 
/**
 * @defgroup BOR_Typedefs
 * @brief    Library Typedefs
 * @{
 */
 
typedef enum {
	BOR_Level_None = OB_BOR_OFF,
	BOR_Level_1 = OB_BOR_LEVEL1,
	BOR_Level_2 = OB_BOR_LEVEL2,
	BOR_Level_3 = OB_BOR_LEVEL3
} BOR_Level_t;
 
typedef enum {
	BOR_Result_Ok,   /*!< Everything OK */
	BOR_Result_Error /*!< An error has occurred */
} BOR_Result_t;

/**
 * @}
 */

/**
 * @defgroup BOR_Functions
 * @brief    Library Functions
 * @{
 */

BOR_Result_t BOR_Set(BOR_Level_t BORValue);
BOR_Level_t BOR_Get(void);

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
