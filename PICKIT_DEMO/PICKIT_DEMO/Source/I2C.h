/*----------------------------------------------------------------------------
 * Name:    I2C.h
 * Author: 	STMicroelectronics
 *----------------------------------------------------------------------------
 * Creates a high-level library for general I2C use, based on the low-level I2C 
 * peripheral library available from ST
 *----------------------------------------------------------------------------*/

#ifndef __I2C_H
#define __I2C_H

//#include <stm32f4xx.h>
#include <stm32f4xx_i2c.h>

extern void 		Init_I2C1								(void);
extern void 		Init_I2C2								(void); 							//Added by WB
extern void 		Init_I2C3								(void); 							//Added by WB
extern void 		I2C_start								(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction);
extern void 		I2C_write								(I2C_TypeDef* I2Cx, uint8_t data);
extern uint8_t	I2C_read_ack						(I2C_TypeDef* I2Cx);
extern uint8_t 	I2C_read_nack						(I2C_TypeDef* I2Cx);
extern void 		I2C_stop								(I2C_TypeDef* I2Cx);

#endif

