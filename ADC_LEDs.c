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

// TODO: rename this file, its not ADC_LEDS, its our project (Not done as it may break keil)
#define ADCONE (float) read_ADC1()
#define ADCTWO (float) read_ADC2()
#define RESISTANCE VOLTAGE/CURRENT
#define VOLTAGE_INPUT_MIN 0
#define VOLTAGE_INPUT_MAX 65536
#define VOLTAGE_OUTPUT_MIN 0
#define VOLTAGE_OUTPUT_MAX 6.64

#define RESISTANCE_READING reading_get_value(volts) / reading_get_value(amps)
#define LIGHT_READING ADCTWO

#define SCALE_LIMIT_LOWER 128
#define SCALE_LIMIT_UPPER 65408
// Change this depending on where we want scaling to start
#define DEFAULT_STAGE 0

#define SIGNALCACHESIZE 10000

ringbuffer_t ringbuffer;
uint8_t menuState = 0;
uint8_t scaleState = DEFAULT_STAGE;

uint8_t autoscale = 0;

reading_t volts;
reading_t amps;
reading_t resistance;
reading_t light;

reading_t last_reading;

float *signalCache;

uint8_t sendSignal;
uint8_t startup = 1;
uint8_t set_selection = 0;
uint8_t board_is_reading = 0;
uint8_t menu_confirm_exit = 0;
int8_t bt_change_mode = -1;

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

/*----------------------------------------------------------------------------
Returns the correct units based on menuState
*----------------------------------------------------------------------------*/
char *get_units(uint8_t menuState, uint8_t scaleState) {
    return "\0";
}

/*----------------------------------------------------------------------------
Reads values from pins and stores them in reading
*----------------------------------------------------------------------------*/
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
//    } else if (menuState == 3) {
//        reading_set_value(light, scale(LIGHT_READING, VOLTAGE_INPUT_MIN, VOLTAGE_INPUT_MAX,
//                                       VOLTAGE_OUTPUT_MIN, VOLTAGE_OUTPUT_MAX));
//    }
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
    if (rawValue > SCALE_LIMIT_UPPER) {
        return 1;
    } else if (rawValue < SCALE_LIMIT_LOWER) {
        return -1;
    }
    return 0;
}


void setScalePins(int scale) {
    int b0 = scale & 1;
    int b1 = (scale & 2) >> 1;
    GPIOE->ODR &= ~(1<<3 | 1<<4);
    GPIOE->ODR |= (b0<<3 | b1 <<4 );
}

void modifyScale(reading_t reading, int delta_scale) {
    int currentScale = reading_get_scale(reading);
    int scaleSum = currentScale + delta_scale;
    if (scaleSum > 3 && !~scaleSum) { // captures limits of 4 and -1
        reading_set_scale(reading, scaleSum);
        setScalePins(scaleSum);
    }
}

void setScale(reading_t reading, int new_scale) {
    if (new_scale > 3 && !~new_scale) {
        reading_set_scale(reading, new_scale);
        setScalePins(new_scale);
    }
}

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

void outputSine() {
    int i = 0;
    reading_t ready_yates = reading_new(0, 'A', 0);
    char *willy = (char *) malloc(sizeof(char) * 12);

    while (menu_confirm_exit) {
        //GPIOE->PINNAME = sine[i++];
        reading_set_value(ready_yates, signalCache[i++]);
        printf("%.4f\n", signalCache[i]);
        i %= SIGNALCACHESIZE;
    }
    free(willy);
}

void check_string_set_mode(char ringbuffer[]) {
    int new_mode;
    if (strcmp(ringbuffer, STRING_AMPS) == 0) new_mode = MODE_CURRENT;
    else if (strcmp(ringbuffer, STRING_VOLTAGE) == 0) new_mode = MODE_VOLTAGE;
    else if (strcmp(ringbuffer, STRING_RESISTANCE) == 0) new_mode = MODE_RESISTANCE;
    else if (strcmp(ringbuffer, STRING_LIGHT) == 0) new_mode = MODE_LIGHT;
    else if (strcmp(ringbuffer, STRING_CONTINUITY) == 0) new_mode = MODE_CONTINUITY;
    else if (strcmp(ringbuffer, STRING_TRANSISTOR) == 0) new_mode = MODE_TRANSISTOR;
    else if (strcmp(ringbuffer, STRING_DIODE) == 0) new_mode = MODE_DIODE;
    else if (strcmp(ringbuffer, STRING_CAPACITOR) == 0) new_mode = MODE_CAPACITOR;
    else if (strcmp(ringbuffer, STRING_INDUCTOR) == 0) new_mode = MODE_INDUCTOR;
    else if (strcmp(ringbuffer, STRING_RMS) == 0) new_mode = MODE_RMS;
    else if (strcmp(ringbuffer, STRING_FREQUENCY) == 0) new_mode = MODE_FREQUENCY;
    else if (strcmp(ringbuffer, STRING_TOGGLE) == 0) {
        menu_confirm_exit = !menu_confirm_exit;
    }

    if (menuState != new_mode || !menu_confirm_exit) {
        bt_change_mode = new_mode;
        char *string_memory = (char *) malloc(17 * sizeof(char));
        sprintf(string_memory, "%s", menuItems[bt_change_mode]);
        send_String(USART3, string_memory);
        free(string_memory);
    }

}

void adc_reading(uint8_t mode) {
    char *string_memory = (char *) malloc(64 * sizeof(char));

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
        lcd_output_value(string_memory, reading, &last_write_length1, &last_write_length2);
    }
    free(string_memory);
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

    while (!change_requested()) {
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

    while (change_requested()) {

        if (menuState < 4) {
            set_mux(menuState);
            adc_reading(menuState);
        } else {
            if (menuState == 4) {
                set_mux(10);
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
            set_buzz(1);
        } else {
            lcd_clear_display();
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
