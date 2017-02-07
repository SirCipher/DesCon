/*----------------------------------------------------------------------------
 * Name:    SENSORS.c
 * Author: 	Wil Bennett
 *----------------------------------------------------------------------------
 * Provides high-level functions for all aspects of interfacing with some of
 * the devices on the PICkit over I2C
 *----------------------------------------------------------------------------*/
 
#include <stm32f4xx.h>
#include <stm32f4xx_i2c.h>
//#include "arm_math.h" 
#include "main.h"
#include <math.h>
//#include <stdio.h>
#include "I2C.h"
#include "LCD.h"
#include "SENSORS.h"

extern volatile uint32_t msTicks;

/*----------------------------------------------------------------------------
//Starts the I2C temperature sensor
 *----------------------------------------------------------------------------*/
void I2C_Temp_Init(void){
	
	//Select config register values
		uint8_t config_reg_data = 0x00;
		uint8_t one_shot				=	0;		
		uint8_t resolution			=	11;		//12-bit resolution
		uint8_t fault_queue			=	00;
		uint8_t	alert_polarity	=	0;
		uint8_t comparator			=	0;
		uint8_t shutdown				=	0;		//Enable
	
	//Merge config reg values into a single byte
		config_reg_data |= (one_shot				<<	7);
		config_reg_data |= (resolution			<<	5);
		config_reg_data |= (fault_queue			<<	3);
		config_reg_data |= (alert_polarity	<<	2);
		config_reg_data	|= (comparator			<<	1);
		config_reg_data	|= (shutdown				<<	0);
		
	//Initialise by writing the config variable to the CONFIG register
		I2C_start(I2C3, TEMP_ADDRESS, I2C_Direction_Transmitter); // start a transmission in Master transmitter mode
		I2C_write(I2C3, TEMP_CONFIG_REG);	//Set device pointer to configuration register
		I2C_write(I2C3, config_reg_data);	//Write the config reg data to the config reg
		I2C_stop(I2C3);
		Delay_Millis(5); //Ensure config values are implemented before continuing
}

/*----------------------------------------------------------------------------
//Reads temperature registers from temp sensor
//Returns the temperature, in Celsius
 *----------------------------------------------------------------------------*/
double	I2C_Temp_Read(void){
	
	double	temperature;
	uint16_t	recievedData[2]	=	{0,0};
	
	//Indicate we'd like to read the temperature register
		I2C_start(I2C3, TEMP_ADDRESS, I2C_Direction_Transmitter);
		I2C_write(I2C3, TEMP_READ_REG);
		I2C_stop(I2C3);

	//Then  open in RX mode and get the data
		I2C_start(I2C3, TEMP_ADDRESS, I2C_Direction_Receiver);
		recievedData[0]	=	I2C_read_ack(I2C3);		//read first byte
		recievedData[1]	=	I2C_read_nack(I2C3);	//read second byte
		I2C_stop(I2C3);	// stop the transmission	
		
	recievedData[1]	|=	recievedData[0]<<8;
	temperature	=	pow(2,-8)	*	(double)recievedData[1];

	return temperature;
}

/*----------------------------------------------------------------------------
//Reads I2C ADC
//Returns the 12-bit reading
 *----------------------------------------------------------------------------*/
uint16_t	I2C_ADC_Read(void){
	
	uint16_t	reading	=	0x0000;

	I2C_start(I2C3, ADC_ADDRESS, I2C_Direction_Receiver);//Open RX comms with the I2C ADC
	reading	|=	I2C_read_ack(I2C3)<<8;	// read first byte
	reading	|=	I2C_read_nack(I2C3);		//read second byte
	I2C_stop(I2C3);	// stop the transmission	

	return	reading;
}

/*----------------------------------------------------------------------------
//Initialises the GPIO device with a set of default paramters
 *----------------------------------------------------------------------------*/
void	I2C_GPIO_Init(void){
	
	//Select config register values
		uint8_t config_reg_data = 0x00;
		uint8_t seqop				=	0;	//Sequential Operation mode bit
		uint8_t disslw			=	0;	//Slew Rate control bit for SDA output
		uint8_t	odr					=	0;	//INT pin as an open-drain output
		uint8_t intpol			=	0;	//Sets the polarity of the INT output pin.

	//Merge config reg values into a single byte
		config_reg_data  = 0x00;
		config_reg_data |= (seqop		<<	5);
		config_reg_data |= (disslw	<<	4);
		config_reg_data |= (odr			<<	2);
		config_reg_data |= (intpol	<<	1);
		
	//Initialise by writing the config variable to the CONFIG register
		I2C_start(I2C3, GPIO_ADDRESS, I2C_Direction_Transmitter); // start a transmission in Master transmitter mode
		I2C_write(I2C3, GPIO_IOCON_REG);	//Set device pointer to configuration register
		I2C_write(I2C3, config_reg_data);	//Write the config reg data to the config reg
		I2C_stop(I2C3);
		Delay_Millis(5);	//Ensure config values are implemented before continuing
}

/*----------------------------------------------------------------------------
//Sets the direction of each of the GPIO pins
//Input: direction byte ( LSB is GP0 ,	MSB is GP7)
//Input:	'1' is INPUT	,	'0' is OUTPUT
 *----------------------------------------------------------------------------*/
void I2C_GPIO_Direction(uint8_t	direction){
	
		I2C_start(I2C3, GPIO_ADDRESS, I2C_Direction_Transmitter); // start a transmission in Master transmitter mode
		I2C_write(I2C3, GPIO_IODIR_REG);	//Set device pointer to IO DIRECTION register
		I2C_write(I2C3, direction);	//Write the byte to the device
		I2C_stop(I2C3);
}

/*----------------------------------------------------------------------------
//Reads the GPIO register of the GPIO device
//Returns: GPIO register byte ( LSB is GP0, MSB is GP7 )
//IMPORTANT: Data is only valid for pins configured as INPUT pins!
 *----------------------------------------------------------------------------*/
uint8_t	I2C_GPIO_Read(void){
	
	uint8_t rxData	=	0x00;
	
	//Indicate we'd like to read the GPIO register
		I2C_start(I2C3, GPIO_ADDRESS, I2C_Direction_Transmitter);
		I2C_write(I2C3, GPIO_GPIO_REG);
		I2C_stop(I2C3);

	//Then  open in RX mode and get the data
		I2C_start(I2C3, GPIO_ADDRESS, I2C_Direction_Receiver);
		rxData	=	I2C_read_nack(I2C3);
		I2C_stop(I2C3);	// stop the transmission
	
	return rxData;
}

/*----------------------------------------------------------------------------
//Writes a byte to GPIO register of the GPIO device
//Input: byteWrite ( LSB is GP0, MSB is GP7 )
//IMPORTANT: Only pins configured as OUTPUT will be effected
 *----------------------------------------------------------------------------*/
void	I2C_GPIO_Write(uint8_t writeByte){
	
	I2C_start(I2C3, GPIO_ADDRESS, I2C_Direction_Transmitter); // start a transmission in Master transmitter mode
	I2C_write(I2C3, GPIO_OLAT_REG);	//Set device pointer to GPIO register
	I2C_write(I2C3, writeByte);	//Write the byte to the device
	I2C_stop(I2C3);
}

