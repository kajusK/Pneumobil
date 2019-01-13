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
 * @file    storage.c
 * @brief   Storing config data to eeprom
 *
 * @addtogroup modules
 * @{
 */

#include <ch.h>

#include "version.h"
#include "utils/assert.h"
#include "modules/log.h"
#include "modules/config.h"
#include "utils/crc.h"
#include "drivers/eeprom.h"

#include "modules/storage.h"

typedef struct {
    uint8_t major;
    uint8_t minor;
} storage_version_t;

#define STORAGE_CRC_SIZE        1
#define STORAGE_VERSION_SIZE   sizeof(storage_version_t)
#define STORAGE_UINT_SIZE       sizeof(config_item_uint)
#define STORAGE_BOOL_SIZE       sizeof(config_item_bool)

#define STORAGE_ADDR_VERSION    0
#define STORAGE_ADDR_UINT       (STORAGE_ADDR_VERSION + STORAGE_VERSION_SIZE + STORAGE_CRC_SIZE)
#define STORAGE_ADDR_BOOL       (STORAGE_ADDR_UINT + STORAGE_UINT_SIZE + STORAGE_CRC_SIZE)

extern uint32_t config_item_uint[CONFIG_UINT_COUNT];
extern uint8_t config_item_bool[CONFIG_BOOL_COUNT];

THD_WORKING_AREA(storagei_thread_area, 64);
static thread_t *storagei_thread;


/**
 * Read data from memory and check crc (last byte after size)
 *
 * @param [out] dest        Destination buffer
 * @param [in] addr         Address of the data in memory
 * @param [in] size         Length of the memory area
 *
 * @return  True if suceeded (crc matches)
 */
static bool Storagei_Read(uint8_t *dest, uint32_t addr, uint32_t size)
{
    uint8_t crc;
    ASSERT_NOT(dest == NULL);

    Eepromd_Read(addr, dest, size);
    Eepromd_Read(addr + size, &crc, 1);
    if (crc != CRC8(dest, size)) {
        return false;
    }
    return true;
}

/**
 * Write data to memory
 *
 * @param [in] src          Source buffer
 * @param [in] addr         Address of the data in memory
 * @param [in] size         Length of the memory area
 *
 * @return  True if suceeded
 */
static bool Storagei_Write(const uint8_t *src, uint32_t addr, uint32_t size)
{
    uint8_t crc;

    ASSERT_NOT(src == NULL);
    Eepromd_Write(addr, src, size);
    crc = CRC8(src, size);
    return Eepromd_Write(addr + size, &crc, 1);
}

static bool Storagei_WriteVersion(const storage_version_t *version)
{
    ASSERT_NOT(version == NULL);

    return Storagei_Write((const uint8_t *)version, STORAGE_ADDR_VERSION,
            STORAGE_VERSION_SIZE);
}

static bool Storagei_ReadVersion(storage_version_t *version)
{
    ASSERT_NOT(version == NULL);

    return Storagei_Read((uint8_t *)version, STORAGE_ADDR_VERSION,
            STORAGE_VERSION_SIZE);
}

static bool Storagei_ReadUint(void)
{
    return Storagei_Read((uint8_t *)config_item_uint, STORAGE_ADDR_UINT,
            STORAGE_UINT_SIZE);
}

static bool Storagei_WriteUint(void)
{
    return Storagei_Write((const uint8_t *)config_item_uint, STORAGE_ADDR_UINT,
            STORAGE_UINT_SIZE);
}

static bool Storagei_ReadBool(void)
{
    return Storagei_Read((uint8_t *)config_item_bool, STORAGE_ADDR_BOOL,
            STORAGE_BOOL_SIZE);
}

static bool Storagei_WriteBool(void)
{
    return Storagei_Write((const uint8_t *)config_item_bool, STORAGE_ADDR_BOOL,
            STORAGE_BOOL_SIZE);
}

/**
 * Processing update events
 */
static THD_FUNCTION(Storagei_Thread, arg)
{
    (void) arg;
    eventmask_t events1, events2;

    while (true) {
        events2 = 0;
        do {
            events1 = chEvtWaitAnyTimeout(ALL_EVENTS,
                    TIME_S2I(STORAGE_SAVE_DELAY_S));
            events2 |= events1;
        } while (events1 != 0 || events2 == 0);

        if (events2 & STORAGE_UPDATE_BOOL) {
            Storagei_WriteBool();
        }
        if (events2 & STORAGE_UPDATE_UINT) {
            Storagei_WriteUint();
        }
    }
}

void Storage_Reset(void)
{
    storage_version_t version;
    version.major = VER_MAJOR;
    version.minor = VER_MINOR;

    Storagei_WriteVersion(&version);
    /* will also call update for all items */
    Config_Reset();
}

bool Storage_LoadAll(void)
{
    bool ret = true;

    storage_version_t version;
    if (Storagei_ReadVersion(&version) != true) {
        Log_Warn(LOG_SOURCE_SYSTEM, "Eeprom version crc failed, resetting config");
        Storage_Reset();
        return false;
    }
    if (version.major != VER_MAJOR) {
        Log_Warn(LOG_SOURCE_SYSTEM, "Eeprom version too old, resetting config");
        Storage_Reset();
        return false;

    }

    if (Storagei_ReadUint() != true) {
        Log_Warn(LOG_SOURCE_SYSTEM, "Eeprom uint crc failed, resetting uints");
        Config_ResetUint();
        ret = false;
    }

    if (Storagei_ReadBool() != true) {
        Log_Warn(LOG_SOURCE_SYSTEM, "Eeprom bool crc failed, resetting bools");
        Config_ResetBool();
        ret = false;
    }

    return ret;
}

void Storage_Update(storage_update_t update)
{
    chEvtSignal(storagei_thread, (eventmask_t) update);
}

void Storage_Init(void)
{
    storagei_thread = chThdCreateStatic(storagei_thread_area,
            sizeof(storagei_thread_area), NORMALPRIO-1, Storagei_Thread, NULL);
}

/** @} */
