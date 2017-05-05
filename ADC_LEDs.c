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
#include "freq_meter.h"


#define ADCONE (float) read_ADC1()
#define ADCTWO (float) read_ADC2()
#define RESISTANCE VOLTAGE/CURRENT
#define VOLTAGE_INPUT_MIN 0
#define VOLTAGE_INPUT_MAX 65536
#define VOLTAGE_OUTPUT_MIN 0
#define VOLTAGE_OUTPUT_MAX 6.64

#define RESISTANCE_READING reading_get_value(volts) / reading_get_value(amps)
#define LIGHT_READING ADCTWO

/* Raw limits to our scales */
#define SCALE_LIMIT_LOWER 128
#define SCALE_LIMIT_UPPER 65408


#define SIGNALCACHESIZE 10000

/* ringbuffer for our input */
ringbuffer_t ringbuffer;

/* Menu State for current choice*/
uint8_t menuState = 0;

/* Boolean for turning autoscaling on and off */
uint8_t autoscale = 1;

/*******************************************************************
 * Declare all the readings we'll be using data will be stored in
 * different areas to allow for quicker switching as the scale
 * will be stored (and therefore recalled) with the reading
 *******************************************************************/
reading_t volts;
reading_t amps;
reading_t resistance;
reading_t light;

/******************************************************************
 * Signal Cache (array of floats) store the generated values
 * of the chosen waveform, frequency, and amplitude which
 * would be looped through. Reduces repeated computing
 ******************************************************************/
float *signalCache;

uint8_t sendSignal;
uint8_t startup = 1;
uint8_t set_selection = 0;
uint8_t board_is_reading = 0;
uint8_t menu_confirm_exit = 0;
int8_t bt_change_mode = -1;

/**********************************************************
 * Menu items are stored as an array to allow for easier
 * modification. menuCount must be set seperately due
 * to how arrays are passed to functions within C.
 **********************************************************/
char *menuItems[] = {
        "Voltage",
        "Current",
        "Resistance",
        "Light",
        "Frequency",
        "AC->DC",
        "Inductance",
        "Capacitance",
        "Continuinity",
        "Transistor",
        "Diode",
        "Rms",
        //"Capacitor",
        //"Inductor",
        //"Sig Gen"
};
int menuCount = 12;


/********************************************************************
 * Reads the input from the hardware and scales to correct
 * value. Stores data within the correct reading struct
 * if required. Only records data within four modes.
 *********************************************************************/
void read_value(int mode) {
    switch (menuState) {
        case 0:
            reading_set_value(volts, scale(ADCONE, VOLTAGE_INPUT_MIN, VOLTAGE_INPUT_MAX,
                                           VOLTAGE_OUTPUT_MIN, VOLTAGE_OUTPUT_MAX));
            break;
        case 1:
            reading_set_value(amps, scale(ADCONE, VOLTAGE_INPUT_MIN, VOLTAGE_INPUT_MAX,
                                          VOLTAGE_OUTPUT_MIN, VOLTAGE_OUTPUT_MAX));
            break;
        case 2:
            reading_set_value(amps, scale(ADCONE * RESISTANCE_MULTIPLIER, VOLTAGE_INPUT_MIN, VOLTAGE_INPUT_MAX,
                                          VOLTAGE_OUTPUT_MIN, VOLTAGE_OUTPUT_MAX));
            break;
        case 3:
            reading_set_value(light, scale(ADCONE, VOLTAGE_INPUT_MIN, VOLTAGE_INPUT_MAX,
                                           VOLTAGE_OUTPUT_MIN, VOLTAGE_OUTPUT_MAX));
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            break;
        default:

            break;
    }
}



/************************************************************************
 * Functions to help output to the LCD and bluetooth respectively in the
 * correct format, full string for LCD & compact for bt. Pointers are
 * passed to lcd_output_value for more efficient screen updating.
 ***********************************************************************/
void lcd_output_value(char *memory, reading_t reading, int *last_write_length1, int *last_write_length2) {
    reading_get_lcd_string(reading, memory);
    lcd_write_string(memory, 1, 0, last_write_length1);
}

void bt_output_value(reading_t reading, char *memory) {
    reading_get_message_form(reading, memory);
    send_String(USART3, memory);
    printf("%s\n", memory);
}

/*********
 *
 *
 */

int is_continuity(int val) {
    return val > CONTINUITY_RAW_CAP;
}

int auto_scale_hardware(int rawValue) {
    if (rawValue > SCALE_LIMIT_UPPER) {
        return 1;
    } else if (rawValue < SCALE_LIMIT_LOWER) {
        return -1;
    }
    return 0;
}


/*****************************************************
 * outputs the scale value to the to the correct pins
 * which outputs to the board to scale the values correctly
 * should only be called from modifyScale
 ****************************************************/
void setScalePins(int scale) {
    int b0 = scale & 1;
    int b1 = (scale & 2) >> 1;
    GPIOE->ODR &= ~(1 << PIN_SCALE_BIT0 | 1 << PIN_SCALE_BIT1 );
    GPIOE->ODR |= (b0 << PIN_SCALE_BIT0 | b1 << PIN_SCALE_BIT1 );
}


/*************************************************
 * Directly sets the scale of the the hardware
 * to the scale of the last reading (which may be 0)
 */
void setScale(reading_t reading, int new_scale) {
    if (new_scale > 3 && !~new_scale) {
        reading_set_scale(reading, new_scale);
        setScalePins(new_scale);
    }
}

/*****************************************************
 * Modifies the scale by delta_scale of the current reading
 * but keeps it within the range of 0-3 the scaling is non-linear,
 * and is handled separately by software, hardware and bt device
 */
void modifyScale(reading_t reading, int delta_scale) {
    int currentScale = reading_get_scale(reading);
    int scaleSum = currentScale + delta_scale;
    setScale(reading,scaleSum);
}


/***
 * returns the correct reading struct based on menu state
 * returns null if no reading state is required
 */
reading_t get_display_lcd_reading(uint8_t menuState) {
    if (menuState == 0) {
        return volts;
    } else if (menuState == 1) {
        return amps;
    } else if (menuState == 2) {
        return resistance;
    } else if (menuState == 3) {
        return light;
    }
    return NULL;
}


/************************************
 * changes the mode of the DMM based on the string received from BT
 */
void check_string_set_mode(char command[]) {
    int new_mode;
    if (strcmp(command, STRING_AMPS) == 0) new_mode = MODE_CURRENT;
    else if (strcmp(command, STRING_VOLTAGE) == 0) new_mode = MODE_VOLTAGE;
    else if (strcmp(command, STRING_RESISTANCE) == 0) new_mode = MODE_RESISTANCE;
    else if (strcmp(command, STRING_LIGHT) == 0) new_mode = MODE_LIGHT;
    else if (strcmp(command, STRING_CONTINUITY) == 0) new_mode = MODE_CONTINUITY;
    else if (strcmp(command, STRING_TRANSISTOR) == 0) new_mode = MODE_TRANSISTOR;
    else if (strcmp(command, STRING_DIODE) == 0) new_mode = MODE_DIODE;
    else if (strcmp(command, STRING_CAPACITOR) == 0) new_mode = MODE_CAPACITOR;
    else if (strcmp(command, STRING_INDUCTOR) == 0) new_mode = MODE_INDUCTOR;
    else if (strcmp(command, STRING_RMS) == 0) new_mode = MODE_RMS;
    else if (strcmp(command, STRING_FREQUENCY) == 0) new_mode = MODE_FREQUENCY;
    else if (strcmp(command, STRING_TOGGLE) == 0) {
        menu_confirm_exit = !menu_confirm_exit;
    }

    /* Ignore menu changes if the current mode is the same */
    if (menuState != new_mode || !menu_confirm_exit) {
        bt_change_mode = new_mode;
        /* Confirm that the string was passed and accepted by the DMM */
        char *string_memory = (char *) malloc(17 * sizeof(char));
        sprintf(string_memory, "%s", menuItems[bt_change_mode]);
        send_String(USART3, string_memory);
        free(string_memory);
    }

}

/**********************************************************************************
 * Handles the non-menu mode of the DMM, stays within this function whilst
 * no mode change is requested
 */

void adc_reading(uint8_t mode) {
    char *string_memory = (char *) malloc(64 * sizeof(char));
    /*
     * Setting both last_write_length1 and 2 to 16 (maximum string length of LCD)
     * forces a line clear on the next write.
     */
    int last_write_length1 = 16, last_write_length2 = 16;
    int delta_scale = 0;
    reading_t reading;

    while (change_requested()) {
        reading = get_display_lcd_reading(mode);
        read_value(mode);
        delta_scale = reading_need_scale(reading, VOLTAGE_OUTPUT_MAX, VOLTAGE_OUTPUT_MIN);
        /*
             We only scale the reading we're displaying
             this should hopefully allow for faster switching (as hopefully the scale wont change)
        */
        if (autoscale) {
            if (delta_scale) {
                modifyScale(reading, delta_scale);
                Delay(100);
                continue;
            }
        }
        switch (menuState) {
            case 0:
            case 1:
            case 2:
            case 3:
                bt_output_value(reading,string_memory);
                lcd_output_value(string_memory, reading, &last_write_length1, &last_write_length2);
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:
                break;
            case 8:
                break;
            default:

                break;
        }
    }

    free(string_memory);
}

int frequency() {

}

float time_period() {

}


void capacitance() {
    float capacitance = time_period() / 1.1 * pow((double) 10, -4);
    char* str = malloc(sizeof(char) * 20);

    sprintf(str,"%.4f",capacitance);
    send_String(USART3, str);

    free(str);
}

void inductance() {
    float inductance_reading = 16500 / frequency();
    char* str = malloc(sizeof(char) * 20);
    sprintf(str,"%.4f", inductance_reading);
    send_String(USART3, str);
    free(str);
}


int change_requested() {
    return menu_confirm_exit && !~bt_change_mode;
}

void menu() {
    int last_write_length1 = 16, last_write_length2 = 16;

    if (startup) {
        display_startup_message();
        startup = 0;
    }

    set_leds();
    /* stay in menu state */
    while (!change_requested()) {
        /* if bt_change_mode is not -1, change the mode to what it is*/
        if (~bt_change_mode) {
            menuState = bt_change_mode;
            bt_change_mode = -1;
            menu_confirm_exit = 1;
        }
        lcd_write_string("Choose a mode", 0, 0, &last_write_length1);
        lcd_write_string(menuItems[menuState], 1, 0, &last_write_length1);
        Delay(500);
    }

    set_leds();
    app_set_mode(menuState);
    lcd_clear_display();

    /* stay in non menu state state */
    while (change_requested()) {

        if (menuState < 4) {
            set_mux(menuState);
            adc_reading(menuState);
        } else {
            if (menuState == 7) {
                capacitance();
            }
            if (menuState > 7 && menuState < 11) {
                set_mux(menuState);
                continuity();
            }
        }
    }
}

void set_leds() {
    if (!menu_confirm_exit) {
        light_led(0x89);
    } else {
        light_led(0x8);
    }
}

void continuity() {
    int last_write_length1 = 16, last_write_length2 = 16;

    while (change_requested()) {
        if (is_continuity(ADCONE)) {
            lcd_write_string("Continuity", 0, 0, &last_write_length1);
            lcd_write_string("detected", 1, 0, &last_write_length1);
            send_String(USART3, "Continuity detected");
            set_buzz(1);
        } else {
            lcd_clear_display();
            send_String(USART3, "Continuity not detected");
            set_buzz(0);
        }
    }
    set_buzz(0);
}

void clear_led() {
    GPIOD->ODR &= 0x00FF;
}

void light_led(uint8_t led) {
    GPIOD->ODR &= 0x00FF;
    GPIOD->ODR |= (led << 8);
}

void flash_led(uint8_t led) {
    for (int i = 0; i < 32; i++) {
        GPIOD->ODR &= 0x00FF;
        GPIOD->ODR |= (led << 8);
        Delay(300);
    }

    GPIOD->ODR &= 0x00FF;
}

void set_mux(uint8_t mux) {
    int b0 = mux & 0x1;
    int b1 = (mux & 0x2) >> 1;
    int b2 = (mux & 0x4) >> 2;

    //GPIOA->ODR &= ~(1<<13);
    //GPIOA->ODR |= (1<<PIN_MUX_BIT0);

//    GPIOB->ODR = 0xffff;
    GPIOB->ODR &= ~((1 << PIN_MUX_BIT0) | (1 << PIN_MUX_BIT1) | (1 << PIN_MUX_BIT2));
    GPIOB->ODR |= ((b0 << PIN_MUX_BIT0) | (b1 << PIN_MUX_BIT1) | (b2 << PIN_MUX_BIT2));
//    GPIOC->ODR |= mux << 4;
}

void set_buzz(uint8_t buzz) {
    if (buzz) {
        GPIOA->ODR &= ~(1 << 13);
        GPIOA->ODR |= (1 << 6);
    } else {
        GPIOA->ODR &= ~(1 << 6);
    }
}

void mux_test() {
    for (int i = 0; i < 8; i++) {
        GPIOC->ODR &= (1 << 13);
        set_mux(i);
        Delay(500);
    }
}

void buzz_test() {
    set_buzz(1);
    Delay(1000);
    set_buzz(0);
}

void test_board() {
    //mux_test();
    buzz_test();
}

int main(void) {
    volts = reading_new(0, 'V', 0);
    amps = reading_new(0, 'A', 0);
    light = reading_new(0, 'L', 0);
    resistance = reading_new(0, 'O', 0);

    init_board();

    while (1) {
        //freq_meter();
        menu();
    }
}
