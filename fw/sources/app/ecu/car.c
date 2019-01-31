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
 * @file    ecu/car.c
 * @brief   Car data acquisition
 *
 * @addtogroup app-ecu
 * @{
 */

#include <utils/assert.h>
#include <modules/config.h>
#include <drivers/adc.h>
#include <drivers/speed.h>

#include "race.h"
#include "car.h"

/** ADC results after measurement in order from lowest to highest channel id */
typedef enum {
    ADC_RES_PRESSURE_6,
    ADC_RES_PRESSURE_5,
    ADC_RES_PRESSURE_4,
    ADC_RES_PRESSURE_1,
    ADC_RES_24V,
    ADC_RES_12V,
    ADC_RES_CUR_LOOP,
    ADC_RES_PRESSURE_3,
    ADC_RES_PRESSURE_2,
    ADC_RES_TEMP,
    ADC_RES_INTREF,
    ADC_RES_COUNT,
} adc_res_t;

/** Definiton of ADC channels that will be measured */
static const uint8_t cari_adc_channels[] = {
    CHN_PRESSURE_6,
    CHN_PRESSURE_5,
    CHN_PRESSURE_4,
    CHN_PRESSURE_1,
    CHN_ADC_24V,
    CHN_ADC_12V,
    CHN_CUR_LOOP_IN,
    CHN_PRESSURE_3,
    CHN_PRESSURE_2,
    ADCD_TEMP_CHANNEL,
    ADCD_INTREF_CHANNEL,
};

/** Buffer for adc data, filled by DMA */
static uint16_t cari_adc_raw[ADC_RES_COUNT];

/**
 * Convert voltage in mV to pressure in kPa
 *
 * @param [in] meas_mv  Measured voltage correcponding to pressure
 * @return Pressure in kPa
 */
static uint16_t Cari_MvTokPa(uint16_t meas_mv)
{
    /*
     * Max pressure is 1000 kPa, max output voltage is 10 V. With
     * resistor divider from 3300 a 6800 resistors, the maximum measured
     * voltage is
     * Vmax = 10 * 3300 / (3300+6800) = 3267,3 mV
     * mV per kPa = Vmax / Pmax = 3267 / 1000 = 3,267 mV
     * Pressure is voltage / mv_per_kPa
     */
    return ((uint32_t) meas_mv*10000)/32673;
}

void Car_GetState(car_data_t *state)
{
    uint16_t ref_mv;
    uint16_t voltages[ADC_RES_COUNT];
    ASSERT_NOT(state == NULL);

    /* Set measured system voltage as adc reference value */
    ref_mv = Adcd_RefToSupplyMv(cari_adc_raw[ADC_RES_INTREF]);
    Adcd_SetReferenceMv(ref_mv);

    for (int i = 0; i < ADC_RES_COUNT; i++) {
        voltages[i] = Adcd_RawToMv(cari_adc_raw[i]);
    }

    state->pressure_kpa[0] = Cari_MvTokPa(voltages[ADC_RES_PRESSURE_1]);
    state->pressure_kpa[1] = Cari_MvTokPa(voltages[ADC_RES_PRESSURE_2]);
    state->pressure_kpa[2] = Cari_MvTokPa(voltages[ADC_RES_PRESSURE_3]);
    state->pressure_kpa[3] = Cari_MvTokPa(voltages[ADC_RES_PRESSURE_4]);
    state->pressure_kpa[4] = Cari_MvTokPa(voltages[ADC_RES_PRESSURE_5]);
    state->pressure_kpa[5] = Cari_MvTokPa(voltages[ADC_RES_PRESSURE_6]);

    /* 22k and 3k3 divider */
    state->voltage24_mv = ((uint32_t) voltages[ADC_RES_24V] * (220+33))/33;
    /* 33k and 12k divider */
    state->voltage12_mv = ((uint32_t) voltages[ADC_RES_12V] * (33+12))/12;
    state->voltage3p3_mv = ref_mv;
    state->sys_temp_deg = Adcd_RawToTemp(cari_adc_raw[ADC_RES_TEMP]);

    state->speed_dms = Speedd_GetSpeedDms();
    state->distance_m = Speedd_GetDistanceM();
    state->speed_avg_dms = (state->distance_m * 10) / (Race_GetDurationMs() / 1000);
}

void Car_Init(void)
{
    uint16_t mmPerPulse;
    uint32_t diameter = Config_GetUint(CONFIG_UINT_WHEEL_DIA_MM);
    uint32_t pulsesRotation = Config_GetUint(CONFIG_UINT_PULSES_PER_WHEEL_ROTATION);

    /*
     * Distance traveled per one sensor pulse in milimeters
     *
     * Depends on wheeel diameter and amount of pulses generated during rotation
     * dpp = 3.14*d/pulses_per_rotation
     */
    mmPerPulse = ((3142 * diameter) / pulsesRotation)/1000;
    Speedd_Init(mmPerPulse);

    Adcd_Init();
    Adcd_AutoConversion(cari_adc_channels, cari_adc_raw, ADC_RES_COUNT, true);
}

/** @} */
