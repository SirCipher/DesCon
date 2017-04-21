#include "stm32f4xx.h"
#include "serial.h"
#include "utility.h"
#include "ringbuffer.h"
#include "ADC.h"
#include <string.h>

#define MAX_CHARACTERS 15
extern ringbuffer_t ringbuffer;
volatile char line_buffer[MAX_CHARACTERS + 1];

// TODO: Move this
//void handle_interrupt(ringbuffer_t rb){
//		uint16_t data;
//    do{
//				data = USART_ReceiveData(USART3);
//				if(data != NULL)
//					ringbuffer_push(rb,(BUFFERTYPE)data);
//    }while(!ringbuffer_is_full(rb) && data);
//}
// GPIO C pin 10/11. 10 RX, 11 TX

static void _configUSART3(uint32_t baudrate, uint32_t fosc) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;

    // 11 is TX. 10 is RX.
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3); //
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);

    USART_InitTypeDef USART_InitStruct;

    USART_InitStruct.USART_BaudRate = baudrate;
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

static void _configUSART2(uint32_t BAUD, uint32_t fosc) {
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
    GPIO_Init(GPIOA,
              &GPIO_InitStruct); // now all the values are passed to the GPIO_Init() function which sets the GPIO registers

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); //
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

    USART_InitTypeDef USART_InitStruct; // this is for the USART2 initilization

    USART_InitStruct.USART_BaudRate = BAUD; // the baudrate is set to the value we passed into this init function
    USART_InitStruct.USART_WordLength = USART_WordLength_8b; // we want the data frame size to be 8 bits (standard)
    USART_InitStruct.USART_StopBits = USART_StopBits_1; // we want 1 stop bit (standard)
    USART_InitStruct.USART_Parity = USART_Parity_No; // we don't want a parity bit (standard)
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // we don't want flow control (standard)
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // we want to enable the transmitter and the receiver
    USART_Init(USART2,
               &USART_InitStruct); // again all the properties are passed to the USART_Init function which takes care of all the bit setting

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // enable the USART2 receive interrupt

    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; // we want to configure the USART2 interrupts
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // this sets the priority group of the USART1 interrupts
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // this sets the subpriority inside the group
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // the USART2 interrupts are globally enabled
    NVIC_Init(
            &NVIC_InitStructure); // the properties are passed to the NVIC_Init function which takes care of the low level stuff

    //Finally this enables the complete USART1 peripheral
    USART_Cmd(USART2, ENABLE);
}

void USART3_IRQHandler(void) {
//  static char rx_buffer[MAX_CHARACTERS];
//  static int rx_index = 0;

    // Have we received a character?
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {
        char rx = USART_ReceiveData(USART3);

        if ((rx == '\r') || (rx == '\n')) { // Is this an end-of-line condition?
            if (ringbuffer_is_empty(ringbuffer) != 0) {
                int maxCount = ringbuffer_count(ringbuffer);
                int i = 0;
                for(int i = 0; i < maxCount; i++){
                    line_buffer[i] = ringbuffer_shift(ringbuffer);
                }

                
                line_buffer[i] = '\0';
								//setState((char *)line_buffer);

//                memcpy((void *) line_buffer, rx_buffer,
//                       rx_index); // Copy to static line buffer from dynamic receive buffer
//                line_buffer[rx_index] = 0;
//                setState(rx_buffer);
//                send_String(USART3, rx_buffer);
//                rx_index = 0; // Reset index pointer
//                memset(rx_buffer, 0, 255); // Clear the buffer after we are done with it.
            }
        } else {
            // Have we overflown? Data loss will occur.
            ringbuffer_push(ringbuffer,rx);
        }
    }
}

void serial_init(void) {
    ringbuffer = ringbuffer_new(MAX_CHARACTERS);

    // Config for BT.
    _configUSART3(9600, 168000000);

    // Config for serial
    _configUSART2(9600, 168000000);
}
