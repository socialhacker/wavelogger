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
products		:= adc_8 adc_644

adc_8[type]		:= library
adc_8[c_files]		:= adc.c
adc_8[c_flags]		:= -mmcu=atmega8
adc_8[libs]		:= types:types_8 twi:twi_8

adc_644[type]		:= library
adc_644[c_files]	:= adc.c
adc_644[c_flags]	:= -mmcu=atmega644p
adc_644[libs]		:= types:types_644 twi:twi_644

include $(depth)/build/Makefile.avr
