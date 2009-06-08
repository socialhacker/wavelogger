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
depth				:= ../..
products			:= file_system_8 file_system_644 commands_8 commands_644

file_system_8[type]		:= library
file_system_8[c_files]		:= file_system.c
file_system_8[c_flags]		:= -mmcu=atmega8
file_system_8[libs]		:= types:types_8
file_system_8[libs]		+= error:error_8
file_system_8[libs]		+= os:os_8
file_system_8[libs]		+= buffer:buffer_8
file_system_8[libs]		+= sdcard:sdcard_8
file_system_8[libs]		+= stdio:stdio_8

file_system_644[type]		:= library
file_system_644[c_files]	:= file_system.c
file_system_644[c_flags]	:= -mmcu=atmega644p
file_system_644[libs]		:= types:types_644
file_system_644[libs]		+= error:error_644
file_system_644[libs]		+= os:os_644
file_system_644[libs]		+= buffer:buffer_644
file_system_644[libs]		+= sdcard:sdcard_644
file_system_644[libs]		+= stdio:stdio_644

commands_8[type]		:= library
commands_8[c_files]		:= commands.c
commands_8[c_flags]		:= -mmcu=atmega8
commands_8[libs]		:= file_system:file_system_8
commands_8[libs]		+= stdio:stdio_8
commands_8[libs]		+= buffer:buffer_8

commands_644[type]		:= library
commands_644[c_files]		:= commands.c
commands_644[c_flags]		:= -mmcu=atmega644p
commands_644[libs]		:= file_system:file_system_644
commands_644[libs]		+= stdio:stdio_644
commands_644[libs]		+= buffer:buffer_644

include $(depth)/build/Makefile.avr
