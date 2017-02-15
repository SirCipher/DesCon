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
#include "stm32f4_ds1307.h"

uint8_t DS1307_Months[] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

DS1307_Result_t DS1307_Init(void) {
	/* Initialize clock */
	I2C_Init(DS1307_I2C, DS1307_I2C_PINSPACK, DS1307_I2C_CLOCK);
	
	/* Check if device is connected */
	if (!I2C_IsDeviceConnected(DS1307_I2C, DS1307_I2C_ADDR)) {
		return DS1307_Result_DeviceNotConnected;
	}
	
	/* Return OK */
	return DS1307_Result_Ok;
}

void DS1307_GetDateTime(DS1307_Time_t* time) {
	uint8_t data[7];
	
	/* Read multi bytes */
	I2C_ReadMulti(DS1307_I2C, DS1307_I2C_ADDR, DS1307_SECONDS, data, 7);
	
	/* Fill data */
	time->seconds = DS1307_Bcd2Bin(data[DS1307_SECONDS]);
	time->minutes = DS1307_Bcd2Bin(data[DS1307_MINUTES]);
	time->hours = DS1307_Bcd2Bin(data[DS1307_HOURS]);
	time->day = DS1307_Bcd2Bin(data[DS1307_DAY]);
	time->date = DS1307_Bcd2Bin(data[DS1307_DATE]);
	time->month = DS1307_Bcd2Bin(data[DS1307_MONTH]);
	time->year = DS1307_Bcd2Bin(data[DS1307_YEAR]);
}

void DS1307_SetDateTime(DS1307_Time_t* time) {
	uint8_t data[7];
	
	/* Format data */
	data[DS1307_SECONDS] = DS1307_Bin2Bcd(DS1307_CheckMinMax(time->seconds, 0, 59));
	data[DS1307_MINUTES] = DS1307_Bin2Bcd(DS1307_CheckMinMax(time->minutes, 0, 59));
	data[DS1307_HOURS] = DS1307_Bin2Bcd(DS1307_CheckMinMax(time->hours, 0, 23));
	data[DS1307_DAY] = DS1307_Bin2Bcd(DS1307_CheckMinMax(time->day, 1, 7));
	data[DS1307_DATE] = DS1307_Bin2Bcd(DS1307_CheckMinMax(time->date, 1, 31));
	data[DS1307_MONTH] = DS1307_Bin2Bcd(DS1307_CheckMinMax(time->month, 1, 12));
	data[DS1307_YEAR] = DS1307_Bin2Bcd(DS1307_CheckMinMax(time->year, 0, 99));
	
	/* Write to device */
	I2C_WriteMulti(DS1307_I2C, DS1307_I2C_ADDR, DS1307_SECONDS, data, 7);
}

uint8_t DS1307_GetSeconds(void) {
	return DS1307_Bcd2Bin(I2C_Read(DS1307_I2C, DS1307_I2C_ADDR, DS1307_SECONDS));
}

uint8_t DS1307_GetMinutes(void) {
	return DS1307_Bcd2Bin(I2C_Read(DS1307_I2C, DS1307_I2C_ADDR, DS1307_MINUTES));
}

uint8_t DS1307_GetHours(void) {
	return DS1307_Bcd2Bin(I2C_Read(DS1307_I2C, DS1307_I2C_ADDR, DS1307_HOURS));
}

uint8_t DS1307_GetDay(void) {
	return DS1307_Bcd2Bin(I2C_Read(DS1307_I2C, DS1307_I2C_ADDR, DS1307_DAY));
}

uint8_t DS1307_GetDate(void) {
	return DS1307_Bcd2Bin(I2C_Read(DS1307_I2C, DS1307_I2C_ADDR, DS1307_DATE));
}

uint8_t DS1307_GetMonth(void) {
	return DS1307_Bcd2Bin(I2C_Read(DS1307_I2C, DS1307_I2C_ADDR, DS1307_MONTH));
}

uint8_t DS1307_GetYear(void) {
	return DS1307_Bcd2Bin(I2C_Read(DS1307_I2C, DS1307_I2C_ADDR, DS1307_YEAR));
}


void DS1307_SetSeconds(uint8_t seconds) {
	I2C_Write(DS1307_I2C, DS1307_I2C_ADDR, DS1307_SECONDS, DS1307_Bin2Bcd(DS1307_CheckMinMax(seconds, 0, 59)));
}

void DS1307_SetMinutes(uint8_t minutes) {
	I2C_Write(DS1307_I2C, DS1307_I2C_ADDR, DS1307_MINUTES, DS1307_Bin2Bcd(DS1307_CheckMinMax(minutes, 0, 59)));
}
void DS1307_SetHours(uint8_t hours) {
	I2C_Write(DS1307_I2C, DS1307_I2C_ADDR, DS1307_HOURS, DS1307_Bin2Bcd(DS1307_CheckMinMax(hours, 0, 23)));
}

void DS1307_SetDay(uint8_t day) {
	I2C_Write(DS1307_I2C, DS1307_I2C_ADDR, DS1307_DAY, DS1307_Bin2Bcd(DS1307_CheckMinMax(day, 1, 7)));
}

void DS1307_SetDate(uint8_t date) {
	I2C_Write(DS1307_I2C, DS1307_I2C_ADDR, DS1307_DATE, DS1307_Bin2Bcd(DS1307_CheckMinMax(date, 1, 31)));
}

void DS1307_SetMonth(uint8_t month) {
	I2C_Write(DS1307_I2C, DS1307_I2C_ADDR, DS1307_MONTH, DS1307_Bin2Bcd(DS1307_CheckMinMax(month, 1, 12)));
}

void DS1307_SetYear(uint8_t year) {
	I2C_Write(DS1307_I2C, DS1307_I2C_ADDR, DS1307_YEAR, DS1307_Bin2Bcd(DS1307_CheckMinMax(year, 0, 99)));
}

void DS1307_EnableOutputPin(DS1307_OutputFrequency_t frequency) {
	uint8_t temp;
	if (frequency == DS1307_OutputFrequency_1Hz) {
		temp = 1 << DS1307_CONTROL_OUT | 1 << DS1307_CONTROL_SQWE;
	} else if (frequency == DS1307_OutputFrequency_4096Hz) {
		temp = 1 << DS1307_CONTROL_OUT | 1 << DS1307_CONTROL_SQWE | 1 << DS1307_CONTROL_RS0;	
	} else if (frequency == DS1307_OutputFrequency_8192Hz) {
		temp =	1 << DS1307_CONTROL_OUT |  1 << DS1307_CONTROL_SQWE | 1 << DS1307_CONTROL_RS1;
	} else if (frequency == DS1307_OutputFrequency_32768Hz) {
		temp = 1 << DS1307_CONTROL_OUT |  1 << DS1307_CONTROL_SQWE | 1 << DS1307_CONTROL_RS1 | 1 << DS1307_CONTROL_RS0;	
	} else if (frequency == DS1307_OutputFrequency_High) {
		temp = 1 << DS1307_CONTROL_OUT;	
	} else if (frequency == DS1307_OutputFrequency_Low) {
		temp = 0;
	}
	
	/* Write to register */
	I2C_Write(DS1307_I2C, DS1307_I2C_ADDR, DS1307_CONTROL, temp);
}

void DS1307_DisableOutputPin(void) {
	/* Set output pin to high */
	DS1307_EnableOutputPin(DS1307_OutputFrequency_High);
}

uint8_t DS1307_Bcd2Bin(uint8_t bcd) {
	uint8_t dec = 10 * (bcd >> 4);
	dec += bcd & 0x0F;
	return dec;
}

uint8_t DS1307_Bin2Bcd(uint8_t bin) {
	uint8_t low = 0;
	uint8_t high = 0;
	
	/* High nibble */
	high = bin / 10;
	/* Low nibble */
	low = bin - (high * 10);
	
	/* Return */
	return high << 4 | low;
}

uint8_t DS1307_CheckMinMax(uint8_t val, uint8_t min, uint8_t max) {
	if (val < min) {
		return min;
	} else if (val > max) {
		return max;
	}
	return val;
}

