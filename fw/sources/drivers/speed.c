/*
    BUT pneumobil - Copyright (C) 2018 Jakub Kaderka.

    This file is part of BUT pneumobil.

    BUT pneumobil is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    BUT pneumobil is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    speed.c
 * @brief   Speed and distance sensor
 *
 * @addtogroup drivers
 * @{
 */

#include <hal.h>

#include "drivers/speed.h"

/** GPT peripheral to be used for timing the speed measurements */
#define PERIOD_GPT GPTD6

/** Timer to be used for pulses counting */
#define SPEED_COUNTER_TMR JOIN(STM32_TIM, TIM_SPEED_CNT)

static stm32_tim_t *speeddi_tim = SPEED_COUNTER_TMR;
static volatile uint32_t speeddi_dist_cnt = 0;
static volatile uint32_t speeddi_speed_cnt = 0;
static volatile uint16_t speeddi_mm_per_pulse = 0;

static void Speeddi_TimerCallback(struct GPTDriver *gptd)
{
    (void) gptd;

    uint32_t value = speeddi_tim->CNT;
    speeddi_tim->CNT -= value;

    speeddi_dist_cnt += value;
    speeddi_speed_cnt = value;
}

void Speedd_Reset(void)
{
    speeddi_dist_cnt = 0;
}

uint32_t Speedd_GetDistanceM(void)
{
    return (speeddi_dist_cnt * speeddi_mm_per_pulse)/1000;
}

uint32_t Speedd_GetSpeedDms(void)
{
    return ((speeddi_speed_cnt * speeddi_mm_per_pulse)*10)/gptGetIntervalX(&PERIOD_GPT);
}

void Speedd_Init(uint16_t mm_per_pulse)
{
    /* 1 kHz clock - period will be in ms */
    static const GPTConfig gptConf = {
        1000,
        Speeddi_TimerCallback,
        0,
        0
    };

    gptStart(&PERIOD_GPT, &gptConf);

    speeddi_mm_per_pulse = mm_per_pulse;

    /* Configure the counting timer */
    JOIN(rccEnableTIM, TIM_SPEED_CNT)(true);
    JOIN(rccResetTIM, TIM_SPEED_CNT)();
    speeddi_tim->CR1  = 0; /* Counter disabled for now */
    speeddi_tim->CR2  = 0;
    /* Channel 1 as input, filtered by 8 fDTS */
    speeddi_tim->CCMR1 = STM32_TIM_CCMR1_CC1S(0x1) | STM32_TIM_CCMR1_IC1F(0x8);
    /* Active on rising edge */
    speeddi_tim->CCER = 0;
    /* External clock mode on input 1 */
    speeddi_tim->SMCR  = STM32_TIM_SMCR_SMS(0x7) | STM32_TIM_SMCR_TS(0x5);
    speeddi_tim->PSC  = 0; /* Prescaler */
    speeddi_tim->ARR = (uint32_t)(-1U); /* Reload at this value */
    speeddi_tim->CNT = 0;

    /* Start the timer */
    speeddi_tim->CR1 = STM32_TIM_CR1_CEN;
    /* periodically call the timer callback, period in ms */
    gptStartContinuous(&PERIOD_GPT, SPEEDD_MEAS_PERIOD_MS);
}

/** @} */

