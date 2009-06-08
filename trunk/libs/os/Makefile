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
depth		:= ../..
products	:= os_8 os_644 os_t85

os_8[type]	:= library
os_8[c_files]	:= os.c
os_8[c_files]	+= arch/atmega8/reset.c
os_8[c_files]	+= arch/atmega8/sleep.c
os_8[c_files]	+= arch/atmega8/tick_timer.c
os_8[c_flags]	:= -mmcu=atmega8 -DF_CPU="8000000UL"
os_8[libs]	:= types:types_8
os_8[libs]	+= error:error_8
os_8[libs]	+= eeprom:eeprom_8

os_644[type]	:= library
os_644[c_files]	:= os.c
os_644[c_files]	+= arch/atmega644p/reset.c
os_644[c_files]	+= arch/atmega644p/sleep.c
os_644[c_files]	+= arch/atmega644p/tick_timer.c
os_644[c_flags]	:= -mmcu=atmega644p -DF_CPU="8000000UL"
os_644[libs]	:= types:types_644
os_644[libs]	:= error:error_644
os_644[libs]	:= eeprom:eeprom_644

os_t85[type]	:= library
os_t85[c_files]	:= os.c
os_t85[c_files]	+= arch/attiny85/reset.c
os_t85[c_files]	+= arch/attiny85/sleep.c
os_t85[c_files]	+= arch/attiny85/tick_timer.c
os_t85[c_flags]	:= -mmcu=attiny85 -DF_CPU="8000000UL"
os_t85[libs]	:= types:types_t85
os_t85[libs]	:= error:error_t85
os_t85[libs]	:= eeprom:eeprom_t85

include $(depth)/build/Makefile.avr
