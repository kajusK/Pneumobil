#!/usr/bin/python3
import sys
import logging
from datetime import datetime
from PyQt5 import QtCore, QtGui, QtWidgets, uic
from PyQt5.QtWidgets import QTableWidgetItem

from uart import *
from comm import *

LOG_SEVERITY = ['NONE', 'ERROR', 'WARNING', 'INFO', 'DEBUG']
LOG_SOURCE = ['SYSTEM', 'DRIVER', 'MODULE', 'COMM', 'APP']
LOG_MODULE = ['ECU', 'HMI', 'PSU', 'SDU', 'RF2USB']
RACE_MODE = ['Arcade', 'Acceleration', 'Long Distance', 'DEBUG']
VALVE_STATE = ['Closed', 'In', 'Out']
# amount of lines to be kept in log window
LOG_KEEP = 1000

# Invocation of methods in main thread
class InvokeEvent(QtCore.QEvent):
    EVENT_TYPE = QtCore.QEvent.Type(QtCore.QEvent.registerEventType())

    def __init__(self, fn, *args, **kwargs):
        QtCore.QEvent.__init__(self, InvokeEvent.EVENT_TYPE)
        self.fn = fn
        self.args = args
        self.kwargs = kwargs

class Invoker(QtCore.QObject):
    def event(self, event):
        event.fn(*event.args, **event.kwargs)

        return True

_invoker = Invoker()

def invoke_in_main_thread(fn, *args, **kwargs):
    QtCore.QCoreApplication.postEvent(_invoker, InvokeEvent(fn, *args, **kwargs))


class MainWindow(QtWidgets.QMainWindow):
    def __init__(self):
        super(MainWindow,self).__init__()
        uic.loadUi('frontend.ui', self)
        self.con = Uart()
        self.initHandlers();
        self.log = logging.getLogger()

        # setup logging module
        handler = logging.StreamHandler()
        formatter = logging.Formatter(
                '%(asctime)s %(levelname)s %(message)s')
        handler.setFormatter(formatter)
        self.log.addHandler(handler)
        self.log.setLevel(logging.DEBUG)

    def initHandlers(self):
        # connect UI actions to internal functions
        self.btConnect.clicked.connect(self.doUartConnect)
        self.btDisconnect.clicked.connect(self.doUartDisconnect)

    def doUartConnect(self):
        if self.con.connect(self.leDevice.text(), self.leBaudrate.text()) == False:
            return

        self.leDevice.setEnabled(False)
        self.leBaudrate.setEnabled(False)
        self.btConnect.setEnabled(False)
        self.btDisconnect.setEnabled(True)

        #connect all required processing signals
        self.con.subscribe(LogMessage.CMD_ID, self.addLog)
        self.con.subscribe(Telemetry.CMD_ID, self.updateTelemetry)

    def doUartDisconnect(self):
        self.con.disconnect()
        self.leDevice.setEnabled(True)
        self.leBaudrate.setEnabled(True)
        self.btConnect.setEnabled(True)
        self.btDisconnect.setEnabled(False)

    def addLog(self, msg):
        if not isinstance(threading.current_thread(), threading._MainThread):
            invoke_in_main_thread(self.updateTelemetry, msg)

        pos = self.tbSystemLog.rowCount()
        if pos > LOG_KEEP:
            self.tbSystemLog.removeRow(0)
            pos -= 1
        self.tbSystemLog.insertRow(pos)

        dt = datetime.now()
        self.tbSystemLog.setItem(pos, 0, QTableWidgetItem(('%02d:%02d:%2d.%3d') %
                (dt.hour, dt.minute, dt.second, dt.microsecond)))

        self.tbSystemLog.setItem(pos, 1, QTableWidgetItem(LOG_MODULE[msg.module]))
        self.tbSystemLog.setItem(pos, 2, QTableWidgetItem(LOG_SEVERITY[msg.severity]))
        self.tbSystemLog.setItem(pos, 3, QTableWidgetItem(LOG_SOURCE[msg.source]))
        self.tbSystemLog.setItem(pos, 4, QTableWidgetItem(msg.msg))

    def updateTelemetry(self, msg):
        if not isinstance(threading.current_thread(), threading._MainThread):
            invoke_in_main_thread(self.updateTelemetry, msg)

        self.lbSpeed.setText("%3.1f" % (msg.speed_kmh))
        self.lbDistance.setText("%5d" % (msg.distance_m))
        self.lbRaceTime.setText("%02d:%02d" % (msg.time_m, msg.time_s))
        self.lbSpeedAvg.setText("%3.1f" % (msg.speed_avg_kmh))
        self.lbSpeedTop.setText("%3.1f" % (msg.speed_max_kmh))
        self.lbMode.setText(RACE_MODE[msg.race_mode])
        self.lbCurFilling.setText("%3d %%" % (msg.filling_pct))
        self.lbCurDeadtime.setText("%4d ms" % (msg.deadtime_ms))
        self.lbBatVoltage.setText("%5d" % (msg.bat_mv))
        self.lbBatCurrent.setText("%4d" % (msg.bat_ma))

        self.pbPressure1.setValue(msg.press1_kpa)
        self.pbPressure2.setValue(msg.press2_kpa)
        self.pbPressure3.setValue(msg.press3_kpa)

        self.lbThrottle.setEnabled(msg.throttle)
        self.lbBrake.setEnabled(msg.brake)

        self.lbValveBack1.setText(VALVE_STATE[msg.valve_b1])
        self.lbValveBack2.setText(VALVE_STATE[msg.valve_b2])
        self.lbValveFront1.setText(VALVE_STATE[msg.valve_f1])
        self.lbValveFront2.setText(VALVE_STATE[msg.valve_f2])
        self.pbPistonPosition.setValue(msg.piston_pct)

if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    window = MainWindow()
    window.lbSpeed.setProperty("value", 172.2)
    window.show()

    sys.exit(app.exec_())
