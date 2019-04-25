/*
    BUT pneumobil - Copyright (C) 2019 Jakub Kaderka.

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
 * @file    psu/psu.c
 * @brief   PSU logic
 *
 * @addtogroup app-psu
 * @{
 */

#include <ch.h>
#include <hal.h>
#include <utils/assert.h>
#include <modules/log.h>
#include <modules/comm/comm.h>
#include <drivers/adc.h>

#include "psu.h"

#define PSU_THREAD_PRIO NORMALPRIO
/** Stack and stuff for thread */
THD_WORKING_AREA(psui_thread_area, 512);

/** Gain of the current measurement IO */
#define GAIN_CUR_BAT 20
#define GAIN_CUR 50

/** current sense resistor size */
#define CUR_SENSE_MOHM 33

/** ADC results after measurement in order from lowest to highest channel id */
typedef enum {
    ADC_RES_U_B2,
    ADC_RES_U_B1,
    ADC_RES_I_24,
    ADC_RES_I_BAT,
    ADC_RES_I_12,
    ADC_RES_U_12,
    ADC_RES_U_5,
    ADC_RES_I_5,
    ADC_RES_TEMP,
    ADC_RES_INTREF,
    ADC_RES_COUNT,
} adc_res_t;

/** Definiton of ADC channels that will be measured */
static const uint8_t psui_adc_channels[] = {
    CHN_U_B2,
    CHN_U_B1,
    CHN_I_24,
    CHN_I_BAT,
    CHN_I_12,
    CHN_U_12,
    CHN_U_5,
    CHN_I_5,
    ADCD_TEMP_CHANNEL,
    ADCD_INTREF_CHANNEL,
};

typedef struct {
    char name[10];
    uint16_t value;
    uint16_t min;
    uint16_t max;
    bool reported;
} psu_range_t;

/** Lookups for charge of 3 cell lipoly voltage */
static const uint16_t psui_charge_lookup[][2] = {
    { 9820, 0 },
    { 11060, 10 },
    { 11180, 20 },
    { 11300, 30 },
    { 11390, 40 },
    { 11510, 50 },
    { 11620, 60 },
    { 11860, 70 },
    { 12070, 80 },
    { 12330, 90 },
    { 12600, 100 }
};

/** Buffer for adc data, filled by DMA */
static uint16_t psui_adc_raw[ADC_RES_COUNT];

/** Valid ranges for voltage sources */
static psu_range_t psui_range_voltages[] = {
    {"Bat1", 0, 11000, 13000, false},
    {"Bat2", 0, 11000, 13000, false},
    {"5 V", 0, 4500, 5500, false},
    {"12 V", 0, 11000, 13000, false},
    {"24 V", 0, 26000, 20000, false},
};

/** Valid ranges for current branches */
static psu_range_t psui_range_currents[] = {
    {"Battery", 0, 6000, 200, false},
    {"5 V", 0, 2000, 100, false},
    {"12 V", 0, 2000, 50, false},
    {"24 V", 0, 2000, 50, false},
};

/**
 * Get remaining battery charge
 *
 * @param [in] bat_mv  Battery voltage
 * @return Charge in percent (0-100)
 */
static uint8_t Psui_GetCharge(uint16_t bat_mv)
{
    int i;
    uint16_t x1, x2;
    uint16_t y1, y2;
    uint8_t size = sizeof(psui_charge_lookup)/sizeof(psui_charge_lookup[0]);

    if (bat_mv <= psui_charge_lookup[0][0]){
        return 0;
    }
    if (bat_mv >= psui_charge_lookup[size-1][0]) {
        return 100;
    }

    for (i = 0; i < size; i++) {
        if (bat_mv <= psui_charge_lookup[i][0]) {
            break;
        }
    }

    if (i == 0) {
        i = 1;
    } else if (i == size) {
        i = size - 1;
    }

    x1 = psui_charge_lookup[i-1][0];
    x2 = psui_charge_lookup[i][0];
    y1 = psui_charge_lookup[i-1][1];
    y2 = psui_charge_lookup[i][1];

    return y1 + ((uint32_t)(bat_mv - x1)*(y2-y1))/(x2 - x1);
}

/**
 * Convert raw measurement of current to corresponding value in mA
 *
 * @param [in] raw      Raw data from ADC
 * @param [in] gain     Gain of the current amplifier
 * @return  Current in mA
 */
static uint16_t Psui_Raw2mA(uint16_t raw_volt, uint8_t gain)
{
    uint32_t voltage = Adcd_RawToMv(raw_volt);

    return (voltage * 1000) / CUR_SENSE_MOHM / gain;
}

/**
 * Check if all values are in given ranges
 *
 * @param [in,out] ranges       Structure with required ranges
 * @param [in] len              Amount of records in ranges
 * @param [in] fmt              Format for error message (name, min, max, value)
 */
static bool Psui_CheckRanges(psu_range_t *ranges, uint16_t len, const char *fmt)
{
    bool error = false;
    ASSERT_NOT(ranges == NULL);

    for (int i = 0; i < len; i++) {
        if (ranges[i].value <= ranges[i].max &&
                ranges[i].value >= ranges[i].min) {
            ranges[i].reported = false;
            continue;
        }

        error = true;
        if (!ranges[i].reported) {
            ranges[i].reported = true;
            Log_Error(LOG_SOURCE_APP, fmt, ranges[i].name, ranges[i].min,
                    ranges[i].max, ranges[i].value);
        }
    }

    return error;
}

/**
 * Process the measured adc data and send them to other modules
 */
static void Psui_ProcessData(void)
{
    uint16_t bat1, bat2;
    uint16_t u12, u5, u24;
    uint16_t i5, i12, i24, ibat;
    uint8_t charge;
    bool currentError;
    bool voltageError;

    Adcd_SetReferenceMv(Adcd_RefToSupplyMv(psui_adc_raw[ADC_RES_INTREF]));

    bat1 = (Adcd_RawToMv(psui_adc_raw[ADC_RES_U_B1]) * (uint32_t) 133) / 33;
    u24 = (Adcd_RawToMv(psui_adc_raw[ADC_RES_U_B2]) * (uint32_t) 377) / 47;
    bat2 = u24 - bat1;
    u5 = (Adcd_RawToMv(psui_adc_raw[ADC_RES_U_5]) * (uint32_t) 30) / 18;
    u12 = (Adcd_RawToMv(psui_adc_raw[ADC_RES_U_12]) * (uint32_t) 133) / 33;

    ibat = Psui_Raw2mA(psui_adc_raw[ADC_RES_I_BAT], GAIN_CUR_BAT);
    i5 = Psui_Raw2mA(psui_adc_raw[ADC_RES_I_5], GAIN_CUR);
    i12 = Psui_Raw2mA(psui_adc_raw[ADC_RES_I_12], GAIN_CUR);
    i24 = Psui_Raw2mA(psui_adc_raw[ADC_RES_I_24], GAIN_CUR);

    charge = Psui_GetCharge(u24/2);

    Comm_SendBatteryState(bat1, bat2, ibat, charge);
    Comm_SendPSUCurrent(i5, i12, i24);
    Comm_SendPSUVoltage(u5, u12, u24);

    psui_range_voltages[0].value = bat1;
    psui_range_voltages[1].value = bat2;
    psui_range_voltages[2].value = u5;
    psui_range_voltages[3].value = u12;
    psui_range_voltages[4].value = u24;

    psui_range_voltages[0].value = ibat;
    psui_range_voltages[1].value = i5;
    psui_range_voltages[2].value = i12;
    psui_range_voltages[3].value = i24;

    voltageError = !Psui_CheckRanges(psui_range_voltages,
                sizeof(psui_range_voltages)/sizeof(psui_range_voltages[0]),
                "Voltage %s out of range (%d - %d) mV - %d mV");
    currentError = !Psui_CheckRanges(psui_range_currents,
                sizeof(psui_range_currents)/sizeof(psui_range_currents[0]),
                "Current %s out of range (%d - %d) mA - %d mA");

    palWriteLine(LINE_LED_VOLTAGE, voltageError);
    palWriteLine(LINE_LED_CURRENT, currentError);
}

static THD_FUNCTION(Psu_Thread, arg)
{
    (void) arg;
    uint16_t ref_mv;

    while (true) {
        /* Set measured system voltage as adc reference value */
        ref_mv = Adcd_RefToSupplyMv(psui_adc_raw[ADC_RES_INTREF]);
        Adcd_SetReferenceMv(ref_mv);
        Psui_ProcessData();
        Comm_SendSystemStatus();

        chThdSleepMilliseconds(100);
    }
}

void Psu_Init(void)
{
    Adcd_AutoConversion(psui_adc_channels, psui_adc_raw, ADC_RES_COUNT, true);

    (void) chThdCreateStatic(psui_thread_area, sizeof(psui_thread_area),
                PSU_THREAD_PRIO, Psu_Thread, NULL);
}

/** @} */
