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
products	:= shell cpu

cpu[type]	:= program
cpu[c_files]	+= cpu.c

cpu[libs]	:= shell:shell_644
cpu[libs]	+= os:os_644
cpu[libs]	+= serial:serial_644
cpu[libs]	+= eeprom:eeprom_644
cpu[libs]	+= types:types_644
cpu[libs]	+= led:led_644
cpu[libs]	+= cylinder:cylinder_644
cpu[libs]	+= analog_disc:analog_644
cpu[libs]	+= gain:gain_644
cpu[libs]	+= adc:adc_644
cpu[libs]	+= dac:dac_644
cpu[libs]	+= rtc:rtc_644
cpu[libs]	+= twi:twi_644
cpu[libs]	+= spi:spi_644
cpu[libs]	+= sdcard:sdcard_644
cpu[libs]	+= block:block_644
cpu[libs]	+= partition:partition_644
cpu[libs]	+= buffer:buffer_644
cpu[libs]	+= power:power_644
cpu[libs]	+= error:error_644
cpu[libs]	+= file_system:file_system_644

cpu[c_flags]	:= -mmcu=atmega644p
cpu[l_flags]	:= -mmcu=atmega644p

shell[type]	:= program
shell[c_files]	+= main.c record.c

shell[libs]	:= shell:shell_644
shell[libs]	+= os:os_644
shell[libs]	+= serial:serial_644
shell[libs]	+= eeprom:eeprom_644
shell[libs]	+= types:types_644
shell[libs]	+= led:commands_644
shell[libs]	+= led:led_644
shell[libs]	+= cylinder:commands_644
shell[libs]	+= cylinder:cylinder_644
shell[libs]	+= stdio:stdio_644
shell[libs]	+= analog_disc:commands_644
shell[libs]	+= analog_disc:analog_644
shell[libs]	+= gain:gain_644
shell[libs]	+= adc:adc_644
shell[libs]	+= dac:dac_644
shell[libs]	+= rtc:commands_644
shell[libs]	+= rtc:rtc_644
shell[libs]	+= twi:commands_644
shell[libs]	+= twi:twi_644
shell[libs]	+= buffer:commands_644
shell[libs]	+= buffer:buffer_644
shell[libs]	+= sdcard:commands_644
shell[libs]	+= file_system:commands_644
shell[libs]	+= file_system:file_system_644
shell[libs]	+= block:block_644
shell[libs]	+= partition:partition_644
shell[libs]	+= spi:spi_644
shell[libs]	+= power:commands_644
shell[libs]	+= power:power_644
shell[libs]	+= error:error_644

shell[c_flags]	:= -mmcu=atmega644p
shell[c_flags]	+= -DERROR_STACK_ENABLE
shell[l_flags]	:= -mmcu=atmega644p

include $(depth)/build/Makefile.avr

PGM_MCU		:= m644p
TARGET		:= $(intermediate)/$(firstword $(selected_products))

include $(depth)/Makefile.avrdude
