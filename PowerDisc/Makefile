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
depth		:= ..
products	:= power

power[type]	:= program
power[c_files]	:= main.c slave.c device.c
power[c_flags]	:= -mmcu=attiny85
power[l_flags]	:= -mmcu=attiny85
power[libs]	:= types:types_t85 error:error_t85 os:os_t85

include $(depth)/build/Makefile.avr

PGM_MCU		:= t85
TARGET		:= release/power

include $(depth)/Makefile.avrdude
