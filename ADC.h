#ifndef _ADC_H_
#define _ADC_H_
#define MAX_CALIBRATION_LOOPS 5;

#include "lcd_buffer.h"
#include "lcd_driver.h"
#include "LED.h"
#include "ADC.h"
#include "serial.h"
#include "sig_gen.h"
#include "SWT.h"
#include "ADC.h"
#include "utility.h"
#include "stm32f4xx.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "misc.h"
#include "defines.h"
#include "app.h"

void ADC1_init(void);

void ADC2_init(void);

void menu(void);

void init_board(void);

/* function to read ADCs and retun value */
unsigned int read_ADC1(void);

unsigned int read_ADC2(void);

void set_cont_ADC1(void);

/* function to set ADC to cont */
unsigned int read_cont_ADC1(void);

double calibrate_ADC1(void);

void send_String(USART_TypeDef *USARTx, char *str);

void send_Char(USART_TypeDef *USARTx, uint8_t ch);

void setState(char *mode);

void adc_reading(uint8_t mode);

void continuity(void);

void welcome_sequence(void);

void check_string_set_mode(char rx_buffer[]);

void set_mux(uint8_t mux);

void light_led(uint8_t led);

void flash_led(uint8_t led);

void clear_led(void);

void set_leds(void);

void start_messages(void);

int change_requested(void);

void set_buzz(uint8_t buzz);

#endif /*_ADC_H_*/
