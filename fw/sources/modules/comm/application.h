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
 * @file    application.h
 * @brief   Communication protocol, application layer
 *
 * @addtogroup modules-comm
 * @{
 */

#ifndef __MODULES_COMM_APPLICATION_H
#define __MODULES_COMM_APPLICATION_H

#include <types.h>
#include "modules/comm/comm.h"
#include "modules/comm/presentation.h"

extern comm_error_t Comm_GetSysStatus(comm_sys_status_t *status);
extern comm_error_t Comm_SetLogMask(const comm_log_mask_t *payload);
extern comm_error_t Comm_GetLogMask(uint8_t interface,
    comm_log_mask_t *response);
extern comm_error_t Comm_SetConfig(const comm_config_item_t *payload);
extern comm_error_t Comm_GetConfig(uint8_t id, uint8_t type,
        comm_config_item_t *response);
extern comm_error_t Comm_ResetConfig(void);
extern comm_error_t Comm_LogMessage(uint16_t len, comm_node_t node,
        const comm_log_msg_t *payload);

#ifdef BOARD_HMI
extern void Comm_CarState(const comm_car_state_t *payload);
extern void Comm_CarIO(const comm_car_io_t *payload);
extern void Comm_PneuState(const comm_pneu_state_t *payload);
extern void Comm_BatteryState(const comm_battery_state_t *payload);
extern void Comm_PSUCurrent(const comm_psu_current_t *payload);
extern void Comm_PSUVoltage(const comm_psu_voltage_t *payload);
#endif //BOARD_HMI

#endif

/** @} */
