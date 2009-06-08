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
products		:= led_8 led_644 commands_8 commands_644

led_8[type]		:= library
led_8[c_files]		:= led_8.c
led_8[c_flags]		:= -mmcu=atmega8
led_8[libs]		:= types:types_8 os:os_8

led_644[type]		:= library
led_644[c_files]	:= led.c
led_644[c_flags]	:= -mmcu=atmega644p
led_644[libs]		:= types:types_644 os:os_644

commands_8[type]	:= library
commands_8[c_files]	:= commands.c
commands_8[c_flags]	:= -mmcu=atmega8
commands_8[libs]	:= led:led_8

commands_644[type]	:= library
commands_644[c_files]	:= commands.c
commands_644[c_flags]	:= -mmcu=atmega644p
commands_644[libs]	:= led:led_644

include $(depth)/build/Makefile.avr
