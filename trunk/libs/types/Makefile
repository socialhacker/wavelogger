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
products		:= types_8 types_644 types_t85

types_8[type]		:= library
types_8[c_files]	:= types.c
types_8[c_flags]	:= -mmcu=atmega8

types_644[type]		:= library
types_644[c_files]	:= types.c
types_644[c_flags]	:= -mmcu=atmega644p

types_t85[type]		:= library
types_t85[c_files]	:= types.c
types_t85[c_flags]	:= -mmcu=attiny85

include $(depth)/build/Makefile.avr
