#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "ADC.h"
#include "utility.h"
#include "ringbuffer.h"
#include "reading.h"
#include "unit_dict.h"
#include <string.h>
#include "siggen.h"

// TODO: rename this file, its not ADC_LEDS, its our project (Not done as it may break keil)
#define VOLTAGE (float) read_ADC1()
#define CURRENT (float) read_ADC2()
#define RESISTANCE VOLTAGE/CURRENT
#define VOLTAGE_INPUT_MIN 0
#define VOLTAGE_INPUT_MAX 65536
#define VOLTAGE_OUTPUT_MIN 0
#define VOLTAGE_OUTPUT_MAX 10

#define RESISTANCE_READING reading_get_value(volts) / reading_get_value(amps)

#define SCALE_LIMITS 128
// Change this depending on where we want scaling to start
#define DEFAULT_STAGE 0


ringbuffer_t ringbuffer;
unsigned int state = 1;
unsigned int scaleState = DEFAULT_STAGE;
reading_t volts;
reading_t amps;
reading_t resistance;

float* sine;
int sendSignal;

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
    return "\0";
}

/*----------------------------------------------------------------------------
  Reads values from pins and stores them in reading
 *----------------------------------------------------------------------------*/
void read_value() {
    reading_set_value(volts,
                      scale(VOLTAGE, VOLTAGE_INPUT_MIN, VOLTAGE_INPUT_MAX, VOLTAGE_OUTPUT_MIN, VOLTAGE_OUTPUT_MAX));
    reading_set_value(amps,
                      scale(CURRENT, VOLTAGE_INPUT_MIN, VOLTAGE_INPUT_MAX, VOLTAGE_OUTPUT_MIN, VOLTAGE_OUTPUT_MAX));
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
}


void display_startup_message() {
    int last_write_length1 = 0, last_write_length2 = 0;
    lcd_clear_display();
    lcd_write_string("Digital", 0, 0, &last_write_length1);
    lcd_write_string("Multimeter", 1, 0, &last_write_length2);
    send_String(USART3, "Digital Multimeter");
    Delay(1000);
    lcd_clear_display();
}


// TODO: SCALE DOWN AND UP
int auto_scale_hardware(int rawValue) {
    // Will limitting these be required?

}

void transform_scale_to_hardware_pins() {
    // scaleStates -> pins, agree with Hardware on what these need to be
}


reading_t get_display_lcd_reading(int state) {
    if (state == 0) {
        return volts;
    } else if (state == 1) {
        return amps;
    } else if (state == 2) {
        reading_set_value(resistance, RESISTANCE_READING);
        reading_set_scale(resistance, reading_get_scale(volts) + reading_get_scale(amps));
        return resistance;
    }
    return NULL;
}

void outputSine(){
    int i = 0;
    while(sendSignal){
        //GPIOE->PINNAME = sine[i++];
        i%=10000;
    }
}

/*----------------------------------------------------------------------------
  Function shows current/voltage/resistance
 *----------------------------------------------------------------------------*/
void main_loop(void) {
    char *string_memory = (char *) malloc(100 * sizeof(char));

//    char *unit = malloc(6 * sizeof(char)); // Max word length + 1 for null char (possible words Volts, Amps, Ohms)
    int last_write_length1 = 0, last_write_length2 = 0;
    int delta_scale = 0;
    reading_t reading;

    while (1) {
        read_value();
        reading - get_display_lcd_reading(state);
        delta_scale = reading_need_scale(reading, VOLTAGE_OUTPUT_MAX, VOLTAGE_OUTPUT_MIN);
         /*
            We only scale the reading we're displaying
            this should hopefully allow for faster switching (as hopefully the scale wont change)
        */
        while (delta_scale) {
            reading_set_scale(reading, reading_get_scale(reading) + delta_scale);
            delta_scale = reading_need_scale(reading, VOLTAGE_OUTPUT_MAX, VOLTAGE_OUTPUT_MIN);
            continue;
        }

        bt_output_value(volts, string_memory);
        bt_output_value(amps, string_memory);
        lcd_output_value(string_memory, reading, &last_write_length1, &last_write_length2);

    }
}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main(void) {
    volts = reading_new(0, 'V', 0);
    amps = reading_new(0, 'A', 0);
    resistance = reading_new(0, 'O', 0);
    // Cache Sine signal values
    sine = generate_sin(10,10000); // tweak
    init_board();
    display_startup_message();
    main_loop();
}
