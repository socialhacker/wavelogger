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

#include "process/block.h"

#include <time.h>

using namespace Err;
using namespace Data;
using namespace Files;

/**********************************************************************************************************************/
Block::Block() :
    _offset(0),
    _type(invalid),
    _ticks(0)
{
}
/**********************************************************************************************************************/
Error Block::read(int file)
{
    ssize_t	length;

    CheckP(length  = ::read(file, _buffer, 512));
    CheckP(_offset = lseek(file, 0, SEEK_CUR));

    _offset -= 512;
    _type    = data;
    _ticks   = 0;

    if (length != 512)
    {
	if (length == 0)
	    _type = eof;
	else
	    _type = invalid;

	return success;
    }

    {
	uint32	sum = 0;

	for (int i = 0; i < 512; ++i)
	    sum += _buffer[i];

	if (sum == 0)
	{
	    _type = empty;
	    return success;
	}
    }

    switch (_data.type)
    {
	case 0x00: _type = header;          break;
	case 0x01: _type = data_broken_rtc; break;
	case 0x02: _type = data;            break;
	default:   _type = invalid;         break;
    }

    if (_type == data ||
	_type == data_broken_rtc)
	_ticks = _data.rtc_ticks;

    return success;
}
/**********************************************************************************************************************/
off_t Block::offset()
{
    return _offset;
}
/**********************************************************************************************************************/
Block::Type Block::type()
{
    return _type;
}
/**********************************************************************************************************************/
uint64 Block::ticks()
{
    return _ticks;
}
/**********************************************************************************************************************/
uint16 Block::sample(int tick, int channel)
{
    if (_type != data &&
	_type != data_broken_rtc)
	return 0;

    const AnalogSample	&sample = _data.sample[tick >> 1];

    switch (channel)
    {
	case 0:
	    if ((tick & 1) == 0)
		return sample.data[1] | ((sample.data[0] & 0xc0) << 2);
	    else
		return sample.data[3] | ((sample.data[0] & 0x0c) << 6);

	case 1:
	    if ((tick & 1) == 0)
		return sample.data[2] | ((sample.data[0] & 0x30) << 4);
	    else
		return sample.data[4] | ((sample.data[0] & 0x03) << 8);

	default:
	    return 0;
    }
}
/**********************************************************************************************************************/
uint8 Block::battery()
{
    return _data.battery;
}
/**********************************************************************************************************************/
uint8 Block::temperature()
{
    return _data.internal_temperature;
}
/**********************************************************************************************************************/
