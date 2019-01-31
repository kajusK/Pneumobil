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
 * @file    application.c
 * @brief   Communication protocol, application layer
 *
 * @addtogroup modules-comm
 * @{
 */

#include <string.h>

#include "utils/assert.h"

#include "modules/log.h"
#include "modules/config.h"
#include "modules/comm/comm.h"
#include "modules/comm/can_transport.h"
#include "modules/comm/uart_transport.h"
#include "modules/comm/session.h"
#include "modules/comm/application.h"

static void Commi_SendLogCan(const log_msg_t *log)
{
    ASSERT_NOT(log == NULL);
    Comm_SendLog(IFACE_CAN, log);
}

static void Commi_SendLogUart(const log_msg_t *log)
{
    ASSERT_NOT(log == NULL);
    Comm_SendLog(IFACE_UART, log);
}

/* ***************************************************************************
 * Application layer module local functions
 *************************************************************************** */
comm_error_t Comm_GetSysStatus(comm_sys_status_t *status)
{
    ASSERT_NOT(status == NULL);

    //TODO
    status->device_id = COMM_MY_ID;
    status->state = 0;
    status->uptime_s = 0;
    status->core_temp_c = 0;
    status->core_voltage_mv = 0;

    return COMM_OK;
}

comm_error_t Comm_SetLogMask(const comm_log_mask_t *payload)
{
    ASSERT_NOT(payload == NULL);

    switch (payload->interface) {
        case COMM_LOG_IFACE_CAN:
            Log_UpdateSubscription(Commi_SendLogCan, payload->severity);
            break;
        case COMM_LOG_IFACE_UART:
            Log_UpdateSubscription(Commi_SendLogUart, payload->severity);
            break;
        default:
            return COMM_ERR_INCORRECT_PARAM;
            break;
    }
    return COMM_OK;
}

comm_error_t Comm_GetLogMask(uint8_t interface, comm_log_mask_t *response)
{
    ASSERT_NOT(response == NULL);

    switch (interface) {
        case COMM_LOG_IFACE_CAN:
            Log_GetSubscription(Commi_SendLogCan, response->severity);
            break;
        case COMM_LOG_IFACE_UART:
            Log_GetSubscription(Commi_SendLogUart, response->severity);
            break;
        default:
            return COMM_ERR_INCORRECT_PARAM;
            break;
    }
    return COMM_OK;
}

comm_error_t Comm_SetConfig(const comm_config_item_t *payload)
{
    ASSERT_NOT(payload == NULL);
    switch (payload->type) {
        case COMM_CONFIG_TYPE_BOOL:
            if (payload->id >= CONFIG_BOOL_COUNT) {
                return COMM_ERR_INCORRECT_PARAM;
            }
            Config_SetBool(payload->id, (bool) payload->u32);
            break;
        case COMM_CONFIG_TYPE_UINT:
            if (payload->id >= CONFIG_UINT_COUNT) {
                return COMM_ERR_INCORRECT_PARAM;
            }
            Config_SetUint(payload->id, (bool) payload->u32);
            break;

        default:
            return COMM_ERR_INCORRECT_PARAM;
            break;
    }
    return COMM_OK;
}

comm_error_t Comm_GetConfig(uint8_t id, uint8_t type,
        comm_config_item_t *response)
{
    ASSERT_NOT(response == NULL);
    response->id = id;
    response->type = type;

    switch (type) {
        case COMM_CONFIG_TYPE_BOOL:
            if (id >= CONFIG_BOOL_COUNT) {
                return COMM_ERR_INCORRECT_PARAM;
            }
            response->u32 = Config_GetBool(id);
            break;
        case COMM_CONFIG_TYPE_UINT:
            if (id >= CONFIG_UINT_COUNT) {
                return COMM_ERR_INCORRECT_PARAM;
            }
            response->u32 = Config_GetUint(id);
            break;

        default:
            return COMM_ERR_INCORRECT_PARAM;
            break;
    }
    return COMM_OK;
}

comm_error_t Comm_ResetConfig(void)
{
    Config_Reset();
    return COMM_OK;
}

comm_error_t Comm_LogMessage(uint16_t len, comm_node_t node,
        const comm_log_msg_t *payload)
{
#ifndef BOARD_HMI
    (void) len;
    (void) node;
    (void) payload;
#endif
    return COMM_OK;
}

/* ***************************************************************************
 * HMI specific commands
 *************************************************************************** */
#ifdef BOARD_HMI
void Comm_CarState(const comm_car_state_t *payload)
{
    ASSERT_NOT(payload == NULL);

    State_UpdateCarState(payload->speed_dms, payload->speed_avg_dms,
            payload->distance_m, payload->mode);
}

void Comm_CarIO(const comm_car_io_t *payload)
{
    ASSERT_NOT(payload == NULL);

    //TODO make sure valves are set correctly
    State_UpdateCarIoState((payload->valves >> 6) & 0x03, /* valve front 1 */
                (payload->valves >> 4) & 0x03,      /* valve front 2 */
                (payload->valves >> 2) & 0x03,      /* valve back 1 */
                payload->valves & 0x03,             /* valve back 2 */
                (payload->endstops >> 1) & 0x01,    /* endstop front */
                payload->endstops & 0x01,           /* endstop back */
                payload->inputs & 0x01,             /* throttle */
                (payload->inputs << 1) & 0x01,      /* brake */
                (payload->inputs << 2) & 0x01,      /* horn */
                (payload->inputs << 3) & 0x01,      /* shifting */
                payload->gear);
}

void Comm_PneuState(const comm_pneu_state_t *payload)
{
    ASSERT_NOT(payload == NULL);

    State_UpdatePneuState(payload->press1_kpa, payload->press2_kpa,
            payload->press3_kpa, payload->piston_pct);
}

void Comm_BatteryState(const comm_battery_state_t *payload)
{
    ASSERT_NOT(payload == NULL);

    State_UpdateBattery(payload->bat1_mv, payload->bat2_mv, payload->current_ma,
            payload->charge_pct);
}

void Comm_PSUCurrent(const comm_psu_current_t *payload)
{
    ASSERT_NOT(payload == NULL);

    State_UpdatePSUCurrent(payload->current5v_ma, payload->current12v_ma,
            payload->current24v_ma);
}

void Comm_PSUVoltage(const comm_psu_voltage_t *payload)
{
    ASSERT_NOT(payload == NULL);

    State_UpdatePSUVoltage(payload->voltage5v_mv, payload->voltage12v_mv,
            payload->voltage24v_mv);
}
#endif // BOARD_HMI

/* ***************************************************************************
 * ECU specific commands
 *************************************************************************** */
#ifdef BOARD_ECU
comm_error_t Comm_StartRace(uint8_t mode)
{
    if (Race_Start(mode)) {
        return COMM_OK;
    }
    return COMM_ERR_INCORRECT_PARAM;
}

comm_error_t Comm_EcuDebug(const comm_ecu_debug_t *payload)
{
    ecu_valves_t valves;

    if (Race_GetMode() != RACE_MODE_DEBUG) {
        return COMM_ERR_INCORRECT_PARAM;
    }

    valves.front1 = (payload->valves >> 6) & 0x03;
    valves.front2 = (payload->valves >> 4) & 0x03;
    valves.back1 = (payload->valves >> 2) & 0x03;
    valves.back2 = payload->valves & 0x03;
    ECU_ValvesSet(&valves);

    ECU_SetHorn(payload->outputs & 0x01);
    ECU_SetBrakeLight(payload->outputs & 0x02);
    ECU_SetOut1(payload->outputs & 0x04);
    ECU_SetOut1(payload->outputs & 0x08);

    return COMM_OK;
}

comm_error_t Comm_EcuUserIo(const comm_ecu_user_io_t *payload)
{
    ECU_SetHorn(payload->horn);
    ECU_SetBrakeLight(payload->brake);
    ECU_SetOut1(payload->out1);
    ECU_SetOut2(payload->out2);

    return COMM_OK;
}
#endif

/* ***************************************************************************
 * Application layer module global functions
 *************************************************************************** */
#ifdef BOARD_HMI
bool Comm_SendEcuStartRace(state_race_mode_t mode)
{
    return Comm_SendPacketCmd(COMM_NODE_ECU, COMM_CMD_START_RACE,
            (uint8_t *) &mode, 1, NULL, 0);
}

bool Comm_SendEcuDebug(state_valve_t front1, state_valve_t front2,
        state_valve_t back1, state_valve_t back2, bool horn, bool brake,
        bool out1, bool out2)
{
    comm_ecu_debug_t debug;

    debug.outputs = horn | (brake << 1) | (out1 << 2) | (out2 << 3);
    debug.valves = (front1 & 0x03) << 6;
    debug.valves |= (front2 & 0x03) << 4;
    debug.valves |= (back1 & 0x03) << 2;
    debug.valves |= (back2 & 0x03);

    return Comm_SendPacketCmd(COMM_NODE_ECU, COMM_CMD_ECU_DEBUG,
            (uint8_t *) &debug, sizeof(debug), NULL, 0);
}

bool Comm_SendEcuUserIo(bool horn, bool brake, bool out1, bool out2)
{
    comm_ecu_user_io_t io;

    io.brake = brake;
    io.horn = horn;
    io.out1 = out1;
    io.out2 = out2;

    return Comm_SendPacketCmd(COMM_NODE_ECU, COMM_CMD_ECU_USER_IO,
            (uint8_t *) &io, sizeof(io), NULL, 0);
}
#endif

#ifdef BOARD_ECU
void Comm_SendCarState(uint16_t speed_dms, uint16_t speed_avg_dms,
        uint16_t distance_m, race_mode_t race_mode)
{
    comm_car_state_t state;

    state.speed_dms = speed_dms;
    state.speed_avg_dms = speed_avg_dms;
    state.distance_m = distance_m;
    state.mode = race_mode;

    Comm_SendPacket(COMM_CMD_CAR_STATE, (uint8_t *)&state, sizeof(state));
}

void Comm_SendCarIO(const ecu_inputs_t *inputs, const ecu_valves_t *valves,
        uint8_t gear)
{
    comm_car_io_t io;
    ASSERT_NOT(valves == NULL || inputs == NULL);

    io.valves = (valves->front1 & 0x03) << 6;
    io.valves |= (valves->front2 & 0x03) << 4;
    io.valves |= (valves->back1 & 0x03) << 2;
    io.valves |= valves->back2 & 0x03;

    io.endstops = (inputs->endstop_front & 0x01) << 1;
    io.endstops |= inputs->endstop_back & 0x01;
    io.inputs = (inputs->shifting & 0x01) << 3;
    io.inputs |= (inputs->horn & 0x01) << 2;
    io.inputs |= (inputs->brake & 0x01) << 1;
    io.inputs |= inputs->throttle & 0x01;

    io.gear = gear;

    Comm_SendPacket(COMM_CMD_CAR_IO, (uint8_t *)&io, sizeof(io));
}

void Comm_SendPneuState(uint16_t press1_kpa, uint16_t press2_kpa,
        uint16_t press3_kpa, uint8_t piston_pct)
{
    comm_pneu_state_t state;

    state.press1_kpa = press1_kpa;
    state.press2_kpa = press2_kpa;
    state.press3_kpa = press3_kpa;
    state.piston_pct = piston_pct;

    Comm_SendPacket(COMM_CMD_PNEU_STATE, (uint8_t *)&state, sizeof(state));
}
#endif

#ifdef BOARD_PSU
void Comm_SendBatteryState(uint16_t b1_mv, uint16_t b2_mv, uint16_t cur_ma,
        uint8_t charge_pct)
{
    comm_battery_state_t state;

    state.bat1_mv = bat1_mv;
    state.bat2_mv = bat2_mv;
    state.current_ma = current_ma;
    state.charge_pct = charge_pct;

    Comm_SendPacket(COMM_CMD_BATTERY_STATE, (uint8_t *)&state, sizeof(state));
}

void Comm_SendPSUCurrent(uint16_t v5_ma, uint16_t v12_ma, uint16_t v24_ma)
{
    comm_psu_current_t cur;

    cur.current5v_ma = v5_ma;
    cur.current12v_ma = v12_ma;
    cur.current24v_ma = v24_ma;

    Comm_SendPacket(COMM_CMD_PSU_CURRENT, (uint8_t *)&cur, sizeof(cur));
}

void Comm_SendPSUVoltage(uint16_t v5_mv, uint16_t v12_mv, uint16_t v24_mv)
{
    comm_psu_voltage_t volt;

    volt.voltage5v_mv = v5_mv;
    volt.voltage12v_mv = v12_mv;
    volt.voltage24v_mv = v24_mv;

    Comm_SendPacket(COMM_CMD_PSU_voltage, (uint8_t *)&volt, sizeof(cur));
}
#endif

void Comm_Init(void)
{
    log_severity_t severity[LOG_SOURCE_COUNT];

    Comm_SessionInit();
    Comm_CanInit();
    Comm_UartInit();

    for (int i = 0; i < LOG_SOURCE_COUNT; i++) {
        severity[i] = LOG_SEVERITY_WARNING;
    }
    severity[LOG_SOURCE_SYSTEM] = LOG_SEVERITY_INFO;
    severity[LOG_SOURCE_ECU] = LOG_SEVERITY_INFO;
    Log_Subscribe(Commi_SendLogCan, severity);

    for (int i = 0; i < LOG_SOURCE_COUNT; i++) {
        severity[i] = LOG_SEVERITY_INFO;
    }
    Log_Subscribe(Commi_SendLogUart, severity);
}

/** @} */
