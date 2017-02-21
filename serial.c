#include "stm32f4xx.h"
#include "serial.h"
#include "utility.h"

static void _configUSART2(uint32_t BAUD, uint32_t fosc)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  //This is for the GPIO pins used as TX and RX
  GPIO_InitTypeDef GPIO_InitStruct;

  /* This sequence sets up the TX and RX pins 
   * so they work correctly with the USART1 peripheral
   */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; // Pins 2 (TX) and 3 (RX) are used
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; // the pins are configured as alternate function so the USART peripheral has access to them
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; // this defines the IO speed and has nothing to do with the baudrate!
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; // this defines the output type as push pull mode (as opposed to open drain)
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP; // this activates the pullup resistors on the IO pins
  GPIO_Init(GPIOA, & GPIO_InitStruct); // now all the values are passed to the GPIO_Init() function which sets the GPIO registers

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); //
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

  USART_InitTypeDef USART_InitStruct; // this is for the USART2 initilization

  USART_InitStruct.USART_BaudRate = BAUD; // the baudrate is set to the value we passed into this init function
  USART_InitStruct.USART_WordLength = USART_WordLength_8b; // we want the data frame size to be 8 bits (standard)
  USART_InitStruct.USART_StopBits = USART_StopBits_1; // we want 1 stop bit (standard)
  USART_InitStruct.USART_Parity = USART_Parity_No; // we don't want a parity bit (standard)
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // we don't want flow control (standard)
  USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // we want to enable the transmitter and the receiver
  USART_Init(USART2, & USART_InitStruct); // again all the properties are passed to the USART_Init function which takes care of all the bit setting

  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // enable the USART2 receive interrupt 

  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; // we want to configure the USART2 interrupts
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // this sets the priority group of the USART1 interrupts
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // this sets the subpriority inside the group
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // the USART2 interrupts are globally enabled
  NVIC_Init( & NVIC_InitStructure); // the properties are passed to the NVIC_Init function which takes care of the low level stuff	

  //Finally this enables the complete USART1 peripheral
  USART_Cmd(USART2, ENABLE);
}

/*----------------------------------------------------------------------------
  USART2 functionality. Currently writes a string to the 
	LCD whenever data is received
 *----------------------------------------------------------------------------*/
void USART2_IRQHandler(void) {

		//Check if interrupt was because data is received
    if (USART_GetITStatus(USART2, USART_IT_RXNE)) {
        lcd_clear_display();
				lcd_write_string("Data received",0,0,0);
				Delay(500);
				
				//Clear interrupt flag
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}


void serial_init(void) {
	_configUSART2(38400, 168000000);
}
