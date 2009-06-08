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
    import wx.glcanvas
except ImportError:
    print "Failed to import OpenGL Canvas.  Please make sure the OpenGL canvas package for "
    print "python is installed.  On Ubuntu this can be done from the package manager by searching "
    print "for 'python canvas' and installing python-gnomecanvas or equivalent."
    exit(1)

try:
    from OpenGL.GL import *
except ImportError:
    print "Failed to import OpenGL.  Please make sure the OpenGL package for python is installed."
    print "On Ubuntu this can be done from the package manager by searching for 'python opengl'"
    print "and installing python-opengl or equivalent."
    exit(1)

from array import array

class Channel() :
    def __init__(self,
                 color      = [0.0, 0.0, 0.0],
                 max_length = 1024) :
        self.color      = color
        self.max_length = max_length
        self.data       = array('f', [0.0 for i in range(0, max_length)])
        self.length     = 0
        self.current    = 0

    def add_data(self, value) :
        self.data[self.current] = value

        self.current = (self.current + 1) % self.max_length

        if (self.length < self.max_length) :
            self.length = self.length + 1

    def value(self, index) :
        extra = self.length - self.current

        if (index < extra) :
            return self.data[index + self.current] / 1023.0

        if (index - extra < self.current) :
            return self.data[index - extra] / 1023.0

        return 0.0

    def clear(self) :
        self.data   = array('f', [0.0 for i in range(0, self.max_length)])
        self.length = 0

    def render(self) :
        glColor3f(*self.color)

        glBegin(GL_LINE_STRIP)

        for i in range(0, self.max_length) :
            glVertex2f(float(i) / (self.max_length - 1), self.value(i))

        glEnd()

class Chart(wx.glcanvas.GLCanvas) :
    def __init__(self, parent) :
        wx.glcanvas.GLCanvas.__init__(self, parent, attribList = [wx.glcanvas.WX_GL_DOUBLEBUFFER])
        wx.EVT_PAINT(self, self.OnPaint)

        self.channels = []

    def add_channel(self, color) :
        self.channels.append(Channel(color = color))

    def add_data(self, index, value) :
        if (index < len(self.channels)) :
            self.channels[index].add_data(value)

    def clear(self) :
        for channel in self.channels :
            channel.clear()

        # FIX FIX FIX: What's the right way to do this?
        self.OnPaint(None)

    def OnPaint(self, event) :
        dc = wx.PaintDC(self)
        self.SetCurrent()
        size = self.GetClientSize()
        glViewport(0,0, size.width, size.height)

        glClearColor(0.9, 0.9, 0.9, 0.0)
        glClear(GL_COLOR_BUFFER_BIT)

        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 1.0)

        for channel in self.channels :
            channel.render()

        glFlush()
        self.SwapBuffers()
