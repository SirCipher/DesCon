#ifndef __MAIN_H
#define __MAIN_H
#include "stm32f4xx.h"

/* Global Definitions */
#define	OFF									0
#define ON									1
#define ON_EXCLUSIVE				2
#define LED_FLASH						2
#define RED_LED							0
#define ORANGE_LED					1
#define GREEN_LED						2
#define BLUE_LED						3
#define ALL_LED							4

#define	TESR_SPEED	25
#define	LED_RING_DELAY	100
#define LED_FLASH_DURATION	50
#define LED_RING_FLASH_INTERVAL	7000

#define FAILURE_MESSAGE_DURATION	500

//Local Function prototypes
void 	SysTick_Handler(void);
void 	Delay_Millis (uint32_t);
void	LED_Init(void);
void 	LED_Write(uint8_t inByte);
void	Totally_Excessive_Startup_Routine(void);

//Global Function Prototypes
extern void Delay_Millis (unsigned int);

#endif
