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

static uint16	_dac_offset[4];

/*********************************************************************************************************************/
Error search_command(uint argc, const char **argv)
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
static AnalogData volatile	data[2];
static AnalogMessage		message[2] = {{(uint8 volatile *) &(data[0]), 200},
					      {(uint8 volatile *) &(data[1]), 200}};

Error sample_command(uint argc, const char **argv)
{
    uint8	index = 0;

    Check(analog_queue_message(&(message[0])));
    Check(analog_queue_message(&(message[1])));

    while (!uart_character_ready())
    {
	Check(analog_wait_message(&(message[index])));

	for (uint i = 0; i < 100; ++i)
	{
	    AnalogSample	*sample = &(data[index].sample[i]);
	    uint16		channel[4];

	    channel[0] = ((uint16)(sample->data[0] & 0xc0)) << 2 | sample->data[1];
	    channel[1] = ((uint16)(sample->data[0] & 0x30)) << 4 | sample->data[2];
	    channel[2] = ((uint16)(sample->data[0] & 0x0c)) << 6 | sample->data[3];
	    channel[3] = ((uint16)(sample->data[0] & 0x03)) << 8 | sample->data[4];

	    write(PSTR("<data> %d %d %d %d\r\n"), channel[0], channel[1], channel[2], channel[3]);
	}

	Check(analog_queue_message(&(message[index])));

	index = !index;
    }

    return success;
}
/*********************************************************************************************************************/
Error dac_command(uint argc, const char **argv)
{
    CheckB(argc >= 2 && argc <= 3);

    if (argc == 3)
    {
	uint8	dac   = parse_hex(argv[1]);
	uint16	value = parse_hex(argv[2]);

	return dac_write(dac, value);
    }
    else
    {
	uint16	value = parse_hex(argv[1]);

	return dac_write_all(value);
    }
}
/*********************************************************************************************************************/
Error adc_command(uint argc, const char **argv)
{
    CheckB(argc >= 1 && argc <= 2);

    if (argc == 2)
    {
	uint8	index = parse_hex(argv[1]);
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
Error gain_command(uint argc, const char **argv)
{
    CheckB(argc == 2);

    uint16	value = parse_hex(argv[1]);

    return gain_write_all(value);
}
/*********************************************************************************************************************/
Error up_command(uint argc, const char **argv)
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
Error down_command(uint argc, const char **argv)
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
Error test_command(uint argc, const char **argv)
{
    CheckB(argc == 2);

    uint32	sum = 0;
    uint8	adc = parse_hex(argv[1]);

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
