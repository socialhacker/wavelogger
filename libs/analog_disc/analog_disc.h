/*
 * Copyright 2009 Anton Staaf
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __analog_disc_h__
#define __analog_disc_h__

#include "libs/os/os.h"
#include "libs/os/arch.h"
#include "libs/types/types.h"
#include "libs/error/error.h"
#include "libs/twi/master.h"
#include "libs/device/device.h"

typedef struct 
{
    uint8	data[5];
} AnalogSample;

typedef struct
{
    uint8		type;
    uint8		rtc_years;
    uint32		rtc_ticks;
    uint8		internal_temperature;
    uint8		ticks;
    uint8		count;
    uint8		battery;
    uint16		checksum;
    AnalogSample	sample[100];
} AnalogData;

typedef struct AnalogMessageStruct
{
    Message			message;
    uint8 volatile * volatile	buffer;
    uint8 volatile		read_count;
} AnalogMessage;

typedef struct
{
    Device			device;

    uint8			adc_buffer[8];
    TWIMessage			adc_message;

    uint8			aux_buffer[8];
    TWIMessage			aux_message;

    Message			*current;
    uint8			index;
    ArchTickHandler		tick;

    uint8			temp_data[5];
} Analog;

/*
 *
 */
extern const BootModule boot_module_analog PROGMEM;

Error analog_init();
Error analog_offset_search(uint8 channel, uint16 *offset);
Error analog_queue_message(AnalogMessage *message);
Error analog_wait_message(AnalogMessage *message);

#endif //__analog_disc_h__
