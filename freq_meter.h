#ifndef _FREQ_METER_H_
#define _FREQ_METER_H_


int freq_meter(void);
void TIM1_IRQ_handler(void);
void TIM1_init_cnt(void);
void GPIOA_init__for_TIM(void) ;
double scale_freq(char ccr2_string_value[10],double scaler);
#endif /*_FREQ_METER_H_*/