#!/usr/bin/env python
# -*- mode: python; -*-
#
# Copyright 2009 Anton Staaf
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
try:
    import wx
except ImportError:
    print "Failed to import WxPython.  Please make sure the WxPython package is installed."
    print "On Ubuntu this can be done from the package manager by searching for 'wx python'"
    print "and installing python-wxgtk2.8 or equivalent."
    exit(1)

try:
    import serial
except ImportError:
    print "Failed to import PySerial.  Please make sure the PySerial package is installed."
    print "On Ubuntu this can be done from the package manager by searching for 'serial python'"
    print "and installing python-serial or equivalent."
    exit(1)

import os
import time
import glob
from threading import Thread
from Queue import Queue

#
# Get a list of serial ports that we can open.
#
def port_list() :
    list = []

    for file in (['/dev/stack_adaptor'] +
                 glob.glob('/dev/ttyUSB*') +
                 glob.glob('/dev/ttyS*') +
                 glob.glob('/dev/tty.*')) :
        try :
            port = serial.Serial(file)
            port.close()
            list.append(file)
        except serial.SerialException :
            pass

    return list

#
# Define SerialText event.
#
EVT_SERIAL_TEXT_ID = wx.NewId()

def EVT_SERIAL_TEXT(win, func) :
    win.Connect(-1, -1, EVT_SERIAL_TEXT_ID, func)

class SerialTextEvent(wx.PyEvent) :
    def __init__(self) :
        wx.PyEvent.__init__(self)
        self.SetEventType(EVT_SERIAL_TEXT_ID)

#
# The SerialThread generates SerialText events.
#
class SerialThread(Thread) :
    def __init__ (self, port) :
        Thread.__init__(self)
        self.port   = port
        self.serial = None
        self.queue  = Queue(0)
        self.target = None
        self._quit  = False

        self.open_port()

    def open_port(self) :
        self.serial = serial.Serial(self.port, 57600, timeout = 0.1)

    def set_target(self, target) :
        self.target = target

    def write(self, string) :
        for byte in string :
            try :
                self.serial.write(byte)
            except :
                self.open_port()
                self.serial.write(byte)
            time.sleep(0.005)

    def get_queue(self) :
        return self.queue

    def quit(self) :
        self._quit = True
        self.join()

    def run(self) :
        while (self._quit == False) :
            try :
                data = self.serial.read(32)
                data = data.replace("\r", "")
                data = data.replace("\0", "")

                if (len(data) > 0) :
                    self.queue.put(data)

                    if (self.target) :
                        wx.PostEvent(self.target, SerialTextEvent())
            except :
                print "Serial Exception..."
                self.open_port()
