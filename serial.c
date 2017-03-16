#include "stm32f4xx.h"
#include "serial.h"
#include "utility.h"
#include "ringbuffer.h"


extern ringbuffer_t ringbuffer;

// TODO: Move this
void handle_interrupt(ringbuffer_t rb){
		uint16_t data;
    do{
				data = USART_ReceiveData(USART3);
				if(data != NULL)
					ringbuffer_push(rb,(BUFFERTYPE)data);
    }while(!ringbuffer_is_full(rb) && data);
}

static void _configUSART3(uint32_t BAUD, uint32_t fosc) {
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

  GPIO_InitTypeDef GPIO_InitStruct;
	
	// 11 is TX. 10 is RX.
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, & GPIO_InitStruct);

  GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3); //
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);

  USART_InitTypeDef USART_InitStruct;

  USART_InitStruct.USART_BaudRate = BAUD;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_Init(USART3, & USART_InitStruct);

  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init( & NVIC_InitStructure);

  USART_Cmd(USART3, ENABLE);
}

static void _configUSART2(uint32_t BAUD, uint32_t fosc){
  uint32_t tmpreg = 0x00, apbclock = 0x00;
  uint32_t integerdivider = 0x00;
  uint32_t fractionaldivider = 0x00;

  apbclock = fosc/16;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;	/* Enable USART2 Clock */

	GPIOA->MODER &= ~GPIO_MODER_MODER2;
  GPIOA->MODER |=  GPIO_MODER_MODER2_1;		/* Setup TX pin for Alternate Function */

  GPIOA->AFR[0] |= (7 << (4*2));		/* Setup TX as the Alternate Function */

  USART2->CR1 |= USART_CR1_UE;	/* Enable USART */

  integerdivider = ((25 * apbclock) / (2 * (BAUD)));  
  tmpreg = (integerdivider / 100) << 4;
  fractionaldivider = integerdivider - (100 * (tmpreg >> 4));

  tmpreg |= ((((fractionaldivider * 8) + 50) / 100)) & ((uint8_t)0x07);

  USART2->BRR = (uint16_t)tmpreg;
  USART2->CR1 |= USART_CR1_TE;	/* Enable Tx */
}

/*----------------------------------------------------------------------------
  USART3 functionality. Currently writes a string to the 
	LCD whenever data is received
 *----------------------------------------------------------------------------*/
void USART3_IRQHandler(void) {

//    //Check if interrupt was because data is received
//    if (USART_GetITStatus(USART3, USART_IT_RXNE)) {
//        lcd_clear_display();
//        lcd_write_string("Data received", 0, 0, 0);
//        Delay(500);
//        handle_interrupt(ringbuffer);
//        //Clear interrupt flag
//        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
//    }
}


void serial_init(void) {
  //ringbuffer = ringbuffer_new(255);

		// Config for BT. 
  _configUSART3(9600, 168000000);
	
	// Config for serial
	_configUSART3(9600, 168000000);
}
