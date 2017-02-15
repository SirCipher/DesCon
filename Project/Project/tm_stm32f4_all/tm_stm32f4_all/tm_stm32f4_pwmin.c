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
#include "stm32f4_pwmin.h"

/* Private functions */
PWMIN_Result_t PWMIN_INT_InitTIM1Pins(PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack);
PWMIN_Result_t PWMIN_INT_InitTIM2Pins(PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack);
PWMIN_Result_t PWMIN_INT_InitTIM3Pins(PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack);
PWMIN_Result_t PWMIN_INT_InitTIM4Pins(PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack);
PWMIN_Result_t PWMIN_INT_InitTIM5Pins(PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack);
PWMIN_Result_t PWMIN_INT_InitTIM8Pins(PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack);
PWMIN_Result_t PWMIN_INT_InitTIM9Pins(PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack);
PWMIN_Result_t PWMIN_INT_InitTIM10Pins(PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack);
PWMIN_Result_t PWMIN_INT_InitTIM11Pins(PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack);
PWMIN_Result_t PWMIN_INT_InitTIM12Pins(PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack);
PWMIN_Result_t PWMIN_INT_InitTIM13Pins(PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack);
PWMIN_Result_t PWMIN_INT_InitTIM14Pins(PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack);
PWMIN_Result_t PWMIN_INT_InitChannel(PWMIN_t* TIM_Data, PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack);

/* Private variables */
__IO float IC2Value = 0;
__IO float DutyCycle = 0;
__IO float Frequency = 0;
GPIO_InitTypeDef PWMIN_GPIO_InitStruct;
static uint16_t PWMIN_SubPriorities = 0;

PWMIN_Result_t PWMIN_InitTimer(
	TIM_TypeDef* TIMx,
	PWMIN_t* PWMIN_Data,
	PWMIN_Channel_t PWMIN_Channel,
	PWMIN_PinsPack_t PinsPack,
	float MinExpectedFrequency,
	IRQn_Type TIMx_IRQn
) {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	TIM_ICInitTypeDef TIM_ICInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	PWMIN_Result_t res;
	
	/* Check timer */
	if (0
#ifdef TIM6
		|| TIMx == TIM6
#endif
#ifdef TIM7
		|| TIMx == TIM7
#endif
	) {
		/* Invalid timer */
		return PWMIN_Result_TimerNotValid;
	}
	
	/* Save timer and settings */
	PWMIN_Data->__TIM = TIMx;
	
	/* Get properties about timer */
	TIMER_PROPERTIES_GetTimerProperties(PWMIN_Data->__TIM, &PWMIN_Data->__TIM_Data);
	
	/* Generate settings */
	TIMER_PROPERTIES_GenerateDataForWorkingFrequency(&PWMIN_Data->__TIM_Data, MinExpectedFrequency);
	
	/* Enable TIM clock */
	TIMER_PROPERTIES_EnableClock(PWMIN_Data->__TIM);
	
	/* Set timer settings */
	TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
	if (PWMIN_Data->__TIM_Data.MaxPeriod == 0x00010000) {
		TIM_TimeBaseStruct.TIM_Period = 0x0000FFFF; /* 16-bit */
	} else {
		TIM_TimeBaseStruct.TIM_Period = PWMIN_Data->__TIM_Data.MaxPeriod;
	}
	TIM_TimeBaseStruct.TIM_Prescaler = PWMIN_Data->__TIM_Data.Prescaler - 1;
	
	/* Initialize TIM */
	TIM_TimeBaseInit(PWMIN_Data->__TIM, &TIM_TimeBaseStruct);
	
	/* Default GPIO settings */
	PWMIN_GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	PWMIN_GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	PWMIN_GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	PWMIN_GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	
	/* Enable TIMER pin */
	if ((res = PWMIN_INT_InitChannel(PWMIN_Data, PWMIN_Channel, PinsPack)) != PWMIN_Result_Ok) {
		/* Return result */
		return res;
	}

	/* Calculate subpriority */
	for (PWMIN_Data->__SubPriority = 0; PWMIN_Data->__SubPriority < 16; PWMIN_Data->__SubPriority++) {
		/* Check if it is available */
		if (!(PWMIN_SubPriorities & (1 << PWMIN_Data->__SubPriority))) {
			/* Available */
			PWMIN_SubPriorities |= 1 << PWMIN_Data->__SubPriority;
			
			/* Stop the loop */
			break;
		}
	}
	
	/* Enable the TIM global Interrupt */
	NVIC_InitStruct.NVIC_IRQChannel = TIMx_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = PWMIN_PRIORITY;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = PWMIN_Data->__SubPriority;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	/* Set timer settings */
	if (PWMIN_Channel == PWMIN_Channel_1) {
		TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;
	} else {
		TIM_ICInitStruct.TIM_Channel = TIM_Channel_2;
	}
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStruct.TIM_ICFilter = 0x0;

	/* Initialize PWM Input config */
	TIM_PWMIConfig(PWMIN_Data->__TIM, &TIM_ICInitStruct);

	/* Select input trigger */
	if (PWMIN_Channel == PWMIN_Channel_1) {
		TIM_SelectInputTrigger(PWMIN_Data->__TIM, TIM_TS_TI1FP1);
	} else {
		TIM_SelectInputTrigger(PWMIN_Data->__TIM, TIM_TS_TI2FP2);
	}

	/* Select the slave Mode: Reset Mode */
	TIM_SelectSlaveMode(PWMIN_Data->__TIM, TIM_SlaveMode_Reset);
	TIM_SelectMasterSlaveMode(PWMIN_Data->__TIM, TIM_MasterSlaveMode_Enable);

	/* TIM enable counter */
	TIM_Cmd(PWMIN_Data->__TIM, ENABLE);

	/* Enable compare interrupt request */
	if (PWMIN_Channel == PWMIN_Channel_1) {
		TIM_ITConfig(PWMIN_Data->__TIM, TIM_IT_CC1, ENABLE);
	} else {
		TIM_ITConfig(PWMIN_Data->__TIM, TIM_IT_CC2, ENABLE);
	}
	
	/* Return OK */
	return PWMIN_Result_Ok;
}

PWMIN_Result_t PWMIN_Get(PWMIN_t* PWMIN_Data) {
	/* Set new data */
	PWMIN_Data->Frequency = PWMIN_Data->__Freq;
	PWMIN_Data->DutyCycle = PWMIN_Data->__Duty;
	
	/* Reset data for future calls */
	PWMIN_Data->__Freq = 0;
	PWMIN_Data->__Duty = 0;
	
	/* Return Ok */
	return PWMIN_Result_Ok;
}

PWMIN_Result_t PWMIN_InterruptHandler(PWMIN_t* PWMIN_Data) {
	/* Interrupt handler for PWMIN */
	
	if (TIM_GetITStatus(PWMIN_Data->__TIM, TIM_IT_CC1)) {
		/* Read data */		

		/* Clear pending bit */
		TIM_ClearITPendingBit(PWMIN_Data->__TIM, TIM_IT_CC1);
		
		/* Get the Input Capture value */
		IC2Value = (float)TIM_GetCapture1(PWMIN_Data->__TIM);

		if (IC2Value != 0) {
			/* Duty cycle and frequency */
			DutyCycle = ((float)TIM_GetCapture2(PWMIN_Data->__TIM) * 100) / IC2Value;
			Frequency = (float)PWMIN_Data->__TIM_Data.TimerFrequency / (float)(IC2Value * PWMIN_Data->__TIM_Data.Prescaler);
			
			/* Save data */
			PWMIN_Data->__Duty = DutyCycle;
			PWMIN_Data->__Freq = Frequency;
		} else {
			/* Reset data */
			DutyCycle = 0;
			Frequency = 0;
		}
	}
	if (TIM_GetITStatus(PWMIN_Data->__TIM, TIM_IT_CC2)) {
		/* Read data */
		
		/* Clear pending bit */
		TIM_ClearITPendingBit(PWMIN_Data->__TIM, TIM_IT_CC2);
		
		/* Get the Input Capture value */
		IC2Value = (float)TIM_GetCapture2(PWMIN_Data->__TIM);

		if (IC2Value != 0) {
			/* Duty cycle and frequency */
			DutyCycle = ((float)TIM_GetCapture1(PWMIN_Data->__TIM) * 100) / IC2Value;
			Frequency = (float)PWMIN_Data->__TIM_Data.TimerFrequency / (IC2Value * PWMIN_Data->__TIM_Data.Prescaler);
			
			/* Save data */
			PWMIN_Data->__Duty = DutyCycle;
			PWMIN_Data->__Freq = Frequency;
		} else {
			/* Reset data */
			DutyCycle = 0;
			Frequency = 0;
		}
	}
	
	/* Return OK */
	return PWMIN_Result_Ok;
}

/* Private functions */
PWMIN_Result_t PWMIN_INT_InitChannel(PWMIN_t* TIM_Data, PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack) {
#ifdef TIM1
	if (TIM_Data->__TIM == TIM1) {
		return PWMIN_INT_InitTIM1Pins(Channel, PinsPack);
	}
#endif
#ifdef TIM2
	if (TIM_Data->__TIM == TIM2) {
		return PWMIN_INT_InitTIM2Pins(Channel, PinsPack);
	} 
#endif
#ifdef TIM3
	if (TIM_Data->__TIM == TIM3) {
		return PWMIN_INT_InitTIM3Pins(Channel, PinsPack);
	} 
#endif
#ifdef TIM4
	if (TIM_Data->__TIM == TIM4) {
		return PWMIN_INT_InitTIM4Pins(Channel, PinsPack);
	} 
#endif
#ifdef TIM5
	if (TIM_Data->__TIM == TIM5) {
		return PWMIN_INT_InitTIM5Pins(Channel, PinsPack);
	} 
#endif
#ifdef TIM8
	if (TIM_Data->__TIM == TIM8) {
		return PWMIN_INT_InitTIM8Pins(Channel, PinsPack);
	} 
#endif
#ifdef TIM9
	if (TIM_Data->__TIM == TIM9) {
		return PWMIN_INT_InitTIM9Pins(Channel, PinsPack);
	} 
#endif
#ifdef TIM10
	if (TIM_Data->__TIM == TIM10) {
		return PWMIN_INT_InitTIM10Pins(Channel, PinsPack);
	} 
#endif
#ifdef TIM11
	if (TIM_Data->__TIM == TIM11) {
		return PWMIN_INT_InitTIM11Pins(Channel, PinsPack);
	} 
#endif
#ifdef TIM12
	if (TIM_Data->__TIM == TIM12) {
		return PWMIN_INT_InitTIM12Pins(Channel, PinsPack);
	} 
#endif
#ifdef TIM13
	if (TIM_Data->__TIM == TIM13) {
		return PWMIN_INT_InitTIM13Pins(Channel, PinsPack);
	} 
#endif
#ifdef TIM14
	if (TIM_Data->__TIM == TIM14) {
		return PWMIN_INT_InitTIM14Pins(Channel, PinsPack);
	}
#endif
	
	/* Timer is not valid */
	return PWMIN_Result_TimerNotValid;
}

PWMIN_Result_t PWMIN_INT_InitTIM1Pins(PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack) {
	PWMIN_Result_t result = PWMIN_Result_PinNotValid;
	
	switch (Channel) {
		case PWMIN_Channel_1:
			switch (PinsPack) {
				case PWMIN_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_8, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM1);
					result = PWMIN_Result_Ok;
#endif
					break;
				case PWMIN_PinsPack_2:
#ifdef GPIOE
					GPIO_InitAlternate(GPIOE, GPIO_PIN_9, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM1);
					result = PWMIN_Result_Ok;
#endif
					break;
				default:
					result = PWMIN_Result_PinNotValid;
					break;
			}
			break;
		case PWMIN_Channel_2:
			switch (PinsPack) {
				case PWMIN_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_9, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM1);
					result = PWMIN_Result_Ok;
#endif
					break;
				case PWMIN_PinsPack_2:
#ifdef GPIOE
					GPIO_InitAlternate(GPIOE, GPIO_PIN_10, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM1);
					result = PWMIN_Result_Ok;
#endif
					break;
				default:
					result = PWMIN_Result_PinNotValid;
					break;
			}
			break;
		default:
			result = PWMIN_Result_ChannelNotValid;
			break;
	}
	
	return result;
}

PWMIN_Result_t PWMIN_INT_InitTIM2Pins(PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack) {
	PWMIN_Result_t result = PWMIN_Result_PinNotValid;
	
	switch (Channel) {
		case PWMIN_Channel_1:
			switch (PinsPack) {
				case PWMIN_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_0, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM2);
					result = PWMIN_Result_Ok;
#endif
					break;
				case PWMIN_PinsPack_2:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_5, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM2);
					result = PWMIN_Result_Ok;
#endif
					break;
				case PWMIN_PinsPack_3:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_15, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM2);
					result = PWMIN_Result_Ok;
#endif
					break;
				default:
					result = PWMIN_Result_PinNotValid;
					break;
			}
			break;
		case PWMIN_Channel_2:
			switch (PinsPack) {
				case PWMIN_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_1, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM2);
					result = PWMIN_Result_Ok;
#endif
					break;
				case PWMIN_PinsPack_2:
#ifdef GPIOB
					GPIO_InitAlternate(GPIOB, GPIO_PIN_3, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM2);
					result = PWMIN_Result_Ok;
#endif
					break;
				default:
					result = PWMIN_Result_PinNotValid;
					break;
			}
			break;
		default:
			result = PWMIN_Result_ChannelNotValid;
			break;
	}
	
	return result;
}

PWMIN_Result_t PWMIN_INT_InitTIM3Pins(PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack) {
	PWMIN_Result_t result = PWMIN_Result_PinNotValid;
	
	switch (Channel) {
		case PWMIN_Channel_1:
			switch (PinsPack) {
				case PWMIN_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_6, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM3);
					result = PWMIN_Result_Ok;
#endif
					break;
				case PWMIN_PinsPack_2:
#ifdef GPIOB
					GPIO_InitAlternate(GPIOB, GPIO_PIN_4, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM3);
					result = PWMIN_Result_Ok;
#endif
					break;
				case PWMIN_PinsPack_3:
#ifdef GPIOC
					GPIO_InitAlternate(GPIOC, GPIO_PIN_10, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM3);
					result = PWMIN_Result_Ok;
#endif
					break;
				default:
					result = PWMIN_Result_PinNotValid;
					break;
			}
			break;
		case PWMIN_Channel_2:
			switch (PinsPack) {
				case PWMIN_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_7, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM3);
					result = PWMIN_Result_Ok;
#endif
					break;
				case PWMIN_PinsPack_2:
#ifdef GPIOB
					GPIO_InitAlternate(GPIOB, GPIO_PIN_5, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM3);
					result = PWMIN_Result_Ok;
#endif
					break;
				case PWMIN_PinsPack_3:
#ifdef GPIOC
					GPIO_InitAlternate(GPIOC, GPIO_PIN_7, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM3);
					result = PWMIN_Result_Ok;
#endif
					break;
				default:
					result = PWMIN_Result_PinNotValid;
					break;
			}
			break;
		default:
			result = PWMIN_Result_ChannelNotValid;
			break;
	}
	
	return result;
}

PWMIN_Result_t PWMIN_INT_InitTIM4Pins(PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack) {
	PWMIN_Result_t result = PWMIN_Result_PinNotValid;

	switch (Channel) {
		case PWMIN_Channel_1:
			switch (PinsPack) {
				case PWMIN_PinsPack_1:
#ifdef GPIOB
					GPIO_InitAlternate(GPIOB, GPIO_PIN_6, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM4);
					result = PWMIN_Result_Ok;
#endif
					break;
				case PWMIN_PinsPack_2:
#ifdef GPIOD
					GPIO_InitAlternate(GPIOD, GPIO_PIN_12, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM4);
					result = PWMIN_Result_Ok;
#endif
					break;
				default:
					result = PWMIN_Result_PinNotValid;
					break;
			}
			break;
		case PWMIN_Channel_2:
			switch (PinsPack) {
				case PWMIN_PinsPack_1:
#ifdef GPIOB
					GPIO_InitAlternate(GPIOB, GPIO_PIN_7, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM4);
					result = PWMIN_Result_Ok;
#endif
					break;
				case PWMIN_PinsPack_2:
#ifdef GPIOD
					GPIO_InitAlternate(GPIOD, GPIO_PIN_13, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM4);
					result = PWMIN_Result_Ok;
#endif
					break;
				default:
					result = PWMIN_Result_PinNotValid;
					break;
			}
			break;
		default:
			result = PWMIN_Result_ChannelNotValid;
			break;
	}
	
	return result;
}

PWMIN_Result_t PWMIN_INT_InitTIM5Pins(PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack) {
	PWMIN_Result_t result = PWMIN_Result_PinNotValid;

	switch (Channel) {
		case PWMIN_Channel_1:
			switch (PinsPack) {
				case PWMIN_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_0, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM5);
					result = PWMIN_Result_Ok;
#endif
					break;
				case PWMIN_PinsPack_2:
#ifdef GPIOH
					GPIO_InitAlternate(GPIOH, GPIO_PIN_10, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM5);
					result = PWMIN_Result_Ok;
#endif
					break;
				default:
					result = PWMIN_Result_PinNotValid;
					break;
			}
			break;
		case PWMIN_Channel_2:
			switch (PinsPack) {
				case PWMIN_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_1, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM5);
					result = PWMIN_Result_Ok;
#endif
					break;
				case PWMIN_PinsPack_2:
#ifdef GPIOH
					GPIO_InitAlternate(GPIOH, GPIO_PIN_11, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM5);
					result = PWMIN_Result_Ok;
#endif
					break;
				default:
					result = PWMIN_Result_PinNotValid;
					break;
			}
			break;
		default:
			result = PWMIN_Result_ChannelNotValid;
			break;
	}
	
	return result;
}

PWMIN_Result_t PWMIN_INT_InitTIM8Pins(PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack) {
	PWMIN_Result_t result = PWMIN_Result_PinNotValid;

	switch (Channel) {
		case PWMIN_Channel_1:
			switch (PinsPack) {
				case PWMIN_PinsPack_1:
#ifdef GPIOC
					GPIO_InitAlternate(GPIOC, GPIO_PIN_6, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM8);
					result = PWMIN_Result_Ok;
#endif
					break;
				case PWMIN_PinsPack_2:
#ifdef GPIOI
					GPIO_InitAlternate(GPIOI, GPIO_PIN_5, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM8);
					result = PWMIN_Result_Ok;
#endif
					break;
				default:
					result = PWMIN_Result_PinNotValid;
					break;
			}
			break;
		case PWMIN_Channel_2:
			switch (PinsPack) {
				case PWMIN_PinsPack_1:
#ifdef GPIOC
					GPIO_InitAlternate(GPIOC, GPIO_PIN_7, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM8);
					result = PWMIN_Result_Ok;
#endif
					break;
				case PWMIN_PinsPack_2:
#ifdef GPIOI
					GPIO_InitAlternate(GPIOI, GPIO_PIN_6, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM8);
					result = PWMIN_Result_Ok;
#endif
					break;
				default:
					result = PWMIN_Result_PinNotValid;
					break;
			}
			break;
		default:
			result = PWMIN_Result_ChannelNotValid;
			break;
	}
	
	return result;
}

PWMIN_Result_t PWMIN_INT_InitTIM9Pins(PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack) {
	PWMIN_Result_t result = PWMIN_Result_PinNotValid;

	switch (Channel) {
		case PWMIN_Channel_1:
			switch (PinsPack) {
				case PWMIN_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_2, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM9);
					result = PWMIN_Result_Ok;
#endif
					break;
				case PWMIN_PinsPack_2:
#ifdef GPIOE
					GPIO_InitAlternate(GPIOE, GPIO_PIN_5, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM9);
					result = PWMIN_Result_Ok;
#endif
					break;
				default:
					result = PWMIN_Result_PinNotValid;
					break;
			}
			break;
		case PWMIN_Channel_2:
			switch (PinsPack) {
				case PWMIN_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_3, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM9);
					result = PWMIN_Result_Ok;
#endif
					break;
				case PWMIN_PinsPack_2:
#ifdef GPIOE
					GPIO_InitAlternate(GPIOE, GPIO_PIN_6, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM9);
					result = PWMIN_Result_Ok;
#endif
					break;
				default:
					result = PWMIN_Result_PinNotValid;
					break;
			}
			break;
		default:
			result = PWMIN_Result_ChannelNotValid;
			break;
	}
	
	return result;
}

PWMIN_Result_t PWMIN_INT_InitTIM10Pins(PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack) {
	PWMIN_Result_t result = PWMIN_Result_PinNotValid;

	switch (Channel) {
		case PWMIN_Channel_1:
			switch (PinsPack) {
				case PWMIN_PinsPack_1:
#ifdef GPIOB
					GPIO_InitAlternate(GPIOB, GPIO_PIN_8, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM10);
					result = PWMIN_Result_Ok;
#endif
					break;
				case PWMIN_PinsPack_2:
#ifdef GPIOF
					GPIO_InitAlternate(GPIOF, GPIO_PIN_6, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM10);
					result = PWMIN_Result_Ok;
#endif
					break;
				default:
					result = PWMIN_Result_PinNotValid;
					break;
			}
			break;
		default:
			result = PWMIN_Result_ChannelNotValid;
			break;
	}
	
	return result;
}

PWMIN_Result_t PWMIN_INT_InitTIM11Pins(PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack) {
	PWMIN_Result_t result = PWMIN_Result_PinNotValid;

	switch (Channel) {
		case PWMIN_Channel_1:
			switch (PinsPack) {
				case PWMIN_PinsPack_1:
#ifdef GPIOB
					GPIO_InitAlternate(GPIOB, GPIO_PIN_9, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM11);
					result = PWMIN_Result_Ok;
#endif
					break;
				case PWMIN_PinsPack_2:
#ifdef GPIOF
					GPIO_InitAlternate(GPIOF, GPIO_PIN_7, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM11);
					result = PWMIN_Result_Ok;
#endif
					break;
				default:
					result = PWMIN_Result_PinNotValid;
					break;
			}
			break;
		default:
			result = PWMIN_Result_ChannelNotValid;
			break;
	}
	
	return result;
}

PWMIN_Result_t PWMIN_INT_InitTIM12Pins(PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack) {
	PWMIN_Result_t result = PWMIN_Result_PinNotValid;

	switch (Channel) {
		case PWMIN_Channel_1:
			switch (PinsPack) {
				case PWMIN_PinsPack_1:
#ifdef GPIOB
					GPIO_InitAlternate(GPIOB, GPIO_PIN_14, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM12);
					result = PWMIN_Result_Ok;
#endif
					break;
				case PWMIN_PinsPack_2:
#ifdef GPIOH
					GPIO_InitAlternate(GPIOH, GPIO_PIN_6, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM12);
					result = PWMIN_Result_Ok;
#endif
					break;
				default:
					result = PWMIN_Result_PinNotValid;
					break;
			}
			break;
		case PWMIN_Channel_2:
			switch (PinsPack) {
				case PWMIN_PinsPack_1:
#ifdef GPIOB
					GPIO_InitAlternate(GPIOB, GPIO_PIN_15, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM12);
					result = PWMIN_Result_Ok;
#endif
					break;
				case PWMIN_PinsPack_2:
#ifdef GPIOH
					GPIO_InitAlternate(GPIOH, GPIO_PIN_9, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM12);
					result = PWMIN_Result_Ok;
#endif
					break;
				default:
					result = PWMIN_Result_PinNotValid;
					break;
			}
			break;
		default:
			result = PWMIN_Result_ChannelNotValid;
			break;
	}
	
	return result;
}

PWMIN_Result_t PWMIN_INT_InitTIM13Pins(PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack) {
	PWMIN_Result_t result = PWMIN_Result_PinNotValid;

	switch (Channel) {
		case PWMIN_Channel_1:
			switch (PinsPack) {
				case PWMIN_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_1, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM13);
					result = PWMIN_Result_Ok;
#endif
					break;
				case PWMIN_PinsPack_2:
#ifdef GPIOF
					GPIO_InitAlternate(GPIOF, GPIO_PIN_8, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM13);
					result = PWMIN_Result_Ok;
#endif
					break;
				default:
					result = PWMIN_Result_PinNotValid;
					break;
			}
			break;
		default:
			result = PWMIN_Result_ChannelNotValid;
			break;
	}
	
	return result;
}

PWMIN_Result_t PWMIN_INT_InitTIM14Pins(PWMIN_Channel_t Channel, PWMIN_PinsPack_t PinsPack) {
	PWMIN_Result_t result = PWMIN_Result_PinNotValid;

	switch (Channel) {
		case PWMIN_Channel_1:
			switch (PinsPack) {
				case PWMIN_PinsPack_1:
#ifdef GPIOA
					GPIO_InitAlternate(GPIOA, GPIO_PIN_7, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM14);
					result = PWMIN_Result_Ok;
#endif
					break;
				case PWMIN_PinsPack_2:
#ifdef GPIOF
					GPIO_InitAlternate(GPIOF, GPIO_PIN_9, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AF_TIM14);
					result = PWMIN_Result_Ok;
#endif
					break;
				default:
					result = PWMIN_Result_PinNotValid;
					break;
			}
			break;
		default:
			result = PWMIN_Result_ChannelNotValid;
			break;
	}
	
	return result;
}
