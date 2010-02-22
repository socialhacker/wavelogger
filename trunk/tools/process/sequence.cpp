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

#include "process/sequence.h"

#include <time.h>

using namespace Err;
using namespace Data;

/**********************************************************************************************************************/
static const uint8	hour_conversion[24] = {11,  0,  1,  2,  3,  4,
					        5,  6,  7,  8,  9, 10,
					       11, 12, 13, 14, 15, 16,
					       17, 18, 11,  0,  1,  2};

static const bool	hour_invertable[24] = {false, false, false, true,  true,  true,
					       true,  true,  true,  true,  true,  false,
					       true,  true,  true,  true,  true,  true,
					       true,  false, false, false, false, false};

static const uint8	hour_inverse[24] = { 0,  0,  0,  4,  5,  6,  // 0x00 - 0x05
					     7,  8,  9, 10, 11,  0,  // 0x06 - 0x0b
					     13, 14, 15, 16, 17, 18,  // 0x0a - 0x11
					     19,  0,  0,  0,  0,  0}; // 0x12 - 0x18

static const uint32	ticks_per_second = 100;
static const uint32	ticks_per_minute = ticks_per_second * 60;
static const uint32	ticks_per_hour   = ticks_per_minute * 60;
static const uint32	ticks_per_day    = ticks_per_hour   * 24;

/**********************************************************************************************************************/
static int get_hour_from_ticks(uint32 ticks)
{
    return (ticks / ticks_per_hour) % 24;
}
/**********************************************************************************************************************/
static int get_partial_hour_from_ticks(uint32 ticks)
{
    return ticks % ticks_per_hour;
}
/**********************************************************************************************************************/
static uint32 zero_hour_from_ticks(uint32 ticks)
{
    uint32	partial_hour = ticks % ticks_per_hour;
    uint32	days         = ticks / ticks_per_day;

    return (days * ticks_per_day) + partial_hour;
}
/**********************************************************************************************************************/
static uint32 convert_ticks(uint32 ticks)
{
    int		hour      = get_hour_from_ticks(ticks);
    int		new_ticks = zero_hour_from_ticks(ticks);
    int		new_hour  = hour_conversion[hour];

    new_ticks += new_hour * ticks_per_hour;
    new_ticks += ticks_per_day;

    return new_ticks;
}
/**********************************************************************************************************************/
static uint32 invert_ticks(uint32 ticks)
{
    int		hour      = get_hour_from_ticks(ticks);
    int		new_ticks = zero_hour_from_ticks(ticks);
    int		new_hour  = hour_inverse[hour];

    new_ticks += new_hour * ticks_per_hour;
    new_ticks -= ticks_per_day;

    return new_ticks;
}
/**********************************************************************************************************************/
static bool blocks_are_contiguous(uint32 previous, uint32 current)
{
    int64 const	threshold     = 20;
    int64	previous_long = previous;
    int64	current_long  = current;

    return (labs(current_long - previous_long) < threshold);
}
/**********************************************************************************************************************/
static int64 broken_ticks_difference(uint32 previous, uint32 current, int64 threshold)
{
    int64	previous_long = get_partial_hour_from_ticks(previous);
    int64	current_long  = get_partial_hour_from_ticks(current);
    int64	difference    = current_long - previous_long;

    if (labs(difference) < threshold)
	return difference;

    if (labs(difference - ticks_per_hour) < threshold)
	return difference - ticks_per_hour;

    return difference + ticks_per_hour;
}
/**********************************************************************************************************************/
static bool broken_blocks_are_contiguous(uint32 previous, uint32 current)
{
    int64 const	threshold     = 20;
    int64	difference    = broken_ticks_difference(previous, current, threshold);

    return (labs(difference) < threshold);
}
/**********************************************************************************************************************/
Sequence::Sequence(off_t offset) :
    _offset(offset),
    _scanning(true),
    _start(uint32(-1)),
    _stop(uint32(-1)),
    _length(0)
{
}
/**********************************************************************************************************************/
Error Sequence::add_block(Block *block, ProcessBlockCallback callback)
{
    uint32	ticks = block->ticks();

    CheckAssertB(block->type() == Block::data);

    CheckB((_start == uint32(-1)) || blocks_are_contiguous(_stop, ticks));

    callback(block);

    if (_start == uint32(-1))
	_start = ticks;

    _stop    = ticks + 200;
    _length += 512;

    return success;
}
/**********************************************************************************************************************/
Error Sequence::add_broken_block(Block *block, ProcessBlockCallback callback)
{
    uint32	ticks = block->ticks();

    CheckAssertB(block->type() == Block::data_broken_rtc);

    CheckB((_start == uint32(-1)) || broken_blocks_are_contiguous(_stop, ticks));

    if (_scanning)
    {
	int	hour       = get_hour_from_ticks(ticks);
	bool	invertable = hour_invertable[hour];

	block->add_reference();

	_blocks.append(block);

	if (invertable)
	{
	    uint32	inverse = invert_ticks(ticks);

	    _stop     = inverse + 200;
	    _scanning = false;

	    /*
	     * Work backwards through the scanned blocks fixing their timestamps.
	     */
	    for (uint i = _blocks.count(); i > 0; --i)
	    {
		int64	difference = broken_ticks_difference(inverse, _blocks[i - 1]->ticks(), 20);

		inverse -= difference;

		CheckB(convert_ticks(inverse) == _blocks[i - 1]->ticks());

		_blocks[i - 1]->ticks(inverse);

		_start = inverse;

		inverse -= 200;
	    }

	    /*
	     * And then run through them in order handing them back to the caller and freeing them.
	     */
	    for (uint i = 0; i < _blocks.count(); ++i)
	    {
		callback(_blocks[i]);

		_blocks[i]->remove_reference();
	    }
	}
    }
    else
    {
	int64	difference = broken_ticks_difference(_stop, ticks, 20);
	uint32	inverse    = _stop + difference;

	CheckB(convert_ticks(inverse) == block->ticks());

	block->ticks(inverse);

	callback(block);

	_stop = inverse + 200;
    }

    _length += 512;

    return success;
}
/**********************************************************************************************************************/
uint32 Sequence::length()
{
    return _length;
}
/**********************************************************************************************************************/
void Sequence::debug_print(int indent)
{
    int32	delta    = _stop - _start;
    float	hours    = float(delta) / float(100 * 60 * 60);
    time_t	start    = _start / 100;
    time_t	stop     = _stop  / 100;
    struct tm	delta_tm = {0};
    time_t	delta_time;

    delta_tm.tm_sec  = 0;
    delta_tm.tm_min  = 0;
    delta_tm.tm_hour = 0;
    delta_tm.tm_mday = 1;
    delta_tm.tm_mon  = 0;
    delta_tm.tm_year = 100;
    delta_tm.tm_yday = 0;

    delta_time = mktime(&delta_tm);

    start += delta_time;
    stop  += delta_time;

    printf("%*sSequence\n",                indent, "");
    printf("%*s    offset...: 0x%08llx\n", indent, "", (int64)_offset);
    printf("%*s    start....: %d : %s",    indent, "", _start, ctime(&start));
    printf("%*s    stop.....: %d : %s",    indent, "", _stop,  ctime(&stop));
    printf("%*s    hours....: %0.2f\n",    indent, "", hours);
}
/**********************************************************************************************************************/
