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
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "libs/os/os.h"
#include "libs/dac/dac.h"
#include "libs/adc/adc.h"
#include "libs/rtc/rtc.h"
#include "libs/analog_disc/analog_disc.h"

SetupError();

static Analog	_analog;

/**************************************************************************************************/
static inline uint8 bcd_to_binary(uint8 bcd)
{
    return ((bcd >> 4) * 10 + (bcd & 0x0f));
}
/**************************************************************************************************/
static AnalogMessage *next_message()
{
    AnalogMessage	*current = _analog.head;

    if (current->next)
    {
	_analog.head = current->next;
	current->next = null;
    }
    else
    {
	_analog.head = null;
	_analog.tail = null;
    }

    return _analog.head;
}
/**************************************************************************************************/
static void analog_tick()
{
    AnalogMessage	*current = _analog.head;

    if (current == null)
	return;

    if (_analog.prime)
    {
	_analog.prime = false;
    }
    else
    {
	AnalogData volatile	*data   = (AnalogData volatile *) current->buffer;

	if ((_analog.current & 0x01) == 0x00)
	    for (uint8 i = 0; i < 5; ++i)
		_analog.temp_data[i] = 0x00;

	if ((_analog.message.state & 0x80) == 0x00)
	{
	    /*
	     * The four ten bit numbers are packed into the five byte array as follows:
	     * [2|2|2|2][8][8][8][8]
	     */
	    if ((_analog.current & 0x01) == 0x00)
	    {
		_analog.temp_data[0] = (((_analog.buffer[0] & 0x0c) << 4) |
					((_analog.buffer[2] & 0x0c) << 2));
		_analog.temp_data[1] = (_analog.buffer[0] << 6) | (_analog.buffer[1] >> 2);
		_analog.temp_data[2] = (_analog.buffer[2] << 6) | (_analog.buffer[3] >> 2);
	    }
	    else
	    {
		_analog.temp_data[0] |= (((_analog.buffer[0] & 0x0c) >> 0) |
					 ((_analog.buffer[2] & 0x0c) >> 2));
		_analog.temp_data[3] = (_analog.buffer[0] << 6) | (_analog.buffer[1] >> 2);
		_analog.temp_data[4] = (_analog.buffer[2] << 6) | (_analog.buffer[3] >> 2);
	    }
	}

	if ((_analog.current & 0x01) == 0x01)
	{
	    uint8 volatile	*sample = (uint8 volatile *) &(data->sample[_analog.current >> 1]);

	    for (uint8 i = 0; i < 5; ++i)
		sample[i] = _analog.temp_data[i];
	}

	++_analog.current;

	/*
	 * If there is no more space in the current buffer move to the next buffer in the queue.
	 */
	if (_analog.current >= current->read_count)
	{
	    current->state  = analog_message_success;
	    current         = next_message();
	    _analog.current = 0;

	    if (current == null) return;

	    current->state  = analog_message_reading;
	}
    }

    /*
     * Queue another A/D conversion cycle.
     */
    _analog.buffer[0] = 0xf0;

    if ((_analog.message.state & 0x80) == 0x00)
	twi_queue_message(&(_analog.message));

    AnalogData volatile	*data = (AnalogData volatile *) current->buffer;

    switch (_analog.current)
    {
	case 0:
	{
	    data->type                 = 0x01;
	    data->rtc_years            = 0;
	    data->rtc_ticks            = 0;
	    data->internal_temperature = 0;
	    data->ticks                = 0;
	    data->count                = 0;
	    data->battery              = 0;
	    data->checksum             = 0;

	    /*
	     * Queue up a RTC query.
	     */
	    _analog.aux_buffer[0]           = 0x00;
	    _analog.aux_message.address     = 0xd0;
	    _analog.aux_message.write_count = 1;
	    _analog.aux_message.read_count  = 8;
	    twi_queue_message(&(_analog.aux_message));
	}
	break;

	case 1:
	{
	    data->rtc_years = _analog.aux_buffer[7];
	    data->rtc_ticks = rtc_ticks(_analog.aux_buffer);

	    /*
	     * Start the attiny85 on the power supply board converting battery voltage and
	     * internal temperature.
	     */
	    _analog.aux_buffer[0]           = 0x00;
	    _analog.aux_buffer[1]           = 0x00;
	    _analog.aux_buffer[2]           = 0x00;
	    _analog.aux_buffer[3]           = 0x00;
	    _analog.aux_message.address     = 0x12;
	    _analog.aux_message.write_count = 4;
	    _analog.aux_message.read_count  = 0;
	    twi_queue_message(&(_analog.aux_message));
	}
	break;

	case 2:
	{
	    /*
	     * Queue up a query of the attiny85.
	     */
	    _analog.aux_message.address     = 0x12;
	    _analog.aux_message.write_count = 0;
	    _analog.aux_message.read_count  = 4;
	    twi_queue_message(&(_analog.aux_message));
	}
	break;

	case 3:
	{
	    uint16	*short_buffer = (uint16 *)(_analog.aux_buffer);

	    data->internal_temperature = short_buffer[0] >> 2;
	    data->battery              = short_buffer[1] >> 2;
	}
	break;
    }
}
/**************************************************************************************************/
Error analog_init()
{
    _analog.message.buffer          = _analog.buffer;
    _analog.message.address         = 0x42;
    _analog.message.write_count     = 1;
    _analog.message.read_count      = 8;

    _analog.aux_message.buffer      = _analog.aux_buffer;
    _analog.aux_message.address     = 0xd0;
    _analog.aux_message.write_count = 1;
    _analog.aux_message.read_count  = 8;

    _analog.head                    = null;
    _analog.tail                    = null;
    _analog.current                 = 0;
    _analog.tick.function           = analog_tick;
    _analog.prime                   = true;

    arch_tick_timer_add(&(_analog.tick));

    return success;
}
/*********************************************************************************************************************/
const BootModule boot_module_analog PROGMEM =
{
    analog_init
};
/**************************************************************************************************/
Error analog_offset_search(uint8 channel, uint16 *offset)
{
    uint16	high = 0x1000;
    uint16	low  = 0x0000;

    for (uint8 i = 0; i < 12; ++i)
    {
	uint16	mid = (high + low) / 2;
	uint16	value;

	Check(dac_write(channel, mid));

	for (uint8 j = 0; j < 10; ++j)
	    os_sleep_ms(20);

	Check(adc_read(channel, &value));

	if (value < 512) high = mid;
	else             low  = mid;
    }

    *offset = low;

    return success;
}
/**************************************************************************************************/
Error analog_queue_message(AnalogMessage *message)
{
    CheckB(message->read_count > 0);

    message->next = null;

    /*
     * Add the message to the end of the message queue.  This must be an atomic operation as the
     * message queue will be modified by the interrupt handler as well.
     *
     * FIX FIX FIX: I should be able to do this without turning off interrupts.
     */
    uint8 sreg_backup = SREG;
    cli();
    {
	/*
	 * Insert the message into the message queue.
	 */
	if (_analog.tail != null)
	{
	    message->state     = analog_message_queued;
	    _analog.tail->next = message;
	}
	else
	{
	    message->state = analog_message_reading;
	    _analog.head   = message;
	    _analog.prime  = true;
	}

	_analog.tail = message;
    }
    SREG = sreg_backup;

    return success;
}
/**************************************************************************************************/
static bool analog_sleep_check(void *user_data)
{
    AnalogMessage	*message = (AnalogMessage *) user_data;

    return ((message->state & 0x80) == 0x80);
}
/**************************************************************************************************/
Error analog_wait_message(AnalogMessage *message)
{
    os_sleep_cpu(message, analog_sleep_check);

    CheckB(message->state == analog_message_success);

    return success;
}
/**************************************************************************************************/
