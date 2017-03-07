#ifndef _BOARD_INIT_H_
#define _BOARD_INIT_H_

void GPIO_init(void);
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void init_board(void);

#endif
