#include "utility.h"
#include "stm32f4xx.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "lcd_driver.h"
#include "freq_meter.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define PERIOD_SCALER 100000

volatile int capture_value;  //period of timing
volatile int freq;


void GPIOA_init__for_TIM(void) {  // GPIO configuration and init: pin 9: timer set up/definitions
    GPIO_InitTypeDef GPIO_Init_port;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM8);          // Timer 8, channel 2, GPIO PB7

    GPIO_Init_port.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init_port.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init_port.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init_port.GPIO_PuPd = GPIO_PuPd_DOWN;

    GPIO_Init(GPIOC, &GPIO_Init_port);
}


void TIM8_init_cnt(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;  //TIM1 Definition
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);  //clock enable

    // TIM1 definitions of mode values: Prescaler, Period etc.
    TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = 100000;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;

    //Hard coded TIM1 config
    // Capture/compare mode register 1
    TIM8->CCMR1 |= TIM_CCMR1_CC2S_0;               //capture compare mode register set to capture compare channel 2

    // Capture/compare enable register
    //TIM8->CCER = TIM_CCER_CC2P ; default?
    TIM8->SMCR |= TIM_SMCR_TS_2 | TIM_SMCR_TS_1;

    TIM8->SMCR |= TIM_SMCR_SMS_2;

    TIM8->CCER |= TIM_CCER_CC2E;
    TIM8->DIER |= TIM_DIER_CC2IE;


    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseInitStruct); //TIM1 Initialising
    TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);    //TIM1 Interupt config and enable
    TIM_Cmd(TIM8, ENABLE); // enable/start timer 1


    //Nested Vector Interrupt Controller config
    NVIC_InitTypeDef NVIC_InitStruct;                                    //NVIC definition
    NVIC_InitStruct.NVIC_IRQChannel = TIM_IT_CC2;              //TIM1 Capture Compare 2 Interrupt
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStruct); //NVIC Initalising





}

void TIM8_IRQ_handler(void) {  //timer 1 CC channel interrupt handler   T
    if (TIM_GetITStatus(TIM8, TIM_IT_Update)) {
        capture_value = TIM8->CCR2;
        TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
    }
}


//void auto_range_freq(char ccr2_string_value[10], int range_scaler){  //changes band of frequency depending on ccr2 value?
//	if(strncmp(ccr2_string_value, ,) ){
//	  range_scaler = ;
//	}
//	else if(strncmp(ccr2_string_value, ,){

//	}



//}
double scale_freq(char ccr2_string_value[10],double scaler) {
//cheat code for range 2.5kHz-4.8kHz, need sig gen tomoz to write proper version
    if (strncmp(ccr2_string_value, "600", 10) < 0) {
        scaler = 0.11;
    } else if (strncmp(ccr2_string_value, "1300", 10) < 0) {
        scaler = 0.3;
    } else if (strncmp(ccr2_string_value, "2100", 10) < 0) {
        scaler = 0.45;
    } else if (strncmp(ccr2_string_value, "3000", 10) < 0) {
        scaler = 0.64;
    } else if (strncmp(ccr2_string_value, "4000", 10) < 0) {
        scaler = 0.85;
    } else if (strncmp(ccr2_string_value, "5000", 10) < 0) {
        scaler = 1.07;
    } else if (strncmp(ccr2_string_value, "6000", 10) < 0) {
        scaler = 1.32;
    } else if (strncmp(ccr2_string_value, "8000", 10) < 0) {
        scaler = 1.88;
    } else if (strncmp(ccr2_string_value, "9000", 10) < 0) {
        scaler = 2.21;
    } else if (strncmp(ccr2_string_value, "10000", 10) < 0) {
        scaler = 2.565;
    } else if (strncmp(ccr2_string_value, "11000", 10) < 0) {
        scaler = 2.96;
    } else if (strncmp(ccr2_string_value, "13000", 10) < 0) {
        scaler = 3.39;
    } else if (strncmp(ccr2_string_value, "15000", 10) < 0) {
        scaler = 4.4;
    } else if (strncmp(ccr2_string_value, "17000", 10) < 0) {
        scaler = 4.98;
    } else if (strncmp(ccr2_string_value, "19000", 10) < 0) {
        scaler = 5.64;
    } else if (strncmp(ccr2_string_value, "20000", 10) < 0) {
        scaler = 6.36;
    } else if (strncmp(ccr2_string_value, "22000", 10) < 0) {
        scaler = 7.12;
    } else if (strncmp(ccr2_string_value, "24000", 10) < 0) {
        scaler = 8.09;
    } else if (strncmp(ccr2_string_value, "26000", 10) < 0) {
        scaler = 9.12;
    } else if (strncmp(ccr2_string_value, "28000", 10) < 0) {
        scaler = 10.28;
    } else if (strncmp(ccr2_string_value, "11000", 10) < 0) {
        scaler = 11.6;
    } else
        scaler = 13.1;

    return scaler;
}

int freq_meter(void) {
    int last_write_length1 = 0, last_write_length2 = 0;
    volatile int frequency = 0, scaler = 0;
    GPIOA_init__for_TIM();                                               // GPIOA init
    TIM8_init_cnt();                                                     //initialise and enable TIM1 counter

    while (1) {
        char snum[10];
        char snum2[10];
        sprintf(snum, "%d", TIM8->CCR2);                                   //convert from int to string

        scale_freq(snum, scaler);
        frequency = TIM8->CCR2 / scaler;
        sprintf(snum2, "%d  Hz", frequency);
        lcd_write_string(snum, 0, 0,
                         &last_write_length1);       // show frequency based on timing period counted, on the LCD

        lcd_write_string(snum2, 1, 0, &last_write_length2);

        for (int i = 0; i < 1000000; i++) {};                 // time to slow down loop execution
        lcd_clear_display();
    };
}

