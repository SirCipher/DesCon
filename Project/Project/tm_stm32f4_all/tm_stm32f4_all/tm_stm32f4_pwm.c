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
#include "stm32f4_pwm.h"

/* Private functions */
void PWM_INT_EnableClock(TIM_TypeDef* TIMx);
PWM_Result_t PWM_INT_GetTimerProperties(TIM_TypeDef* TIMx, uint32_t* frequency, uint32_t* period);

PWM_Result_t PWM_INT_InitTIM1Pins(PWM_Channel_t Channel, PWM_PinsPack_t PinsPack);
PWM_Result_t PWM_INT_InitTIM2Pins(PWM_Channel_t Channel, PWM_PinsPack_t PinsPack);
PWM_Result_t PWM_INT_InitTIM3Pins(PWM_Channel_t Channel, PWM_PinsPack_t PinsPack);
PWM_Result_t PWM_INT_InitTIM4Pins(PWM_Channel_t Channel, PWM_PinsPack_t PinsPack);
PWM_Result_t PWM_INT_InitTIM5Pins(PWM_Channel_t Channel, PWM_PinsPack_t PinsPack);
PWM_Result_t PWM_INT_InitTIM8Pins(PWM_Channel_t Channel, PWM_PinsPack_t PinsPack);
PWM_Result_t PWM_INT_InitTIM9Pins(PWM_Channel_t Channel, PWM_PinsPack_t PinsPack);
PWM_Result_t PWM_INT_InitTIM10Pins(PWM_Channel_t Channel, PWM_PinsPack_t PinsPack);
PWM_Result_t PWM_INT_InitTIM11Pins(PWM_Channel_t Channel, PWM_PinsPack_t PinsPack);
PWM_Result_t PWM_INT_InitTIM12Pins(PWM_Channel_t Channel, PWM_PinsPack_t PinsPack);
PWM_Result_t PWM_INT_InitTIM13Pins(PWM_Channel_t Channel, PWM_PinsPack_t PinsPack);
PWM_Result_t PWM_INT_InitTIM14Pins(PWM_Channel_t Channel, PWM_PinsPack_t PinsPack);

PWM_Result_t PWM_InitTimer(TIM_TypeDef* TIMx, PWM_TIM_t* TIM_Data, double PWMFrequency) {
	TIM_TimeBaseInitTypeDef TIM_BaseStruct;
	TIMER_PROPERTIES_t Timer_Data;

	/* Check valid timer */
	if (0 
#ifdef TIM6
		|| TIMx == TIM6
#endif	
#ifdef TIM7
		|| TIMx == TIM7
#endif
	) {
		/* Timers TIM6 and TIM7 can not provide PWM feature */
		return PWM_Result_TimerNotValid;
	}
	
	/* Save timer */
	TIM_Data->TIM = TIMx;
	
	/* Get timer properties */
	TIMER_PROPERTIES_GetTimerProperties(TIMx, &Timer_Data);
	
	/* Check for maximum timer frequency */
	if (PWMFrequency > Timer_Data.TimerFrequency) {
		/* Frequency too high */
		return PWM_Result_FrequencyTooHigh;
	} else if (PWMFrequency == 0) {
		/* Not valid frequency */
		return PWM_Result_FrequencyTooLow;
	}

	/* Generate settings */
	TIMER_PROPERTIES_GenerateDataForWorkingFrequency(&Timer_Data, PWMFrequency);
	
	/* Check valid data */
	if (Timer_Data.Period == 0) {
		/* Too high frequency */
		return PWM_Result_FrequencyTooHigh;
	}
	
	/* Tests are OK */
	TIM_Data->Frequency = PWMFrequency;
	TIM_Data->Micros = 1000000 / PWMFrequency;
	TIM_Data->Period = Timer_Data.Period;
	TIM_Data->Prescaler = Timer_Data.Prescaler;
	
	/* Enable clock for Timer */	
	TIMER_PROPERTIES_EnableClock(TIMx);

	/* Set timer options */
	TIM_BaseStruct.TIM_Prescaler = Timer_Data.Prescaler - 1;
	TIM_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_BaseStruct.TIM_Period = Timer_Data.Period - 1; 
	TIM_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_BaseStruct.TIM_RepetitionCounter = 0;
	
	/* Initialize timer */
	TIM_TimeBaseInit(TIMx, &TIM_BaseStruct);
	
	/* Preload enable */
	TIM_ARRPreloadConfig(TIMx, ENABLE);
	
	if (0
#ifdef TIM1
		|| TIMx == TIM1
#endif	
#ifdef TIM8
		|| TIMx == TIM8
#endif
	) {
		/* Enable PWM outputs */
		TIM_CtrlPWMOutputs(TIMx, ENABLE);
	}
	
	/* Start timer */
	TIMx->CR1 |= TIM_CR1_CEN;
	
	/* Set default values */
	TIM_Data->CH_Init = 0;
	
	/* Return OK */
	return PWM_Result_Ok;
}


PWM_Result_t PWM_InitChannel(PWM_TIM_t* TIM_Data, PWM_Channel_t Channel, PWM_PinsPack_t PinsPack) {
#ifdef TIM1
	if (TIM_Data->TIM == TIM1) {
		return PWM_INT_InitTIM1Pins(Channel, PinsPack);
	}  
#endif
#ifdef TIM2
	if (TIM_Data->TIM == TIM2) {
		return PWM_INT_InitTIM2Pins(Channel, PinsPack);
	}  
#endif
#ifdef TIM3
	if (TIM_Data->TIM == TIM3) {
		return PWM_INT_InitTIM3Pins(Channel, PinsPack);
	}  
#endif
#ifdef TIM4
	if (TIM_Data->TIM == TIM4) {
		return PWM_INT_InitTIM4Pins(Channel, PinsPack);
	}  
#endif
#ifdef TIM5
	if (TIM_Data->TIM == TIM5) {
		return PWM_INT_InitTIM5Pins(Channel, PinsPack);
	}  
#endif
#ifdef TIM8
	if (TIM_Data->TIM == TIM8) {
		return PWM_INT_InitTIM8Pins(Channel, PinsPack);
	}  
#endif
#ifdef TIM9
	if (TIM_Data->TIM == TIM9) {
		return PWM_INT_InitTIM9Pins(Channel, PinsPack);
	}  
#endif
#ifdef TIM10
	if (TIM_Data->TIM == TIM10) {
		return PWM_INT_InitTIM10Pins(Channel, PinsPack);
	}  
#endif
#ifdef TIM11
	if (TIM_Data->TIM == TIM11) {
		return PWM_INT_InitTIM11Pins(Channel, PinsPack);
	}  
#endif
#ifdef TIM12
	if (TIM_Data->TIM == TIM12) {
		return PWM_INT_InitTIM12Pins(Channel, PinsPack);
	}
#endif
#ifdef TIM13
	if (TIM_Data->TIM == TIM13) {
		return PWM_INT_InitTIM13Pins(Channel, PinsPack);
	} 
#endif
#ifdef TIM14
	if (TIM_Data->TIM == TIM14) {
		return PWM_INT_InitTIM14Pins(Channel, PinsPack);
	}
#endif
	
	/* Timer is not valid */
	return PWM_Result_TimerNotValid;
}

PWM_Result_t PWM_SetChannel(PWM_TIM_t* TIM_Data, PWM_Channel_t Channel, uint32_t Pulse) {
	TIM_OCInitTypeDef TIM_OCStruct;
	uint8_t ch = (uint8_t)Channel;
	
	/* Check pulse length */
	if (Pulse >= TIM_Data->Period) {
		/* Pulse too high */
		return PWM_Result_PulseTooHigh;
	}

	/* Common settings */
	TIM_OCStruct.TIM_Pulse = Pulse;
	TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
	
	/* Save pulse value */
	TIM_Data->CH_Periods[ch] = Pulse;
	
	/* Go to bits */
	ch = 1 << ch;
	
	/* Select proper channel */
	switch (Channel) {
		case PWM_Channel_1:
			/* Check if initialized */
			if (!(TIM_Data->CH_Init & ch)) {
				TIM_Data->CH_Init |= ch;
				
				/* Init channel */
				TIM_OC1Init(TIM_Data->TIM, &TIM_OCStruct);
				TIM_OC1PreloadConfig(TIM_Data->TIM, TIM_OCPreload_Enable);
			}
			
			/* Set pulse */
			TIM_Data->TIM->CCR1 = Pulse;
			break;
		case PWM_Channel_2:
			/* Check if initialized */
			if (!(TIM_Data->CH_Init & ch)) {
				TIM_Data->CH_Init |= ch;
				
				/* Init channel */
				TIM_OC2Init(TIM_Data->TIM, &TIM_OCStruct);
				TIM_OC2PreloadConfig(TIM_Data->TIM, TIM_OCPreload_Enable);
			}
			
			/* Set pulse */
			TIM_Data->TIM->CCR2 = Pulse;
			break;
		case PWM_Channel_3:
			/* Check if initialized */
			if (!(TIM_Data->CH_Init & ch)) {
				TIM_Data->CH_Init |= ch;
				
				/* Init channel */
				TIM_OC3Init(TIM_Data->TIM, &TIM_OCStruct);
				TIM_OC3PreloadConfig(TIM_Data->TIM, TIM_OCPreload_Enable);
			}
			
			/* Set pulse */
			TIM_Data->TIM->CCR3 = Pulse;
			break;
		case PWM_Channel_4:
			/* Check if initialized */
			if (!(TIM_Data->CH_Init & ch)) {
				TIM_Data->CH_Init |= ch;
				
				/* Init channel */
				TIM_OC4Init(TIM_Data->TIM, &TIM_OCStruct);
				TIM_OC4PreloadConfig(TIM_Data->TIM, TIM_OCPreload_Enable);
			}
			
			/* Set pulse */
			TIM_Data->TIM->CCR4 = Pulse;
			break;
		default:
			break;
	}
	
	/* Return everything OK */
	return PWM_Result_Ok;
}

PWM_Result_t PWM_SetChannelPercent(PWM_TIM_t* TIM_Data, PWM_Channel_t Channel, float percent) {
	/* Check input value */
	if (percent > 100) {
		return PWM_SetChannel(TIM_Data, Channel, TIM_Data->Period);
	} else if (percent <= 0) {
		return PWM_SetChannel(TIM_Data, Channel, 0);
	}
	
	/* Set channel value */
	return PWM_SetChannel(TIM_Data, Channel, (uint32_t)((float)(TIM_Data->Period - 1) * percent) / 100);
}

PWM_Result_t PWM_SetChannelMicros(PWM_TIM_t* TIM_Data, PWM_Channel_t Channel, uint32_t micros) {
	/* If we choose too much micro seconds that we have valid */
	if (micros > TIM_Data->Micros) {
		/* Too high pulse */
		return PWM_Result_PulseTooHigh;
	}
	
	/* Set PWM channel */
	return PWM_SetChannel(TIM_Data, Channel, (uint32_t)((TIM_Data->Period - 1) * micros) / TIM_Data->Micros);
}

/* Private functions */
PWM_Result_t PWM_INT_InitTIM1Pins(PWM_Channel_t Channel, PWM_PinsPack_t PinsPack) {
	PWM_Result_t result = PWM_Result_PinNotValid;
	
	switch (Channel) {
		case PWM_Channel_1:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_8, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM1);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOE
					GPIO_InitAlternate(GPIOE, GPIO_PIN_9, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM1);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		case PWM_Channel_2:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_9, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM1);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOE
					GPIO_InitAlternate(GPIOE, GPIO_PIN_10, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM1);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		case PWM_Channel_3:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_10, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM1);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOE
					GPIO_InitAlternate(GPIOE, GPIO_PIN_13, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM1);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		case PWM_Channel_4:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_11, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM1);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOE
					GPIO_InitAlternate(GPIOE, GPIO_PIN_14, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM1);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		default:
			result = PWM_Result_ChannelNotValid;
			break;
	}
	
	return result;
}

PWM_Result_t PWM_INT_InitTIM2Pins(PWM_Channel_t Channel, PWM_PinsPack_t PinsPack) {
	PWM_Result_t result = PWM_Result_PinNotValid;
	
	switch (Channel) {
		case PWM_Channel_1:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_0, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM2);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_5, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM2);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_3:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_15, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM2);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		case PWM_Channel_2:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_1, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM2);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOB
					GPIO_InitAlternate(GPIOB, GPIO_PIN_3, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM2);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		case PWM_Channel_3:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_2, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM2);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOB
					GPIO_InitAlternate(GPIOB, GPIO_PIN_10, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM2);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		case PWM_Channel_4:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_3, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM2);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOB
					GPIO_InitAlternate(GPIOB, GPIO_PIN_11, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM2);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		default:
			result = PWM_Result_ChannelNotValid;
			break;
	}
	
	return result;
}

PWM_Result_t PWM_INT_InitTIM3Pins(PWM_Channel_t Channel, PWM_PinsPack_t PinsPack) {
	PWM_Result_t result = PWM_Result_PinNotValid;
	
	switch (Channel) {
		case PWM_Channel_1:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_6, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM3);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOB
					GPIO_InitAlternate(GPIOB, GPIO_PIN_4, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM3);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_3:
#ifdef GPIOC
					GPIO_InitAlternate(GPIOC, GPIO_PIN_10, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM3);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		case PWM_Channel_2:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_7, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM3);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOB
					GPIO_InitAlternate(GPIOB, GPIO_PIN_5, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM3);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_3:
#ifdef GPIOC
					GPIO_InitAlternate(GPIOC, GPIO_PIN_7, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM3);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		case PWM_Channel_3:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOB
					GPIO_InitAlternate(GPIOB, GPIO_PIN_0, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM3);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOC
					GPIO_InitAlternate(GPIOC, GPIO_PIN_8, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM3);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		case PWM_Channel_4:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOB
					GPIO_InitAlternate(GPIOB, GPIO_PIN_1, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM3);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOC
					GPIO_InitAlternate(GPIOC, GPIO_PIN_9, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM3);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		default:
			result = PWM_Result_ChannelNotValid;
			break;
	}
	
	return result;
}

PWM_Result_t PWM_INT_InitTIM4Pins(PWM_Channel_t Channel, PWM_PinsPack_t PinsPack) {
	PWM_Result_t result = PWM_Result_PinNotValid;

	switch (Channel) {
		case PWM_Channel_1:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOB
					GPIO_InitAlternate(GPIOB, GPIO_PIN_6, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM4);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOD
					GPIO_InitAlternate(GPIOD, GPIO_PIN_12, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM4);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		case PWM_Channel_2:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOB
					GPIO_InitAlternate(GPIOB, GPIO_PIN_7, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM4);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOD
					GPIO_InitAlternate(GPIOD, GPIO_PIN_13, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM4);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		case PWM_Channel_3:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOB
					GPIO_InitAlternate(GPIOB, GPIO_PIN_8, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM4);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOD
					GPIO_InitAlternate(GPIOD, GPIO_PIN_14, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM4);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		case PWM_Channel_4:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOB
					GPIO_InitAlternate(GPIOB, GPIO_PIN_9, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM4);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOD
					GPIO_InitAlternate(GPIOD, GPIO_PIN_15, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM4);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		default:
			result = PWM_Result_ChannelNotValid;
			break;
	}
	
	return result;
}

PWM_Result_t PWM_INT_InitTIM5Pins(PWM_Channel_t Channel, PWM_PinsPack_t PinsPack) {
	PWM_Result_t result = PWM_Result_PinNotValid;

	switch (Channel) {
		case PWM_Channel_1:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_0, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM5);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOH
					GPIO_InitAlternate(GPIOH, GPIO_PIN_10, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM5);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		case PWM_Channel_2:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_1, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM5);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOH
					GPIO_InitAlternate(GPIOH, GPIO_PIN_11, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM5);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		case PWM_Channel_3:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_2, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM5);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOH
					GPIO_InitAlternate(GPIOH, GPIO_PIN_12, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM5);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		case PWM_Channel_4:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_3, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM5);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOI
					GPIO_InitAlternate(GPIOI, GPIO_PIN_0, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM5);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		default:
			result = PWM_Result_ChannelNotValid;
			break;
	}
	
	return result;
}

PWM_Result_t PWM_INT_InitTIM8Pins(PWM_Channel_t Channel, PWM_PinsPack_t PinsPack) {
	PWM_Result_t result = PWM_Result_PinNotValid;

	switch (Channel) {
		case PWM_Channel_1:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOC
					GPIO_InitAlternate(GPIOC, GPIO_PIN_6, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM8);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOI
					GPIO_InitAlternate(GPIOI, GPIO_PIN_5, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM8);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		case PWM_Channel_2:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOC
					GPIO_InitAlternate(GPIOC, GPIO_PIN_7, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM8);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOI
					GPIO_InitAlternate(GPIOI, GPIO_PIN_6, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM8);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		case PWM_Channel_3:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOC
					GPIO_InitAlternate(GPIOC, GPIO_PIN_8, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM8);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOI
					GPIO_InitAlternate(GPIOI, GPIO_PIN_7, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM8);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		case PWM_Channel_4:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOC
					GPIO_InitAlternate(GPIOC, GPIO_PIN_9, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM8);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOI
					GPIO_InitAlternate(GPIOI, GPIO_PIN_8, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM8);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		default:
			result = PWM_Result_ChannelNotValid;
			break;
	}
	
	return result;
}

PWM_Result_t PWM_INT_InitTIM9Pins(PWM_Channel_t Channel, PWM_PinsPack_t PinsPack) {
	PWM_Result_t result = PWM_Result_PinNotValid;

	switch (Channel) {
		case PWM_Channel_1:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_2, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM9);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOE
					GPIO_InitAlternate(GPIOE, GPIO_PIN_5, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM9);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		case PWM_Channel_2:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_3, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM9);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOE
					GPIO_InitAlternate(GPIOE, GPIO_PIN_6, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM9);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		default:
			result = PWM_Result_ChannelNotValid;
			break;
	}
	
	return result;
}

PWM_Result_t PWM_INT_InitTIM10Pins(PWM_Channel_t Channel, PWM_PinsPack_t PinsPack) {
	PWM_Result_t result = PWM_Result_PinNotValid;

	switch (Channel) {
		case PWM_Channel_1:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOB
					GPIO_InitAlternate(GPIOB, GPIO_PIN_8, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM10);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOF
					GPIO_InitAlternate(GPIOF, GPIO_PIN_6, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM10);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		default:
			result = PWM_Result_ChannelNotValid;
			break;
	}
	
	return result;
}

PWM_Result_t PWM_INT_InitTIM11Pins(PWM_Channel_t Channel, PWM_PinsPack_t PinsPack) {
	PWM_Result_t result = PWM_Result_PinNotValid;

	switch (Channel) {
		case PWM_Channel_1:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOB
					GPIO_InitAlternate(GPIOB, GPIO_PIN_9, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM11);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOF
					GPIO_InitAlternate(GPIOF, GPIO_PIN_7, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM11);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		default:
			result = PWM_Result_ChannelNotValid;
			break;
	}
	
	return result;
}

PWM_Result_t PWM_INT_InitTIM12Pins(PWM_Channel_t Channel, PWM_PinsPack_t PinsPack) {
	PWM_Result_t result = PWM_Result_PinNotValid;

	switch (Channel) {
		case PWM_Channel_1:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOB
					GPIO_InitAlternate(GPIOB, GPIO_PIN_14, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM12);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOH
					GPIO_InitAlternate(GPIOH, GPIO_PIN_6, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM12);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		case PWM_Channel_2:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOB
					GPIO_InitAlternate(GPIOB, GPIO_PIN_15, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM12);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOH
					GPIO_InitAlternate(GPIOH, GPIO_PIN_9, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM12);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		default:
			result = PWM_Result_ChannelNotValid;
			break;
	}
	
	return result;
}

PWM_Result_t PWM_INT_InitTIM13Pins(PWM_Channel_t Channel, PWM_PinsPack_t PinsPack) {
	PWM_Result_t result = PWM_Result_PinNotValid;

	switch (Channel) {
		case PWM_Channel_1:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_1, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM13);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOF
					GPIO_InitAlternate(GPIOF, GPIO_PIN_8, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM13);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		default:
			result = PWM_Result_ChannelNotValid;
			break;
	}
	
	return result;
}

PWM_Result_t PWM_INT_InitTIM14Pins(PWM_Channel_t Channel, PWM_PinsPack_t PinsPack) {
	PWM_Result_t result = PWM_Result_PinNotValid;

	switch (Channel) {
		case PWM_Channel_1:
			switch (PinsPack) {
				case PWM_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_7, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM14);
					result = PWM_Result_Ok;
#endif
					break;
				case PWM_PinsPack_2:
#ifdef GPIOF
					GPIO_InitAlternate(GPIOF, GPIO_PIN_9, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM14);
					result = PWM_Result_Ok;
#endif
					break;
				default:
					result = PWM_Result_PinNotValid;
					break;
			}
			break;
		default:
			result = PWM_Result_ChannelNotValid;
			break;
	}
	
	return result;
}

