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
products		:= analog_8 analog_644 commands_8 commands_644

analog_8[type]		:= library
analog_8[c_files]	:= analog_disc.c
analog_8[c_flags]	:= -mmcu=atmega8
analog_8[libs]		:= types:types_8 error:error_8 os:os_8 twi:twi_8 gain:gain_8

analog_644[type]	:= library
analog_644[c_files]	:= analog_disc.c
analog_644[c_flags]	:= -mmcu=atmega644p
analog_644[libs]	:= types:types_644 error:error_644 os:os_644 twi:twi_644 gain:gain_644

commands_8[type]	:= library
commands_8[c_files]	:= commands.c
commands_8[c_flags]	:= -mmcu=atmega8
commands_8[libs]	:= dac:dac_8
commands_8[libs]	+= adc:adc_8
commands_8[libs]	+= stdio:stdio_8
commands_8[libs]	+= serial:serial_8
commands_8[libs]	+= analog_disc:analog_8

commands_644[type]	:= library
commands_644[c_files]	:= commands.c
commands_644[c_flags]	:= -mmcu=atmega644p
commands_644[libs]	:= dac:dac_644
commands_644[libs]	+= adc:adc_644
commands_644[libs]	+= stdio:stdio_644
commands_644[libs]	+= serial:serial_644
commands_644[libs]	+= analog_disc:analog_644

include $(depth)/build/Makefile.avr
