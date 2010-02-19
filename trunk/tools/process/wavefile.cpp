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
#include "process/wavefile.h"

#include <time.h>

using namespace Err;
using namespace Data;
using namespace Files;

/**********************************************************************************************************************/
bool	hour_invertable[24] = {false, false, false, true,  true,  true,
			       true,  true,  true,  true,  true,  false,
			       true,  true,  true,  true,  true,  true,
			       true,  false, false, false, false, false};

uint8	hour_inverse[24] = { 0,  0,  0,  4,  5,  6,  // 0x00 - 0x05
			     7,  8,  9, 10, 11,  0,  // 0x06 - 0x0b
			    13, 14, 15, 16, 17, 18,  // 0x0a - 0x11
			    19,  0,  0,  0,  0,  0}; // 0x12 - 0x18

/**********************************************************************************************************************/
static int get_hour_from_ticks(uint32 ticks)
{
    return (ticks / (100 * 60 * 60)) % 24;
}
/**********************************************************************************************************************/
static bool blocks_are_not_contiguous(Block *previous, Block *current)
{
    CheckAssertB(previous->type() == current->type());
    CheckAssertB(current->type() == Block::data ||
		 current->type() == Block::data_broken_rtc);

    uint32	current_ticks  = current->ticks();
    uint32	previous_ticks = previous->ticks();
    uint const	threshold      = 20;

    switch (current->type())
    {
	case Block::data:
	    return (abs(current_ticks - previous_ticks - 200) > threshold);

	case Block::data_broken_rtc:
	    return (abs(current_ticks - previous_ticks - 200) > threshold);

	default:
	    CheckAssertB(false);
    }

    return false;
}
/**********************************************************************************************************************/
class Sequence
{
    typedef Err::Error	Error;

    bool			_scanning;
    uint64			_start;
    uint64			_stop;
    uint32			_length;
    Data::Array<Block *>	_blocks;

public:
    Sequence();

    Error add_block(Block *block, Wavefile::ProcessBlockCallback callback);
    uint32 length();

    void debug_print(int indent);
};
/**********************************************************************************************************************/
Sequence::Sequence() :
    _scanning(true),
    _start(uint64(-1)),
    _stop(uint64(-1)),
    _length(0)
{
}
/**********************************************************************************************************************/
Error Sequence::add_block(Block *block, Wavefile::ProcessBlockCallback callback)
{
    uint64	ticks = block->ticks();

    if (_scanning)
    {
	int	hour       = get_hour_from_ticks(ticks);
	bool	invertable = hour_invertable[hour];

	if (invertable)
	{
	    printf("Done scanning\n");
	    _scanning = false;
	}

	callback(block);
    }
    else
    {
	callback(block);
    }

    _blocks.append(block);

    if (_start == uint64(-1))
	_start = ticks;

    _stop    = ticks + 200;
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
    int64	delta = _stop - _start;
    float	hours = float(delta) / float(100 * 60 * 60);
    time_t	start = _start / 100;
    time_t	stop  = _stop  / 100;
    struct tm	delta_tm;
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

    printf("%*sSequence\n",               indent, "");
    printf("%*s    start....: %lld : %s", indent, "", _start, ctime(&start));
    printf("%*s    stop.....: %lld : %s", indent, "", _stop,  ctime(&stop));
    printf("%*s    hours....: %0.2f\n",   indent, "", hours);
}
/**********************************************************************************************************************/
class Segment
{
    typedef Err::Error	Error;

    off_t			_offset;
    uint32			_length;
    Data::Array<Sequence *>	_sequences;

public:
    Segment(Block *header);

    Error add_sequence(Sequence *sequence);

    void debug_print(int indent);
};
/**********************************************************************************************************************/
Segment::Segment(Block *header) :
    _offset(header->offset()),
    _length(0)
{
}
/**********************************************************************************************************************/
Error Segment::add_sequence(Sequence *sequence)
{
    _sequences.append(sequence);
    _length += sequence->length();

    return success;
}
/**********************************************************************************************************************/
void Segment::debug_print(int indent)
{
    printf("%*sSegment\n",                 indent, "");
    printf("%*s    offset...: 0x%08llx\n", indent, "", (int64)_offset);
    printf("%*s    length...: 0x%08x\n",   indent, "", _length);

    for (uint i = 0; i < _sequences.length(); ++i)
	_sequences[i]->debug_print(indent + 4);
}
/**********************************************************************************************************************/
static void null_callback(Block *block)
{
}
/**********************************************************************************************************************/
Wavefile::Wavefile() :
    _block(new Block()),
    _old_block(new Block()),
    _file(-1),
    _callback(null_callback)
{
}
/**********************************************************************************************************************/
Wavefile::~Wavefile()
{
    delete _block;
    delete _old_block;
}
/**********************************************************************************************************************/
Error Wavefile::read(Path path, ProcessBlockCallback callback)
{
    const char	*filename = path.get().str();

    CheckB(callback);

    _callback = callback;

    CheckStringP(_file = open(filename, O_RDONLY), "Failed to open file %s", filename);

    Check(match(Block::invalid));

    while (_block->type() != Block::eof)
	Check(read_segment());

    return success;
}
/**********************************************************************************************************************/
Error Wavefile::read_segment()
{
    printf("Reading segment  at 0x%08llx\n", (int64)_block->offset());

    Check(match(Block::header));
    _callback(_old_block);

    Segment	*segment = new Segment(_old_block);

    while (_block->type() == Block::data ||
	   _block->type() == Block::data_broken_rtc)
	Check(read_sequence(segment));

    while (_block->type() == Block::empty)
	Check(match(Block::empty));

    _segments.append(segment);

    return success;
}
/**********************************************************************************************************************/
Error Wavefile::read_sequence(Segment *segment)
{
    printf("Reading sequence at 0x%08llx (0x%08llx)\n", (int64)_block->offset(), _block->ticks());

    uint64	ticks     = uint64(-1);
    Sequence	*sequence = new Sequence();

    while (_block->type() == Block::data ||
	   _block->type() == Block::data_broken_rtc)
    {
	if ((ticks != uint64(-1)) && blocks_are_not_contiguous(_old_block, _block))
	    break;

	Check(match(_block->type()));

	ticks = _old_block->ticks();

	Check(sequence->add_block(_old_block, _callback));
    }

    Check(segment->add_sequence(sequence));

    return success;
}
/**********************************************************************************************************************/
Error Wavefile::match(const Block::Type type)
{
    CheckStringB(type == _block->type(), "Missmatched block type %d != %d", type, _block->type());

    swap(_block, _old_block);

    _block->read(_file);

    return success;
}
/**********************************************************************************************************************/
void Wavefile::debug_print(int indent)
{
    printf("%*sWavefile\n",              indent, "");

    for (uint i = 0; i < _segments.length(); ++i)
	_segments[i]->debug_print(indent + 4);
}
/**********************************************************************************************************************/
