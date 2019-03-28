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
 * @file    encoder.c
 * @brief   Quadruple encoder driver
 *
 * @addtogroup drivers
 * @{
 */

#include <hal.h>

#include "drivers/encoder.h"

#define QEID JOIN(QEID, TIM_ROT_ENC_A)

static void Encoderdi_Start(bool invert)
{
    static QEIConfig config;
    config.mode = QEI_MODE_QUADRATURE;
    config.resolution = QEI_SINGLE_EDGE;
    if (invert) {
        config.dirinv = QEI_DIRINV_TRUE;
    } else {
        config.dirinv = QEI_DIRINV_FALSE;
    }
    config.overflow = QEI_OVERFLOW_WRAP;

    qeiStart(&QEID, &config);
    qeiEnable(&QEID);
}

int16_t Encoderd_Get(void)
{
    return qeiGetCount(&QEID);
}

void Encoderd_Set(int16_t value)
{
    qeiSetCount(&QEID, value);
}

void Encoderd_InvertDirection(void)
{
    qeiDisable(&QEID);
    qeiStop(&QEID);
    Encoderdi_Start(true);
}

void Encoderd_Init(void)
{
    Encoderdi_Start(false);
}

/** @} */

