#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "ADC.h"
#include "utility.h"
#include "ringbuffer.h"
#include <string.h>

// TODO: rename this file, its not ADC_LEDS, its our project (Not done as it may break keil)
#define VOLTAGE (float) read_ADC1()
#define CURRENT (float) read_ADC2()
#define RESISTANCE VOLTAGE/CURRENT
#define VOLTAGE_INPUT_MIN 0
#define VOLTAGE_INPUT_MAX 65536
#define VOLTAGE_OUTPUT_MIN 0
#define VOLTAGE_OUTPUT_MAX 10

#define SCALE_LIMITS 128
// Change this depending on where we want scaling to start
#define DEFAULT_STAGE 2 



ringbuffer_t ringbuffer;
unsigned int state = 1;
unsigned int scaleState = DEFAULT_STAGE;

// TODO: (Re)Move this?
/*----------------------------------------------------------------------------
  Function that read Button pins
 *----------------------------------------------------------------------------*/
uint32_t BTN_Get(void) {

    return (GPIOA->IDR & (1UL << 0));
}

/*----------------------------------------------------------------------------
  Checks buttons and returns whether the state has changed or not
 *----------------------------------------------------------------------------*/
int check_state_changed(unsigned int *state) {
    for (int i = 0; i < 3; i++) {
        if (SWT_Check(i) && *state != i + 1) {
            *state = i + 1;
            return 1;
        }
    }
    return 0;
}

/*----------------------------------------------------------------------------
  Returns the correct units based on state
 *----------------------------------------------------------------------------*/
char *get_units(int state, int scaleState) {
		char* prefix  = malloc(10*sizeof(prefix));
		switch(scaleState){
			case 0: 
				prefix = "micro";
				break;
			case 1:
				prefix = "milli";
				break;
			case 2:
				prefix = "";
				break;
			case 3:
				prefix = "kilo";
				break;
			case 4:
				prefix = "mega";
				break;
		}
		// note: we may want to make a new struct to store the prefix and the units seperately
		// this will mean we can transform them later to things like mV or whatever easier
		// dependant on context
    switch (state) {
        case 1:
            return strcat(prefix,"Volts");
        case 2:
            return strcat(prefix,"Amps");
        case 3:
            return strcat(prefix,"Ohms");
    }
    return "\0";
}

/*----------------------------------------------------------------------------
  Returns the correct value dependant on state
 *----------------------------------------------------------------------------*/
float read_value(int state) {
    switch (state) {
        case 1:
            return VOLTAGE;
        case 2:
            return CURRENT;
        case 3:
            return RESISTANCE;
        default:
            return 0;
    }
}

void lcd_output_value(char* memory,float value, char *unit, int *last_write_length1, int *last_write_length2){
    sprintf(memory, "%.2f", value);

    lcd_write_string(memory, 1, 0, last_write_length1);
    lcd_write_string(unit, 0, 0, last_write_length2);
}

void bt_output_value(char* memory, float value, char *unit){
    // TODO: change this to follow data pattern
		
	sprintf(memory, "<%.4f> %s",value,unit);

	send_String(USART3, memory);
}



void display_startup_message() {
    int last_write_length1 = 0, last_write_length2 = 0;
    lcd_clear_display();
    lcd_write_string("You're a", 0, 0, &last_write_length1);
    lcd_write_string("Multimeter Harry", 1, 0, &last_write_length2);
		send_String(USART3, "You're a multimeter, Harry");
    Delay(1000);
    lcd_clear_display();
}

void auto_scale_hardware(int rawValue){
	// Will limitting these be required?
	if(rawValue < SCALE_LIMITS){
		scaleState++;
		//write to pin increase
		// transform_scale_to_hardware_pins()
	}
	else if(rawValue > (VOLTAGE_INPUT_MAX-SCALE_LIMITS)){
		scaleState--;
		//write to pin decrease
		// transform_scale_to_hardware_pins()
	}
}

void transform_scale_to_hardware_pins(){
	// scaleStates -> pins, agree with Hardware on what these need to be
}

void setState(char *mode){
	if(strcmp(mode, "Volts") == 0){
		state = 1;
	} else if(strcmp(mode, "Amps") == 0){
		state = 2;
	} else if(strcmp(mode, "Resistance") == 0){
		state = 3;
	}
}


/*----------------------------------------------------------------------------
  Function shows current/voltage/resistance
 *----------------------------------------------------------------------------*/
void main_loop(void) {
    char *string_memory = (char *) malloc(100 * sizeof(char));

    float value = 0;
    float rawValue = 0;
    char *unit = malloc(6 * sizeof(char)); // Max word length + 1 for null char (possible words Volts, Amps, Ohms)
    int last_write_length1 = 0, last_write_length2 = 0;
    unit = "Volts";

    while (1) {
        rawValue = read_value(state);
				auto_scale_hardware(rawValue);
				
        unit = get_units(state,scaleState);
        value = scale(rawValue, VOLTAGE_INPUT_MIN, VOLTAGE_INPUT_MAX, VOLTAGE_OUTPUT_MIN, VOLTAGE_OUTPUT_MAX);

        lcd_output_value(string_memory,value,unit, &last_write_length1,&last_write_length2);
        bt_output_value(string_memory,value,unit);
    }
}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main(void) {
  init_board(); 
  display_startup_message();
	main_loop();
}
