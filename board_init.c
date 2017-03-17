#include "ADC.h"
#include "board_init.h"
#include "ringbuffer.h"
#include "stm32f4xx.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_hd44780.h"

extern ringbuffer_t ringbuffer;

/*----------------------------------------------------------------------------
  Function that initializes Button pins
 *----------------------------------------------------------------------------*/
void GPIO_init(void){
		/* Enable clock for SYSCFG */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
		GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    
		//Initialise GPIOE 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;  // we want to configure PE8-15
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;       //As inputs
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;	//Polled at 2Mhz (Humans aren't quite fast enough to justify more)
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;   	//As push / pull 
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;   	//With a pulldown resistor, as detecting a high level
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
    EXTI_InitStruct.EXTI_Line = EXTI_Line8 | EXTI_Line9 | EXTI_Line10 | EXTI_Line11 | EXTI_Line12 | EXTI_Line13 | EXTI_Line14 | EXTI_Line15;
    /* Enable interrupt */
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    /* Interrupt mode */
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    /* Triggers on rising and falling edge */
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
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

void EXTI9_5_IRQHandler(void) {
	/* Make sure that interrupt flag is set */
   if (EXTI_GetITStatus(EXTI_Line8) != RESET) {
			
		 EXTI_ClearITPendingBit(EXTI_Line8);
   } else if (EXTI_GetITStatus(EXTI_Line9) != RESET) {

		 EXTI_ClearITPendingBit(EXTI_Line9);
   } 
 }

void EXTI15_10_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line10) != RESET) {

		EXTI_ClearITPendingBit(EXTI_Line10);
	} else if (EXTI_GetITStatus(EXTI_Line11) != RESET) {

		EXTI_ClearITPendingBit(EXTI_Line11);
   }else if (EXTI_GetITStatus(EXTI_Line12) != RESET) {

		 EXTI_ClearITPendingBit(EXTI_Line12);
   }else if (EXTI_GetITStatus(EXTI_Line13) != RESET) {

		 EXTI_ClearITPendingBit(EXTI_Line13);
   }else if (EXTI_GetITStatus(EXTI_Line14) != RESET) {

		 EXTI_ClearITPendingBit(EXTI_Line14);
   } else if (EXTI_GetITStatus(EXTI_Line15) != RESET) {

		 EXTI_ClearITPendingBit(EXTI_Line15);
   }
 }

void TOM_lcd_init(){
	TM_HD44780_Init(16, 2);
}

/*----------------------------------------------------------------------------
  Initialize the components we require on the board
 *----------------------------------------------------------------------------*/
void init_board(void) {
    SystemCoreClockUpdate();                      /* Get Core Clock Frequency   */
    if (SysTick_Config(SystemCoreClock / 1000)) { /* SysTick 1 msec interrupts  */
        while (1);                                  /* Capture error              */
    }

    //ringbuffer = ringbuffer_new(255);

    GPIO_init();
    ADC1_init();
    ADC2_init();
    SWT_Init();
    lcd_init(LCD_LINES_TWO, LCD_CURSOR_OFF, LCD_CBLINK_OFF, 128);
		TOM_lcd_init();
    serial_init();
}
