#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "ADC.h"
#include "utility.h"

// TODO: rename this file, its not ADC_LEDS, its our project (Not done as it may break keil)
#define VOLTAGE (float) read_ADC1()
#define CURRENT (float) read_ADC2()
#define RESISTANCE VOLTAGE/CURRENT
#define VOLTAGE_INPUT_MIN 0
#define VOLTAGE_INPUT_MAX 65536
#define VOLTAGE_OUTPUT_MIN 0
#define VOLTAGE_OUTPUT_MAX 10

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
char *get_units(int state) {
    switch (state) {
        case 1:
            return "Volts";
        case 2:
            return "Amps";
        case 3:
            return "Ohms";
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
    sprintf(memory, "%s %s", memory, unit);
    send_String(USART3, memory);
}

/*----------------------------------------------------------------------------
  Function shows current/voltage/resistance
 *----------------------------------------------------------------------------*/
void main_loop(void) {
    char *string_memory = (char *) malloc(100 * sizeof(char));
    unsigned int state = 1;
    float value = 0;
    float rawValue = 0;
    char *unit = malloc(6 * sizeof(char)); // Max word length + 1 for null char (possible words Volts, Amps, Ohms)
    int last_write_length1 = 0, last_write_length2 = 0;
    unit = "Volts";

    while (1) {
        if (check_state_changed(&state)) { // We should only be changing units if the state has changed
            unit = get_units(state);
        }
        rawValue = read_value(state);
        value = scale(rawValue, VOLTAGE_INPUT_MIN, VOLTAGE_INPUT_MAX, VOLTAGE_OUTPUT_MIN, VOLTAGE_OUTPUT_MAX);

        lcd_output_value(string_memory,value,unit, &last_write_length1,&last_write_length2);
        bt_output_value(string_memory,value,unit);
    }
}

void display_startup_message() {
    int last_write_length1 = 0, last_write_length2 = 0;
    lcd_clear_display();
    lcd_write_string("You're a", 0, 0, &last_write_length1);
    lcd_write_string("Multimeter Harry", 1, 0, &last_write_length2);

    Delay(1000);
    lcd_clear_display();
}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main(void) {
    init_board(); //
    display_startup_message();
    main_loop();
}
