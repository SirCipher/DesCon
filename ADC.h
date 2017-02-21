#ifndef _ADC_H_
#define _ADC_H_
#define MAX_CALIBRATION_LOOPS 5;

#include "STM32F4xx.h"
#include "utils.h"

void ADC1_init(void);
void ADC2_init(void);

/* function to read ADCs and retun value */
unsigned int read_ADC1 (void);
unsigned int read_ADC2 (void);

void set_cont_ADC1(void);	

/* function to set ADC to cont */
unsigned int read_cont_ADC1 (void);

double calibrate_ADC1(void);

#endif /*_ADC_H_*/
