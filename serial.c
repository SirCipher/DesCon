#include "stm32f4xx.h"
#include "serial.h"
#include "utility.h"

static void _configUSART3(uint32_t BAUD, uint32_t fosc) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3); //
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);

    USART_InitTypeDef USART_InitStruct;

    USART_InitStruct.USART_BaudRate = BAUD;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART3, &USART_InitStruct);

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(USART3, ENABLE);
}

/*----------------------------------------------------------------------------
  USART3 functionality. Currently writes a string to the 
	LCD whenever data is received
 *----------------------------------------------------------------------------*/
void USART3_IRQHandler(void) {

    //Check if interrupt was because data is received
    if (USART_GetITStatus(USART3, USART_IT_RXNE)) {
        lcd_clear_display();
        lcd_write_string("Data received", 0, 0, 0);
        Delay(500);

        //Clear interrupt flag
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}


void serial_init(void) {
    _configUSART3(9600, 168000000);
}
