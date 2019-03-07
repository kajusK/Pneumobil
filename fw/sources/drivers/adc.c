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
 * @file    adc.c
 * @brief   ADC driver
 *
 * @addtogroup drivers
 * @{
 */

#include "hal.h"
#include "modules/log.h"
#include "drivers/adc.h"

/**  Resolution steps of the ADC 2^bits - 1 */
#define ADCD_RESOLUTION_STEPS 4095U
/** Power supply voltage for measurements without using reference */
#define ADCD_REF_VOLTAGE_MV 3300U

/** Calibration value for internal temperature sensor at 110 deg C */
#define TEMP110_CAL (*((uint16_t*) ((uint32_t) 0x1FFFF7C2)))
/** Calibration value for internal temperature sensor at 30 deg C */
#define TEMP30_CAL (*((uint16_t*) ((uint32_t) 0x1FFFF7B8)))
/** Calibration value for internal reference */
#define REF_CAL (*((uint16_t *) ((uint32_t) 0x1FFFF7BA)))

/** Reference voltage for the adc module */
static uint16_t adcdi_ref_mv = ADCD_REF_VOLTAGE_MV;

/**
 * ADC error callback
 *
 * @param [in] adcp     ADC driver
 * @param [in] err      ADC error flag
 */
static void Adcdi_ErrorCb(ADCDriver *adcp, adcerror_t err)
{
    (void)adcp;
    (void)err;

    Log_Warn(LOG_SOURCE_DRIVER, "ADC error occured, num: %d", err);
}

uint16_t Adcd_RawToMv(uint16_t raw)
{
    return (((uint32_t) adcdi_ref_mv)*raw)/ADCD_RESOLUTION_STEPS;
}

int8_t Adcd_RawToTemp(uint16_t raw)
{
    uint16_t data;

    /** Temperature calibration was taken at 3.3V reference, calibrate to current one */
    data = ((uint32_t) raw * adcdi_ref_mv)/3300;
    /* from datasheet */
    return ((int32_t)data - TEMP30_CAL)*(110-30)/(TEMP110_CAL-TEMP30_CAL);
}

uint16_t Adcd_RefToSupplyMv(uint16_t raw)
{
    return ((uint32_t) 3300*REF_CAL)/raw;
}

void Adcd_AutoConversion(const uint8_t *channels, uint16_t *buffer, uint8_t len,
        bool circular)
{
    uint32_t chsel = 0;
    static ADCConversionGroup adcGroup = {
        false,       /* Circular mode enable */
        0,            /* Number of channels in conversion */
        NULL,           /* Conversion end callback */
        Adcdi_ErrorCb,  /* Conversion error callback */
        ADC_CFGR1_CONT | ADC_CFGR1_RES_12BIT,   /* CFGR1 */
#if (STM32_ADC_SUPPORTS_OVERSAMPLING)
        0,              /* CFGR2 */
#endif
        ADC_TR(0, 0),      /* TR treshold register*/
        ADC_SMPR_SMP_28P5, /* SMPR - 54 cycles long conversion */
        0,                 /* Chanel selection */
    };

    adcGroup.circular = circular;
    adcGroup.num_channels = len;

    for (int i = 0; i < len; i++) {
        chsel |= 1 << channels[i];
    }
    adcGroup.chselr = chsel;

    if (circular) {
        adcStartConversion(&ADCD1, &adcGroup, buffer, len);
    } else {
        adcConvert(&ADCD1, &adcGroup, buffer, len);
    }
}

void Adcd_SetReferenceMv(uint16_t ref_mv)
{
    adcdi_ref_mv = ref_mv;
}

void Adcd_Init(void)
{
    adcStart(&ADCD1, NULL);
    /* Enable reference, temperature sensor and battery measurement */
    adcSTM32EnableVREF();
    adcSTM32EnableTS();
}

/** @} */

