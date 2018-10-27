
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
 * @file    encoder.h
 * @brief   Quadrature encoder driver
 *
 * @addtogroup drivers
 * @{
 */

#ifndef __DRIVERS_ENCODER_H
#define __DRIVERS_ENCODER_H

#include <types.h>

/**
 * Get Current encoder value
 *
 * @return Encoder value
 */
extern int16_t Encoderd_Get(void);

/**
 * Set encoder counter to given value
 *
 * @param [in] value    Encoder value
 */
extern void Encoderd_Set(int16_t value);

/**
 * Invert counting direction of the encoder
 */
extern void Encoderd_InvertDirection(void);

/**
 * Initialize quadrature encoder driver
 */
extern void Encoderd_Init(void);

#endif

/** @} */
