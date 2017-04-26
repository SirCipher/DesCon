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

#define MENU_LEFT -1
#define MENU_RIGHT -2
#define MENU_CONFIRM_EXIT -3
#define MENU_CONFIRM 1
#define MENU_EXIT 0
#define MODE_VOLTAGE 0
#define MODE_CURRENT 1
#define MODE_RESISTANCE 2
#define MODE_LIGHT 3
#define MODE_CONTINUITY 4
#define MODE_TRANSISTOR 5
#define MODE_DIODE 6
#define MODE_CAPACITOR 7
#define MODE_INDUCTOR 8
#define MODE_RMS 9
#define MODE_FREQUENCY 10
#define MODE_DISPLAY 0
#define MODE_SET 1

void ADC1_init(void);
void ADC2_init(void);
void set_mode_menu(int mode, int set_mode);

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

void TOM_lcd_send_string(int line, char* message);


#endif /*_ADC_H_*/
