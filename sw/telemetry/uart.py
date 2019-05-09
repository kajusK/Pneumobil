#!/usr/bin/python3
#
# Uart handling
#

import logging
import inspect
import sys
import serial
import threading
import crcmod
from comm import *
from pydispatch import dispatcher

UART_SOF = 0xff

BAUDRATE = 115200
PORT = "/dev/ttyUSB0"
UART_TIMEOUT_S = 0.5

# Receiving state machine
UART_STATE_START = 0
UART_STATE_LEN = 1
UART_STATE_DATA = 2
UART_STATE_CRC = 3

class Uart:
    def __init__(self):
        self.port = None
        self.log = logging.getLogger()#self.__class__.__name)
        self.thread = None

    def connect(self, port=PORT, baudrate=BAUDRATE):
        if self.port is not None and self.port.isOpen():
            self.log.warning("Serial port {} already open".format(port))
            return

        self.log.info("Opening serial port {} with baudrate {}".format(port, baudrate))
        try:
            self.port = serial.Serial(port, baudrate)
        except serial.serialutil.SerialException:
            self.log.error("Failed to open port %s", port)
            return False
        self.port.timeout = UART_TIMEOUT_S

        self.thread_run = True
        self.thread = threading.Thread(target=self._dataThread)
        self.thread.start()

        return True

    def disconnect(self):
        if self.port is None:
            return

        self.thread_run = False
        self.thread.join()

        self.log.info("Closing serial port")
        self.port.close()
        self.port = None

    def subscribe(self, cmd_id, callback):
        dispatcher.connect(callback, signal=cmd_id, sender=self)

    def _processData(self, buf):
        cmd = buf[0] & 0x3f
        self.log.debug('CMD = %02x, resp = %d, error = %d, data = %s',
                cmd, buf[0] & 0x80 != 0, buf[0] & 0x40 != 0,
                ''.join('%02X ' % int(b) for b in buf[1:]))

        msg = None
        for name, obj in inspect.getmembers(sys.modules[CommStructure.__module__]):
            try:
                if obj.CMD_ID == cmd:
                    self.log.debug("Found class %s for decoding cmd %d", name, cmd)
                    msg = obj()
                    break
            except AttributeError:
                pass

        if msg != None:
            msg.decode(buf[1:])
            dispatcher.send(signal = cmd, sender = self, msg = msg)
        else:
            self.log.debug("Class for decoding cmd %d not found", cmd)

    def _crcVerify(self, crc, buf):
        # calculate crc for given buffer and return true if result is same as crc
        crcfunc = crcmod.predefined.mkCrcFun("crc-8")
        if crc != crcfunc(bytes(buf)):
            return False
        return True

    def _dataThread(self):
        # thread for receiving data over uart
        state = UART_STATE_START
        length = 0
        count = 0
        buf = []

        while self.thread_run:
            c = self.port.read(1)
            #timeouted
            if len(c) == 0:
                state = UART_STATE_START
                continue
            c = int.from_bytes(c, byteorder='little')

            if state == UART_STATE_START:
                if c == UART_SOF:
                    state = UART_STATE_LEN
                    length = 0
                    count = 0
                    crc = 0
                    buf = []
            elif state == UART_STATE_LEN:
                state = UART_STATE_DATA
                length = c
            elif state == UART_STATE_DATA:
                buf.append(c)
                count += 1
                if count == length:
                    state = UART_STATE_CRC
            elif state == UART_STATE_CRC:
                state = UART_STATE_START
                if self._crcVerify(c, [UART_SOF, length]+buf):
                    self.log.debug("Received: {}".format(''.join('%02x ' % b for b in buf)))
                    self._processData(buf)
                else:
                    self.log.warn("Received uart crc does not match")
            else:
                state = UART_STATE_START
                self.log.error("Entered incorrect uart state {}".format(state))

if __name__ == '__main__':
    conn = Comm()
    comm.connect()
    comm.disconnect
