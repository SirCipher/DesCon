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
#include "stm32f4_low_power.h"

void LOWPOWER_SleepUntilInterrupt(uint8_t delay_timer) {
	/* Disable systick */
	if (delay_timer) {
		DELAY_DisableDelayTimer();
	}
	
	/* Wait for interrupt command */
	__WFI();
	
	/* At this stage, MCU will be in sleep mode, and when interrupt wake it up, */
	/* it will begin here */
	
	/* Enable delay timer back */
	if (delay_timer) {
		DELAY_EnableDelayTimer();
	}
}

void LOWPOWER_SleepUntilEvent(void) {
	/* We don't need delay timer disable, because delay timer does not make an event */
	
	/* Wait for event */
	__WFE();
}

void LOWPOWER_Standby(void) {
	/* Clear Standby and wakeup flag */
    PWR_ClearFlag(PWR_FLAG_SB | PWR_FLAG_WU);
	
	/* Go to standby mode */
	PWR_EnterSTANDBYMode();
}

uint8_t LOWPOWER_StandbyReset(void) {
	/* Check Standby Flag */
	if (PWR_GetFlagStatus(PWR_FLAG_SB) != RESET) {
		/* Clear Standby and wakeup flag */
		PWR_ClearFlag(PWR_FLAG_SB | PWR_FLAG_WU);
		
		/* Reset was from wakeup from standy */
		return 1;
	}
	
	/* Reset was not from standby */
	return 0;
}

void LOWPOWER_StopUntilInterrupt(void) {
	/* Go to STOP mode */
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
	
	/* After wakeup, call system init to enable PLL as clock source */
	SystemInit();
}

void LOWPOWER_StopUntilEvent(void) {
	/* Go to STOP mode */
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFE);
	
	/* After wakeup, call system init to enable PLL as clock source */
	SystemInit();
}

void LOWPOWER_EnableWakeUpPin(void) {
	/* Enable Wakeup pin, PA0 */
#if defined(STM32F446xx)
	PWR_WakeUpPinCmd(PWR_WakeUp_Pin1, ENABLE);
#else
	PWR_WakeUpPinCmd(ENABLE);
#endif
}

void LOWPOWER_DisableWakeUpPin(void) {
	/* Disable Wakeup pin, PA0 */
#if defined(STM32F446xx)
	PWR_WakeUpPinCmd(PWR_WakeUp_Pin1, DISABLE);
#else
	PWR_WakeUpPinCmd(DISABLE);
#endif
}

