#include "ADC.h"
#include "board_init.h"
#include "ringbuffer.h"
#include "stm32f4xx.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "utility.h"

extern ringbuffer_t ringbuffer;
extern uint8_t state;

/*----------------------------------------------------------------------------
  Function that initializes Button pins
 *----------------------------------------------------------------------------*/
void GPIO_init(void) {
    /* Enable clock for SYSCFG */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    //Initialise GPIOE
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14 |
                               GPIO_Pin_15;  // we want to configure PE8-15
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;       //As inputs
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;    //Polled at 2Mhz (Humans aren't quite fast enough to justify more)
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;    //As push / pull
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;    //With a pulldown resistor, as detecting a high level
    GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* Tell system that you will use PDE8-15 to trigger interrupts */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource8);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource9);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource10);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource11);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource12);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource13);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource14);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource15);

    /* Port E's buttons from 8 through 15 all lie on different EXTI lines*/
    EXTI_InitStruct.EXTI_Line =
            EXTI_Line8 | EXTI_Line9 | EXTI_Line10 | EXTI_Line11 | EXTI_Line12 | EXTI_Line13 | EXTI_Line14 | EXTI_Line15;
    /* Enable interrupt */
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    /* Interrupt mode */
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    /* Triggers on rising and falling edge */
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
    /* Add to EXTI */
    EXTI_Init(&EXTI_InitStruct);

    /* Add IRQ vector to NVIC */
    /* PE8-15 are connected to EXTI_Line8-15, which have EXTI9_5_IRQn and EXTI15_10_IRQn */
    NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
    /* Set priority */
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
    /* Set sub priority */
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
    /* Enable interrupt */
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    /* Add to NVIC */
    NVIC_Init(&NVIC_InitStruct);
    NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
    /* Add to NVIC Again */
    NVIC_Init(&NVIC_InitStruct);
}

void GPIOC_init(void) {
    /* Enable clock for SYSCFG */
    GPIO_InitTypeDef GPIO_InitStruct;

    //Initialise GPIOE
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_7 | GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15;   // we want to configure PE8-15
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;       
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;    //Polled at 2Mhz (Humans aren't quite fast enough to justify more)
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;    //As push / pull
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;    //With a pulldown resistor, as detecting a high level
    GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void GPIOA_init(void){
		/* Enable clock for SYSCFG */
    GPIO_InitTypeDef GPIO_InitStruct;

    //Initialise GPIOE
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_13 | GPIO_Pin_14 ;   // we want to configure PE8-15
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;       
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;    //Polled at 2Mhz (Humans aren't quite fast enough to justify more)
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;    //As push / pull
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;    //With a pulldown resistor, as detecting a high level
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}


uint8_t button_selected = 0;
extern uint8_t menuState;
extern uint8_t menuCount;
extern uint8_t set_selection;
extern uint8_t menu_confirm_exit;

void EXTI9_5_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line8) != RESET) {
        if (!menu_confirm_exit) {
            menuState += menuCount - 1;
            menuState %= menuCount;
        }

        EXTI_ClearITPendingBit(EXTI_Line8);

    } else if (EXTI_GetITStatus(EXTI_Line9) != RESET) {
        EXTI_ClearITPendingBit(EXTI_Line9);
    }
}

void EXTI15_10_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line10) != RESET) {
        EXTI_ClearITPendingBit(EXTI_Line10);

    } else if (EXTI_GetITStatus(EXTI_Line11) != RESET) {
        if (menu_confirm_exit) menu_confirm_exit = 0;
        else if (!menu_confirm_exit) menu_confirm_exit = 1;

        EXTI_ClearITPendingBit(EXTI_Line11);

    } else if (EXTI_GetITStatus(EXTI_Line12) != RESET) {
        EXTI_ClearITPendingBit(EXTI_Line12);

    } else if (EXTI_GetITStatus(EXTI_Line13) != RESET) {
        EXTI_ClearITPendingBit(EXTI_Line13);

    } else if (EXTI_GetITStatus(EXTI_Line14) != RESET) {
        EXTI_ClearITPendingBit(EXTI_Line14);

    } else if (EXTI_GetITStatus(EXTI_Line15) != RESET) {
        if (!menu_confirm_exit) {
            menuState++;
            menuState %= menuCount;
        }

        EXTI_ClearITPendingBit(EXTI_Line15);
    }
}

void init_leds() {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    GPIOD->MODER |= (0x5555UL << 16);
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

// Welcome LED sequence
void welcome_sequence(void) {
    int led2light, i;

    for (i = 0; i < 8; i++) {
        led2light = (int) pow(2.0, i);
        GPIOD->ODR &= 0x00FF;
        GPIOD->ODR |= (led2light << 8);
        Delay(40);
    }

    for (i = 7; i >= 0; i--) {
        led2light = (int) pow(2, i);
        GPIOD->ODR &= 0x00FF;
        GPIOD->ODR |= (led2light << 8);
        Delay(40);
    }
    GPIOD->ODR &= 0x00FF;
}

/*----------------------------------------------------------------------------
  Initialize the components we require on the board
 *----------------------------------------------------------------------------*/
void init_board(void) {
    SystemCoreClockUpdate();                      /* Get Core Clock Frequency   */
    if (SysTick_Config(SystemCoreClock / 1000)) { /* SysTick 1 msec interrupts  */
        while (1);                                  /* Capture error              */
    }

    GPIO_init();
		GPIOA_init();
		GPIOC_init();
    init_leds();
    ADC1_init();
    ADC2_init();
    SWT_Init();
    lcd_init(LCD_LINES_TWO, LCD_CURSOR_OFF, LCD_CBLINK_OFF, 128);
    serial_init();
    welcome_sequence();
}
