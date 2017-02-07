/*----------------------------------------------------------------------------
 * Name:    SENSORS.h
 * Author: 	Wil Bennett
 *----------------------------------------------------------------------------
 * Creates a high-level library for general I2C use, based on the low-level I2C 
 * peripheral library available from ST
 *----------------------------------------------------------------------------*/

#ifndef __SENSORS_H
#define __SENSORS_H

//#include <stm32f4xx.h>
#include <stm32f4xx_i2c.h>

//Generic return values
#define FUNCTION_OK			0x00
#define FUNCTION_FAIL		0x01

//Addresses for the PICKIT Serial I2C demo board peripherals, and their registers
#define TEMP_ADDRESS		0x92
	#define	TEMP_READ_REG		0x00
	#define	TEMP_CONFIG_REG	0x01
	#define	TEMP_HYST_REG		0x02
	#define	TEMP_LIMIT_REG	0x03
#define ADC_ADDRESS 		0x9A
#define GPIO_ADDRESS 		0x40
	#define	GPIO_IODIR_REG		0x00
	#define	GPIO_IPOL_REG			0x01
	#define	GPIO_GPINTEN_REG	0x02
	#define	GPIO_DEFVAL_REG		0x03
	#define	GPIO_INTCON_REG		0x04
	#define	GPIO_IOCON_REG		0x05
	#define	GPIO_GPPU_REG			0x06
	#define	GPIO_INTF_REG			0x07
	#define	GPIO_INTCAP_REG		0x08
	#define	GPIO_GPIO_REG			0x09
	#define	GPIO_OLAT_REG			0x0A
//#define EEPROM_ADDRESS 	0xA7
//#define DAC_ADDRESS 		0x90

extern	void			I2C_Temp_Init		(void);
extern	double		I2C_Temp_Read		(void);
extern	uint16_t	I2C_ADC_Read		(void);
extern	void			I2C_GPIO_Init		(void);
extern	void 			I2C_GPIO_Direction	(uint8_t	direction);
extern	uint8_t		I2C_GPIO_Read		(void);
extern	void			I2C_GPIO_Write	(uint8_t writeByte);
//extern	uint8_t		I2C_EEPROM_Init	(void);
//extern	uint8_t		I2C_EEPROM_Write(uint16_t address, uint8_t writeState);
//extern	uint8_t		I2C_EEPROM_Read	(uint16_t address, uint8_t* readState);
//extern	void			I2C_DAC_Init		(void);
//extern	uint8_t		I2C_DAC_Write		(uint16_t);

#endif
