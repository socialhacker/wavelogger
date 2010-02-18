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

#include "process/block.h"

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
