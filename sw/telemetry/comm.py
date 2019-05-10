#!/usr/bin/python3
#
# Communication fields decoding
#

from packing import *

class CommStructure:
    def __str__(self):
        # Make string form internal variables
        return "TODO"

    def decode(self):
        #decode data into internal structure
        raise NotImplementedError

class Telemetry(CommStructure):
    CMD_ID = 0x3a
    MSG_LEN = 27

    def decode(self, buf):
        self.speed_kmh = unpack_uint16(buf[0:2])/100*3.6
        self.speed_avg_kmh = unpack_uint16(buf[2:4])/100*3.6
        self.speed_max_kmh = unpack_uint16(buf[4:6])/100*3.6
        self.distance_m = unpack_uint16(buf[6:8])
        self.race_mode = unpack_uint8(buf[8])
        self.filling_pct = unpack_uint8(buf[9])
        self.deadtime_ms = unpack_uint16(buf[10:12])
        self.press1_kpa = unpack_uint16(buf[12:14])
        self.press2_kpa = unpack_uint16(buf[14:16])
        self.press3_kpa = unpack_uint16(buf[16:18])
        self.time_s = unpack_uint8(buf[18])
        self.time_m = unpack_uint8(buf[19])
        self.bat_mv = unpack_uint16(buf[20:22])
        self.bat_ma = unpack_uint16(buf[22:24])
        self.piston_pct = unpack_uint8(buf[24])

        byte = unpack_uint8(buf[25])
        self.brake = (byte >> 7) & 0x01
        self.throttle = (byte >> 6) & 0x01
        self.valve_f1 = (byte >> 4) & 0x03
        self.valve_f2 = (byte >> 2) & 0x03
        self.valve_b1 = byte & 0x03
        self.valve_b2 = (byte >> 6) & 0x03

class LogMessage(CommStructure):
    CMD_ID = 0x07
    MSG_MIN_LEN = 3

    def decode(self, buf):
        self.module = unpack_uint8(buf[0])
        self.source = unpack_uint8(buf[1])
        self.severity = unpack_uint8(buf[2])
        self.msg = ''.join([chr(x) for x in buf[3:]])
