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
static void analog_tick()
{
    /*
     * If there is no current buffer then check to see if there is a buffer waiting in the queue.
     * If there is then we set it up for reading and prime the ADC converter so that the next time
     * the analog_tick function is called there will be a message waiting with current analog
     * data.
     */
    if (_analog.current == null)
    {
	_analog.current = device_get_next_message(&_analog.device);
	_analog.index   = 0;

	if (_analog.current == null)
	    return;

	_analog.current->state = message_state_reading;

	/*
	 * Queue up the first A/D conversion cycle.
	 */
	_analog.adc_buffer[0] = 0xf0;

	if ((_analog.adc_message.state & 0x80) == 0x00)
	    twi_queue_message(&(_analog.adc_message));

	return;
    }

    /*
     * If there is no more space in the current buffer move to the next buffer in the queue.
     */
    if (_analog.index >= ((AnalogMessage *)_analog.current)->read_count)
    {
	_analog.current->state = message_state_success;
	_analog.current        = device_get_next_message(&_analog.device);
	_analog.index          = 0;

	if (_analog.current == null)
	    return;

	_analog.current->state = message_state_reading;
    }

    AnalogMessage	*message = (AnalogMessage *) _analog.current;
    AnalogData volatile	*data    = (AnalogData volatile *) message->buffer;

    if ((_analog.index & 0x01) == 0x00)
	for (uint8 i = 0; i < 5; ++i)
	    _analog.temp_data[i] = 0x00;

    if ((_analog.adc_message.state & 0x80) == 0x00)
    {
	/*
	 * The four ten bit numbers are packed into the five byte array as follows:
	 * [2|2|2|2][8][8][8][8]
	 */
	if ((_analog.index & 0x01) == 0x00)
	{
	    _analog.temp_data[0] = (((_analog.adc_buffer[0] & 0x0c) << 4) |
				    ((_analog.adc_buffer[2] & 0x0c) << 2));
	    _analog.temp_data[1] = (_analog.adc_buffer[0] << 6) | (_analog.adc_buffer[1] >> 2);
	    _analog.temp_data[2] = (_analog.adc_buffer[2] << 6) | (_analog.adc_buffer[3] >> 2);
	}
	else
	{
	    _analog.temp_data[0] |= (((_analog.adc_buffer[0] & 0x0c) >> 0) |
				     ((_analog.adc_buffer[2] & 0x0c) >> 2));
	    _analog.temp_data[3] = (_analog.adc_buffer[0] << 6) | (_analog.adc_buffer[1] >> 2);
	    _analog.temp_data[4] = (_analog.adc_buffer[2] << 6) | (_analog.adc_buffer[3] >> 2);
	}
    }

    if ((_analog.index & 0x01) == 0x01)
    {
	uint8 volatile	*sample = (uint8 volatile *) &(data->sample[_analog.index >> 1]);

	for (uint8 i = 0; i < 5; ++i)
	    sample[i] = _analog.temp_data[i];
    }

    /*
     * Queue another A/D conversion cycle.
     */
    _analog.adc_buffer[0] = 0xf0;

    if ((_analog.adc_message.state & 0x80) == 0x00)
	twi_queue_message(&(_analog.adc_message));

    /*
     * For the first couple samples in a message we query the RTC, battery voltage and internal
     * temperature.
     */
    switch (_analog.index)
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

    ++_analog.index;
}
/**************************************************************************************************/
Error analog_init()
{
    device_initialize(&_analog.device);

    _analog.adc_message.buffer      = _analog.adc_buffer;
    _analog.adc_message.address     = 0x42;
    _analog.adc_message.write_count = 1;
    _analog.adc_message.read_count  = 8;

    _analog.aux_message.buffer      = _analog.aux_buffer;
    _analog.aux_message.address     = 0xd0;
    _analog.aux_message.write_count = 1;
    _analog.aux_message.read_count  = 8;

    _analog.current                 = null;
    _analog.index                   = 0;

    /*
     * Register a timer tick handler.
     */
    _analog.tick.function = analog_tick;

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

    message_queue((Message *) message, &_analog.device);

    return success;
}
/**************************************************************************************************/
Error analog_wait_message(AnalogMessage *message)
{
    return message_wait((Message *) message);
}
/**************************************************************************************************/
