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
products		:= eeprom_8 eeprom_644 eeprom_t85

eeprom_8[type]		:= library
eeprom_8[c_files]	:= eeprom.c
eeprom_8[c_flags]	:= -mmcu=atmega8
eeprom_8[libs]		:= types:types_8

eeprom_644[type]	:= library
eeprom_644[c_files]	:= eeprom.c
eeprom_644[c_flags]	:= -mmcu=atmega644p
eeprom_644[libs]	:= types:types_644

eeprom_t85[type]	:= library
eeprom_t85[c_files]	:= eeprom.c
eeprom_t85[c_flags]	:= -mmcu=attiny85
eeprom_t85[libs]	:= types:types_t85

include $(depth)/build/Makefile.avr
