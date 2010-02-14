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
#ifndef __process_wavefile_h__
#define __process_wavefile_h__

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "globals.h"
#include "libs/error/error.h"
#include "libs/data/array.h"
#include "libs/files/path.h"

/**********************************************************************************************************************/
class Block
{
    typedef Err::Error	Error;

public:
    enum Type
    {
	unknown         = 0,
	invalid         = 1,
	empty           = 2,
	header          = 3,
	data            = 4,
	data_broken_rtc = 5,
	eof             = 6
    };

private:
    struct AnalogSample
    {
	uint8		data[5];
    };

    struct Data
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
    } __attribute__((packed));

    static_assert(sizeof(AnalogSample) == 5,   sizeof_AnalogSample_not_5_bytes);
    static_assert(sizeof(Data)         == 512, sizeof_Data_not_512_bytes);

    struct Header
    {
	uint8		type;
	uint8		reset_reason;
	uint8		padding0[2];
	uint16		offset[4];
	uint8		padding[512 - 12];
    };

    static_assert(sizeof(Header) == 512, sizeof_Header_not_512_bytes);

    union
    {
	Header	_header;
	Data	_data;
	uint8	_buffer[512];
    };

    off_t	_offset;
    Type	_type;
    uint64	_ticks;

public:
    Block();

    Error  read(int file);
    off_t  offset();
    Type   type();
    uint64 ticks();
    uint16 sample(int tick, int channel);
    uint8  battery();
    uint8  temperature();
};
/**********************************************************************************************************************/
class Sequence
{
    typedef Err::Error	Error;

    uint64	_start;
    uint64	_stop;
    uint32	_length;

public:
    Sequence();

    Error add_block(Block *block);
    uint32 length();

    void debug_print(int indent);
};
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
class Wavefile
{
    typedef Err::Error	Error;
    typedef void (*ProcessBlockCallback)(Block *block);

    Block			*_block;
    Block			*_old_block;
    int				_file;
    Data::Array<Segment *>	_segments;
    ProcessBlockCallback	_callback;

    Error read_segment();
    Error read_sequence(Segment *segment);
    Error match(const Block::Type type);

public:
    Wavefile();
    virtual ~Wavefile();

    Error read(Files::Path path, ProcessBlockCallback callback);
    void debug_print(int indent);
};
/**********************************************************************************************************************/

#endif //__process_wavefile_h__
