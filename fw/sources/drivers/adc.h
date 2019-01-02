/*
    BUT pneumobil - Copyright (C) 2018 Jakub Kaderka.

    This file is part of BUT pneumobil ECU.

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

#ifndef __DRIVERS_ADC_H
#define __DRIVERS_ADC_H

#include <types.h>
#include <hal.h>

/** Channel for internal temperature sensor */
#define ADCD_TEMP_CHANNEL 16
/** Channel for internal voltage reference */
#define ADCD_INTREF_CHANNEL 17
/** Channel for Vbat monitoring (divided by 2 internally) */
#define ADCD_VBAT_CHANNEL 18

/**
 * Convert raw adc data to milivolts
 *
 * @param [in] raw      Converted data from adc
 * @return Measured voltage in mV
 */
extern uint16_t Adcd_RawToMv(uint16_t raw);

/**
 * Convert raw adc data from internal temp sensor to temperature
 *
 * Works for 3,3 V reference voltage
 *
 * @param [in] raw  Raw adc data
 * @return temperature in degrees C
 */
extern int8_t Adcd_RawToTemp(uint16_t raw);

/**
 * Calculate Supply voltage from measured internal reference voltage
 *
 * @param [in] raw      Raw value measured from internal reference channel
 * @return      Supply voltage in mV
 */
extern uint16_t Adcd_RefToSupplyMv(uint16_t raw);

/**
 * Run conversion on selected adc channels in ascending order
 *
 * In case continuous is True, the conversion will run forever with data
 * stored in buffer by DMA and the function will return after first conversion
 *
 * @param [in] channels         List of channel numbers to measure
 * @param [out] buffer          Buffer where the data will be stored
 * @param [in] len              Amount of channels (same as buffer length)
 * @param [in] circular         True for circular conversion, false for single
 */
extern void Adcd_AutoConversion(const uint8_t *channels, uint16_t *buffer,
        uint8_t len, bool circular);

/**
 * Set reference voltage for calculations (default is 3300 mV)
 *
 * Correct value can be obtained using Adcd_RefToSupplyMv()
 *
 * @param [in] ref_mv       Measured reference voltage in mV
 * @see Adcd_RefToSupplyMv
 */
extern void Adcd_SetReferenceMv(uint16_t ref_mv);

/**
 * Initialize ADC module
 */
extern void Adcd_Init(void);

#endif

/** @} */
