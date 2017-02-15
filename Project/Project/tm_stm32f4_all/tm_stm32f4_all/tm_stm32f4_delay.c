/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */
#include "stm32f4_delay.h"

__IO uint32_t TimingDelay = 0;
__IO uint32_t Time = 0;
__IO uint32_t Time2 = 0;
volatile uint32_t mult;
uint8_t DELAY_Initialized = 0;

/* Private structure */
typedef struct {
	uint8_t Count;
	DELAY_Timer_t* Timers[DELAY_MAX_CUSTOM_TIMERS];
} DELAY_Timers_t;

/* Custom timers structure */
static DELAY_Timers_t CustomTimers;

#if defined(DELAY_TIM)
void DELAY_INT_InitTIM(void);
#endif

#if defined(DELAY_TIM)
void DELAY_TIM_IRQ_HANDLER(void) {
	DELAY_TIM->SR = ~TIM_IT_Update;
#elif defined(KEIL_IDE)
void TimingDelay_Decrement(void) {
#else
void SysTick_Handler(void) {
#endif
	uint8_t i;
	
	Time++;
	if (Time2 != 0x00) {
		Time2--;
	}
	
	/* Call user function */
	DELAY_1msHandler();
	
	/* Check custom timers */
	for (i = 0; i < CustomTimers.Count; i++) {
		/* Check if timer is enabled */
		if (
			CustomTimers.Timers[i] &&          /*!< Pointer exists */
			CustomTimers.Timers[i]->Enabled && /*!< Timer is enabled */
			CustomTimers.Timers[i]->CNT > 0    /*!< Counter is not NULL */
		) {
			/* Decrease counter */
			CustomTimers.Timers[i]->CNT--;
			
			/* Check if count is zero */
			if (CustomTimers.Timers[i]->CNT == 0) {
				/* Call user callback function */
				CustomTimers.Timers[i]->Callback(CustomTimers.Timers[i]->UserParameters);
				
				/* Set new counter value */
				CustomTimers.Timers[i]->CNT = CustomTimers.Timers[i]->ARR;
				
				/* Disable timer if auto reload feature is not used */
				if (!CustomTimers.Timers[i]->AutoReload) {
					/* Disable counter */
					CustomTimers.Timers[i]->Enabled = 0;
				}
			}
		}
	}
}

void DELAY_Init(void) {	
#if defined(DELAY_TIM)
	DELAY_INT_InitTIM();
#else
	/* Set Systick interrupt every 1ms */
	if (SysTick_Config(SystemCoreClock / 1000)) {
		/* Capture error */
		while (1);
	}
	
	#ifdef __GNUC__
		/* Set multiplier for delay under 1us with pooling mode = not so accurate */
		mult = SystemCoreClock / 7000000;
	#else
		/* Set multiplier for delay under 1us with pooling mode = not so accurate */
		mult = SystemCoreClock / 3000000;
	#endif
#endif
	
	/* Set initialized flag */
	DELAY_Initialized = 1;
}

void DELAY_EnableDelayTimer(void) {
	/* Check if library is even initialized */
	if (!DELAY_Initialized) {
		return;
	}
	
#if defined(DELAY_TIM)
	/* Enable TIMER for delay, useful when you wakeup from sleep mode */
	DELAY_TIM->CR1 |= TIM_CR1_CEN;
#else
	/* Enable systick interrupts, useful when you wakeup from sleep mode */  
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
#endif
}

void DELAY_DisableDelayTimer(void) {
#if defined(DELAY_TIM)
	/* Disable TIMER for delay, useful when you go to sleep mode */
	DELAY_TIM->CR1 &= ~TIM_CR1_CEN;
#else
	/* Disable systick, useful when you go to sleep mode */
	SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
#endif
}

/* Internal functions */
#if defined(DELAY_TIM)
void DELAY_INT_InitTIM(void) {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	TIMER_PROPERTIES_t TIM_Data;
	
	/* Get timer properties */
	TIMER_PROPERTIES_GetTimerProperties(DELAY_TIM, &TIM_Data);
	
	/* Generate timer properties, 1us ticks */
	TIMER_PROPERTIES_GenerateDataForWorkingFrequency(&TIM_Data, 1000000);
	
	/* Enable clock for TIMx */
	TIMER_PROPERTIES_EnableClock(DELAY_TIM);
	
	/* Set timer settings */
	TIM_TimeBaseStruct.TIM_ClockDivision = 0;
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStruct.TIM_Period = 999; /* 1 millisecond */
	TIM_TimeBaseStruct.TIM_Prescaler = SystemCoreClock / (1000000 * (SystemCoreClock / TIM_Data.TimerFrequency)) - 1; /* With prescaler for 1 microsecond tick */
	TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
	
	/* Initialize timer */
	TIM_TimeBaseInit(DELAY_TIM, &TIM_TimeBaseStruct);
	
	/* Enable interrupt each update cycle */
	DELAY_TIM->DIER |= TIM_IT_Update;
	
	/* Set NVIC parameters */
	NVIC_InitStruct.NVIC_IRQChannel = DELAY_TIM_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	
	/* Add to NVIC */
	NVIC_Init(&NVIC_InitStruct);
	
	/* Start timer */
	DELAY_TIM->CR1 |= TIM_CR1_CEN;
}
#endif

DELAY_Timer_t* DELAY_TimerCreate(uint32_t ReloadValue, uint8_t AutoReload, uint8_t StartTimer, void (*DELAY_CustomTimerCallback)(void *), void* UserParameters) {
	DELAY_Timer_t* tmp;
	
	/* Check if available */
	if (CustomTimers.Count >= DELAY_MAX_CUSTOM_TIMERS) {
		return NULL;
	}
	
	/* Try to allocate memory for timer structure */
	tmp = (DELAY_Timer_t *) LIB_ALLOC_FUNC(sizeof(DELAY_Timer_t));
	
	/* Check if allocated */
	if (tmp == NULL) {
		return NULL;
	}
	
	/* Fill settings */
	tmp->ARR = ReloadValue;
	tmp->CNT = tmp->ARR;
	tmp->AutoReload = AutoReload;
	tmp->Enabled = StartTimer;
	tmp->Callback = DELAY_CustomTimerCallback;
	tmp->UserParameters = UserParameters;
	
	/* Increase number of timers in memory */
	CustomTimers.Timers[CustomTimers.Count++] = tmp;
	
	/* Return pointer to user */
	return tmp;
} 

void DELAY_TimerDelete(DELAY_Timer_t* Timer) {
	uint8_t i;
	uint32_t irq;
	DELAY_Timer_t* tmp;
	
	/* Get location in array of pointers */
	for (i = 0; i < CustomTimers.Count; i++) {
		if (Timer == CustomTimers.Timers[i]) {
			break;
		}
	}
	
	/* Check for valid input */
	if (i == CustomTimers.Count) {
		return;
	}
	
	/* Save pointer to timer */
	tmp = CustomTimers.Timers[i];
	
	/* Get interrupt status */
	irq = __get_PRIMASK();

	/* Disable interrupts */
	__disable_irq();
	
	/* Shift array up */
	for (; i < (CustomTimers.Count - 1); i++) {
		/* Shift data to the left */
		CustomTimers.Timers[i] = CustomTimers.Timers[i + 1];
	}
	
	/* Decrease count */
	CustomTimers.Count--;
	
	/* Enable IRQ if necessary */
	if (!irq) {
		__enable_irq();
	}
	
	/* Free timer */
	LIB_FREE_FUNC(tmp);
}

DELAY_Timer_t* DELAY_TimerStop(DELAY_Timer_t* Timer) {
	/* Disable timer */
	Timer->Enabled = 0;
	
	/* Return pointer */
	return Timer;
}

DELAY_Timer_t* DELAY_TimerStart(DELAY_Timer_t* Timer) {
	/* Enable timer */
	Timer->Enabled = 1;
	
	/* Return pointer */
	return Timer;
}

DELAY_Timer_t* DELAY_TimerReset(DELAY_Timer_t* Timer) {
	/* Reset timer */
	Timer->CNT = Timer->ARR;
	
	/* Return pointer */
	return Timer;
}

DELAY_Timer_t* DELAY_TimerAutoReload(DELAY_Timer_t* Timer, uint8_t AutoReload) {
	/* Reset timer */
	Timer->AutoReload = AutoReload;
	
	/* Return pointer */
	return Timer;
}

DELAY_Timer_t* DELAY_TimerAutoReloadValue(DELAY_Timer_t* Timer, uint32_t AutoReloadValue) {
	/* Reset timer */
	Timer->ARR = AutoReloadValue;
	
	/* Return pointer */
	return Timer;
}
