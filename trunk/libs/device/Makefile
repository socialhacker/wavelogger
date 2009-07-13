# -*- mode: makefile; -*-
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
depth			:= ../..
products		:= device_8 device_644 device_t85

device_8[type]		:= library
device_8[c_files]	:= device.c
device_8[c_flags]	:= -mmcu=atmega8
device_8[libs]		:= types:types_8
device_8[libs]		+= error:error_8

device_644[type]	:= library
device_644[c_files]	:= device.c
device_644[c_flags]	:= -mmcu=atmega644p
device_644[libs]	:= types:types_644
device_644[libs]	:= error:error_644

device_t85[type]	:= library
device_t85[c_files]	:= device.c
device_t85[c_flags]	:= -mmcu=attiny85
device_t85[libs]	:= types:types_t85
device_t85[libs]	:= error:error_t85

include $(depth)/build/Makefile.avr
