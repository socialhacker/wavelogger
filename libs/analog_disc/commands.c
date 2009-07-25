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
#include "libs/dac/dac.h"
#include "libs/adc/adc.h"
#include "libs/gain/gain.h"
#include "libs/rtc/commands.h"
#include "libs/stdio/write.h"
#include "libs/serial/serial.h"
#include "libs/analog_disc/analog_disc.h"
#include "libs/analog_disc/commands.h"

SetupError();

static uint16			_dac_offset[4];
static AnalogData volatile	_data[2];
static AnalogMessage		_message[2] =
{
    {{message_state_new, null}, (uint8 volatile *) &(_data[0]), 200},
    {{message_state_new, null}, (uint8 volatile *) &(_data[1]), 200}
};

/*********************************************************************************************************************/
static Error search_command(uint argc, const char **argv)
{
    for (uint8 i = 0; i < 4; ++i)
    {
	Check(analog_offset_search(i, _dac_offset + i));
	Check(dac_write(i, _dac_offset[i]));
    }

    write(PSTR("Offsets: %d %d %d %d\r\n"),
	  _dac_offset[0],
	  _dac_offset[1],
	  _dac_offset[2],
	  _dac_offset[3]);

    return success;
}
/*********************************************************************************************************************/
static Error sample_command(uint argc, const char **argv)
{
    uint8	index = 0;

    _message[0].read_count = 200;
    _message[1].read_count = 200;

    Check(analog_queue_message(&(_message[0])));
    Check(analog_queue_message(&(_message[1])));

    while (!uart_character_ready())
    {
	Check(analog_wait_message(&(_message[index])));

	for (uint i = 0; i < 100; ++i)
	{
	    AnalogSample	*sample = &(_data[index].sample[i]);
	    uint16		channel[4];

	    channel[0] = ((uint16)(sample->data[0] & 0xc0)) << 2 | sample->data[1];
	    channel[1] = ((uint16)(sample->data[0] & 0x30)) << 4 | sample->data[2];
	    channel[2] = ((uint16)(sample->data[0] & 0x0c)) << 6 | sample->data[3];
	    channel[3] = ((uint16)(sample->data[0] & 0x03)) << 8 | sample->data[4];

	    write(PSTR("<data> %d %d\r\n"), channel[0], channel[1]);
	    write(PSTR("<data> %d %d\r\n"), channel[2], channel[3]);
	}

	Check(analog_queue_message(&(_message[index])));

	index = !index;
    }

    return success;
}
/*********************************************************************************************************************/
static Error average_command(uint argc, const char **argv)
{
    uint8	index = 0;
    uint32	count;

    CheckB(argc == 2);

    /*
     * Ensure that the count is an even number of samples.  And that it is no more than 200
     * samples.
     */
    count = parse_number(argv[1]);
    count = (count + 1) & ~1;
    count = (count < 200) ? count : 200;

    _message[0].read_count = count;
    _message[1].read_count = count;

    Check(analog_queue_message(&(_message[0])));
    Check(analog_queue_message(&(_message[1])));

    while (!uart_character_ready())
    {
	uint32	channel[2] = {0, 0};

	Check(analog_wait_message(&(_message[index])));

	for (uint i = 0; i < (count >> 1); ++i)
	{
	    AnalogSample	*sample = &(_data[index].sample[i]);

	    channel[0] += ((uint16)(sample->data[0] & 0xc0)) << 2 | sample->data[1];
	    channel[1] += ((uint16)(sample->data[0] & 0x30)) << 4 | sample->data[2];
	    channel[0] += ((uint16)(sample->data[0] & 0x0c)) << 6 | sample->data[3];
	    channel[1] += ((uint16)(sample->data[0] & 0x03)) << 8 | sample->data[4];
	}

	channel[0] /= count;
	channel[1] /= count;

	write(PSTR("<average of %ld samples> %ld %ld\r\n"), count, channel[0], channel[1]);

	Check(analog_queue_message(&(_message[index])));

	index = !index;
    }

    return success;
}
/*********************************************************************************************************************/
static Error dac_command(uint argc, const char **argv)
{
    CheckB(argc >= 2 && argc <= 3);

    if (argc == 3)
    {
	uint8	dac   = parse_number(argv[1]);
	uint16	value = parse_number(argv[2]);

	return dac_write(dac, value);
    }
    else
    {
	uint16	value = parse_number(argv[1]);

	return dac_write_all(value);
    }
}
/*********************************************************************************************************************/
static Error adc_command(uint argc, const char **argv)
{
    CheckB(argc >= 1 && argc <= 2);

    if (argc == 2)
    {
	uint8	index = parse_number(argv[1]);
	uint16	value;

	Check(adc_read(index, &value));

	write(PSTR("Channel %d: %d\r\n"), index, value);
    }
    else
    {
	uint16	value[4];

	Check(adc_read_all(value));

	for (uint8 i = 0; i < 4; ++i)
	    write(PSTR("Channel %d: %d\r\n"), i, value[i]);
    }

    return success;
}
/*********************************************************************************************************************/
static Error gain_command(uint argc, const char **argv)
{
    CheckB(argc == 2);

    uint16	value = parse_number(argv[1]);

    return gain_write_all(value);
}
/*********************************************************************************************************************/
static Error up_command(uint argc, const char **argv)
{
    for (uint8 i = 0; i < 4; ++i)
    {
	_dac_offset[i] += 1;
	Check(dac_write(i, _dac_offset[i]));
    }

    write(PSTR("Offsets: %d %d %d %d\r\n"),
	  _dac_offset[0],
	  _dac_offset[1],
	  _dac_offset[2],
	  _dac_offset[3]);

    return success;
}
/*********************************************************************************************************************/
static Error down_command(uint argc, const char **argv)
{
    for (uint8 i = 0; i < 4; ++i)
    {
	_dac_offset[i] -= 1;
	Check(dac_write(i, _dac_offset[i]));
    }

    write(PSTR("Offsets: %d %d %d %d\r\n"),
	  _dac_offset[0],
	  _dac_offset[1],
	  _dac_offset[2],
	  _dac_offset[3]);

    return success;
}
/*********************************************************************************************************************/
static Error test_command(uint argc, const char **argv)
{
    CheckB(argc == 2);

    uint32	sum = 0;
    uint8	adc = parse_number(argv[1]);

    Check(rtc_command(0, null));

    for (uint16 i = 0; i < 1000; ++i)
    {
	uint16	value;

	Check(adc_read(adc, &value));

	sum += value;
    }

    Check(rtc_command(0, null));

    write(PSTR("Total: %ld\r\n"), sum);

    return success;
}
/*********************************************************************************************************************/
const ShellCommand shell_command_gain      PROGMEM = {"gain",      gain_command};
const ShellCommand shell_command_adc       PROGMEM = {"adc",       adc_command};
const ShellCommand shell_command_dac       PROGMEM = {"dac",       dac_command};
const ShellCommand shell_command_search    PROGMEM = {"search",    search_command};
const ShellCommand shell_command_sample    PROGMEM = {"sample",    sample_command};
const ShellCommand shell_command_average   PROGMEM = {"average",   average_command};
const ShellCommand shell_command_test      PROGMEM = {"test",      test_command};
const ShellCommand shell_command_up        PROGMEM = {"up",        up_command};
const ShellCommand shell_command_down      PROGMEM = {"down",      down_command};
/*********************************************************************************************************************/
