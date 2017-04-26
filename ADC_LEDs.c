#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "ADC.h"
#include "utility.h"
#include "ringbuffer.h"
#include "board_init.h"
#include "reading.h"
#include "unit_dict.h"
#include <string.h>
#include "siggen.h"

// TODO: rename this file, its not ADC_LEDS, its our project (Not done as it may break keil)
#define ADCONE (float) read_ADC1()
#define ADCTWO (float) read_ADC2()
#define RESISTANCE VOLTAGE/CURRENT
#define VOLTAGE_INPUT_MIN 0
#define VOLTAGE_INPUT_MAX 65536
#define VOLTAGE_OUTPUT_MIN 0
#define VOLTAGE_OUTPUT_MAX 10

#define RESISTANCE_READING reading_get_value(volts) / reading_get_value(amps)
#define LIGHT_READING ADCTWO

#define SCALE_LIMITS 128
// Change this depending on where we want scaling to start
#define DEFAULT_STAGE 0

#define SIGNALCACHESIZE 10000

ringbuffer_t ringbuffer;
uint8_t menuState = 0;
uint8_t scaleState = DEFAULT_STAGE;

reading_t volts;
reading_t amps;
reading_t resistance;
reading_t light;

float *signalCache;

uint8_t sendSignal;
uint8_t board_is_reading = 0;
uint8_t current_mode = 0;
uint8_t startup = 1;
uint8_t set_selection = 0;
uint8_t menu_confirm_exit = 0;

char *menuItems[] = {
	"Voltage",
	"Current",
	"Resistance",
	"Light",
	"Continuinity",
	"Transistor",
	"Diode",
	"Capacitor",
	"Inductor",
	"Rms",
	"Frequency",
	"Sig Gen"
};

int menuCount = 12;

/*----------------------------------------------------------------------------
Returns the correct units based on menuState
*----------------------------------------------------------------------------*/
char *get_units(uint8_t menuState, uint8_t scaleState) {
	return "\0";
}

/*----------------------------------------------------------------------------
Reads values from pins and stores them in reading
*----------------------------------------------------------------------------*/
void read_value() {
	if (menuState < 3) {
			reading_set_value(volts, scale(ADCONE, VOLTAGE_INPUT_MIN, VOLTAGE_INPUT_MAX, 
												VOLTAGE_OUTPUT_MIN, VOLTAGE_OUTPUT_MAX));
			reading_set_value(amps, scale(ADCTWO, VOLTAGE_INPUT_MIN, VOLTAGE_INPUT_MAX, 
												VOLTAGE_OUTPUT_MIN, VOLTAGE_OUTPUT_MAX));
	} else if (menuState == 3) {
			reading_set_value(light, scale(LIGHT_READING, VOLTAGE_INPUT_MIN, VOLTAGE_INPUT_MAX,
												VOLTAGE_OUTPUT_MIN,VOLTAGE_OUTPUT_MAX));
	}
}

void lcd_output_value(char *memory, reading_t reading, int *last_write_length1, int *last_write_length2) {
	reading_get_lcd_string(reading, memory);
	lcd_write_string(memory, 1, 0, last_write_length1);
	// lcd_write_string(unit, 0, 0, last_write_length2);
}

void bt_output_value(reading_t reading, char *memory) {
// TODO: change this to follow data pattern

//	sprintf(memory, "<%.4f> %s",value,unit);
	reading_get_message_form(reading, memory);
	send_String(USART3, memory);
	printf("%s\n", memory);
}

int is_continuity(int val) {
	return val > CONTINUITY_RAW_CAP;
}

// TODO: SCALE DOWN AND UP
int auto_scale_hardware(int rawValue) {
	// Will limitting these be required?
	return 0;
}

void transform_scale_to_hardware_pins() {
	// scaleStates -> pins, agree with Hardware on what these need to be
}

reading_t get_display_lcd_reading(uint8_t menuState) {
	if (menuState == 0) {
		return volts;
	} else if (menuState == 1) {
		return amps;
	} else if (menuState == 2) {
		reading_set_value(resistance, RESISTANCE_READING);
		reading_set_scale(resistance, reading_get_scale(volts) + reading_get_scale(amps));
		return resistance;
	} else if (menuState == 3) {
		return light;
	}
	return NULL;
}

void outputSine() {
	int i = 0;
	reading_t ready_yates = reading_new(0, 'A', 0);
	char *willy = malloc(sizeof(char) * 100);

	while (menu_confirm_exit) {
		//GPIOE->PINNAME = sine[i++];
		reading_set_value(ready_yates, signalCache[i++]);
		printf("%.4f\n", signalCache[i]);
		i %= SIGNALCACHESIZE;
	}
	free(willy);
}

void check_string_set_mode(char rx_buffer[]) {
	if (strcmp(rx_buffer, STRING_AMPS) == 0) current_mode = MODE_CURRENT;
	if (strcmp(rx_buffer, STRING_VOLTAGE) == 0) current_mode = MODE_VOLTAGE;
	if (strcmp(rx_buffer, STRING_RESISTANCE) == 0) current_mode = MODE_RESISTANCE;
	if (strcmp(rx_buffer, STRING_LIGHT) == 0) current_mode = MODE_LIGHT;
	if (strcmp(rx_buffer, STRING_CONTINUITY) == 0) current_mode = MODE_CONTINUITY;
	if (strcmp(rx_buffer, STRING_TRANSISTOR) == 0) current_mode = MODE_TRANSISTOR;
	if (strcmp(rx_buffer, STRING_DIODE) == 0) current_mode = MODE_DIODE;
	if (strcmp(rx_buffer, STRING_CAPACITOR) == 0) current_mode = MODE_CAPACITOR;
	if (strcmp(rx_buffer, STRING_INDUCTOR) == 0) current_mode = MODE_INDUCTOR;
	if (strcmp(rx_buffer, STRING_RMS) == 0) current_mode = MODE_RMS;
	if (strcmp(rx_buffer, STRING_FREQUENCY) == 0) current_mode = MODE_FREQUENCY;
	if (strcmp(rx_buffer, STRING_TOGGLE) == 0) {
			if (menu_confirm_exit) menu_confirm_exit = 0;
			else menu_confirm_exit = 1;
	}
}

void adc_reading(uint8_t mode) {
	char *string_memory = (char *) malloc(64 * sizeof(char));

	int last_write_length1 = 16, last_write_length2 = 16;
	int delta_scale = 0;
	reading_t reading;

	while (menu_confirm_exit) {
		read_value();
		reading = get_display_lcd_reading(mode);
		delta_scale = reading_need_scale(reading, VOLTAGE_OUTPUT_MAX, VOLTAGE_OUTPUT_MIN);
		/*
			 We only scale the reading we're displaying
			 this should hopefully allow for faster switching (as hopefully the scale wont change)
		*/
		while (delta_scale && 0) {
			reading_set_scale(reading, reading_get_scale(reading) + delta_scale);
			delta_scale = reading_need_scale(reading, VOLTAGE_OUTPUT_MAX, VOLTAGE_OUTPUT_MIN);
			continue;
		}
		if (mode < 3) {
			bt_output_value(volts, string_memory);
			bt_output_value(amps, string_memory);
		} else {
			bt_output_value(light, string_memory);
		}
		lcd_output_value(string_memory, reading, &last_write_length1, &last_write_length2);
	}
	free(string_memory);
}

void menu() {
	int last_write_length1 = 16, last_write_length2 = 16;

	if(startup) {
		display_startup_message();
		startup =0;
	}
	
	lcd_write_string("Choose a mode", 0, 0, &last_write_length1);
	send_String(USART3, "Choose a mode");

	set_leds();
	
	while (!menu_confirm_exit) {
		lcd_write_string(menuItems[menuState], 1, 0, &last_write_length1);
		Delay(500);
	}
	
	set_leds();
	
	lcd_clear_display();
	while (menu_confirm_exit) {
		if (menuState < 4) {
				adc_reading(menuState);
		} else {
				if (menuState == 4) {
					set_mux(10);
					continuity();
				}
		}
	}
}

void set_leds(){
	if(!menu_confirm_exit){
		light_led(0x89);
	} else{
		light_led(0x8);
	}
}

void set_mux(uint8_t mux) {
    // GPIOX->PINNAME = mux; // || do da funky m4f
}

void set_buzz(uint8_t buzz) {
    //GPIOX->PINNAME = buzz>0;
}

void continuity() {
	while (menu_confirm_exit) {
			while (is_continuity(ADCONE)) set_buzz(1);
			set_buzz(0);
	}
}

void clear_led(){
	GPIOD->ODR &= 0x00FF;
}

void light_led(uint8_t led) {
	GPIOD->ODR &= 0x00FF;
	GPIOD->ODR |= (led << 8);
}

void flash_led(uint8_t led){
	
	for(int i =0;i < 32; i++){
		GPIOD->ODR &= 0x00FF;
		GPIOD->ODR |= (led << 8);
		Delay(300);
	}
	
	GPIOD->ODR &= 0x00FF;
}

int main(void) {
	volts = reading_new(0, 'V', 0);
	amps = reading_new(0, 'A', 0);
	light = reading_new(0, 'L', 0);
	resistance = reading_new(0, 'O', 0);

	init_board();

	while (1) {
			menu();
	}
}
