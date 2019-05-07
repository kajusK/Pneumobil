#!/usr/bin/env python3
##
# @brief Read data from uart and print log messages
#

from pprint import pprint
import serial
import crcmod

COMM_CMD_LOG_MESSAGE = 0x07
log_sources = [
        "SYSTEM",
        "DRIVER",
        "MODULE",
        "COMM",
        "APP"]

log_severity = [
        "None",
        "Error",
        "Warning",
        "Info",
        "Debug"
        ]


class CommUart:
    part = 0
    len = 0
    received = 0
    msg = []

    def __init__(self, uart, baudrate):
        self.serial = serial.Serial(uart, baudrate)

    def crcVerify(self, crc, buf):
        crcfunc = crcmod.predefined.mkCrcFun("crc-8")
        if crc != crcfunc(bytes(buf)):
            print("Warn: Incorrect crc")
            return False
        return True

    def msgLog(self, msg):
        if len(msg) < 3:
            print("Warn: Log message too short")
            return

        log = { 'severity': log_severity[msg[1]],
                'source' : log_sources[msg[0]],
                'msg': ''.join(map(chr, msg[2:]))}
        pprint(log)

    def msgProcess(self, msg):
        if msg[0] == COMM_CMD_LOG_MESSAGE:
            self.msgLog(msg[1:])

    def receive(self):
        c = self.serial.read()
        c = int.from_bytes(c, byteorder='little')

        if self.part == 0:
            if c == 0xff:
                self.part = 1
                self.len = 0
                self.msg = []
        elif self.part == 1:
            self.len = c
            self.part += 1
        elif self.part == 2:
            self.msg.append(c)
            if len(self.msg) == self.len:
                self.part += 1
        elif self.part == 3:
            if self.crcVerify(c, [0xff, self.len]+self.msg):
                self.msgProcess(self.msg)
            self.part = 0
        else:
            self.part = 0
            print("Error: Got to incorrect receiver state")

c = CommUart('/dev/ttyUSB0', 115200)

while True:
    c.receive()
