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
products		:= rtc_8 rtc_644 commands_8 commands_644

rtc_8[type]		:= library
rtc_8[c_files]		:= rtc.c
rtc_8[c_flags]		:= -mmcu=atmega8
rtc_8[libs]		:= types:types_8 twi:twi_8 os:os_8

rtc_644[type]		:= library
rtc_644[c_files]	:= rtc.c
rtc_644[c_flags]	:= -mmcu=atmega644p
rtc_644[libs]		:= types:types_644 twi:twi_644 os:os_644

commands_8[type]	:= library
commands_8[c_files]	:= commands.c
commands_8[c_flags]	:= -mmcu=atmega8
commands_8[libs]	:= rtc:rtc_8 stdio:stdio_8

commands_644[type]	:= library
commands_644[c_files]	:= commands.c
commands_644[c_flags]	:= -mmcu=atmega644p
commands_644[libs]	:= rtc:rtc_644 stdio:stdio_644

include $(depth)/build/Makefile.avr
