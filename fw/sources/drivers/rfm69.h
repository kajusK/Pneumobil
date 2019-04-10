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
 * @file    rfm69.h
 * @brief   HopeRF RFM69 wireless transceiver driver
 *
 * @addtogroup drivers
 * @{
 */

#ifndef __DRIVERS_RFM69_H
#define __DRIVERS_RFM69_H

#include <types.h>

/**
 * TX output power control
 *
 * The RFM69W can be in range -18 to 13 dBm (only PA1 is used).
 * The RFM69HW version can be in range -2 to 20 dBm (+20dBm mode always on)
 *
 * @param dBm Output level in dBm
 */
extern void rfm69_SetPowerLevel(int8_t dBm);

/**
 * Set 16 bytes encryption key or NULL for no encryption
 *
 * Keys must be same on both communication sides
 *
 * @param [in] key      16 bytes long encryption key
 */
extern void rfm69_SetEncrypt(const uint8_t* key);

/**
 * Get last rssi (signal strength) value
 *
 * After switching to rx mode or when no rssi sample is available, -127 dBm is
 * returned
 */
extern int16_t rfm69i_GetRSSIdBm(void);

/**
 * Get currently tuned frequency in Hz
 */
extern uint32_t rfm69_GetFrequencyHz(void);

/**
 * Tune device to given frequency in Hz
 */
extern void rfm69_SetFrequencyHz(uint32_t freq_Hz);

/**
 * Set the bitrate in bits per second.
 */
extern void rfm69_SetBitrate(uint32_t bitrate);

/**
 * Control of promiscuous mode
 *
 * @param enable    if true, capture all framew, else only destined to this node
 *                  or broadcast
 */
extern void rfm69_EnablePromiscuous(bool enable);

/**
 * Send data over air
 *
 * Waits for channel to become clear. The RFM module is set in variable
 * length mode - fifo starts with payload length, followed by destination node
 * addr and data.
 *
 * @param dest      Destination address
 * @param [in] buffer   Data buffer
 * @param len       Lenght of the buffer (up to 61 bytes)
 */
extern void rfm69_Send(uint8_t dest, const void* buffer, uint8_t len);

/**
 * Try to receive a packet
 *
 * Switches to RX mode if not already in RX and checks if packet was
 * received, if not, returns 0, but module remains in RX.
 * The function is expected to be called periodically to check for received
 * data
 *
 * @param buffer    Pointer to memory to store data to
 * @param max_len   Maximum amount of bytes that can be read into buffer
 *
 * @return  Amount of bytes received or 0 if nothing was received
 */
extern uint8_t rfm69_Receive(char *buffer, uint8_t max_len);

/*
 * Initialize rfm module
 *
 * @param nodeID        Id of this node (for filtered communication)
 * @param networkID     Network id this node belongs to (sync value)
 * @param isHxx         Set to true for high power variants (rfm69hw,...)
 * @param csma_ca       Set to true for checking channel activity before tx
 *
 * @return True of ok, false if module not responding
 */
extern bool rfm69_Init(uint8_t nodeID, uint8_t networkID, bool isHxx, bool csma_ca);

#endif

/** @} */
