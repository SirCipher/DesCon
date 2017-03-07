#ifndef _UTILITY_H_
#define _UTILITY_H_
#define MAX_CALIBRATION_LOOPS 5;

#include "stm32f4xx.h"

void Delay(uint32_t dlyTicks);

void SysTick_Handler(void);

double scale(double x, double in_min, double in_max, double out_min, double out_max);

#endif /*_UTILITY_H_*/
