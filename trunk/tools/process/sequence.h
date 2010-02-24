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

#ifndef __process_sequence_h__
#define __process_sequence_h__

#include <unistd.h>

#include "globals.h"
#include "libs/error/error.h"
#include "libs/data/array.h"

#include "process/block.h"

/**********************************************************************************************************************/
class Sequence
{
public:
    typedef void (*ProcessBlockCallback)(Block *block);

private:
    typedef Err::Error	Error;

    off_t			_offset;
    bool			_scanning;
    uint64			_start;
    uint64			_stop;
    uint32			_length;
    Data::Array<Block *>	_blocks;

public:
    Sequence(off_t offset);

    Error add_block       (Block *block, ProcessBlockCallback callback);
    Error add_broken_block(Block *block, ProcessBlockCallback callback);
    uint32 length();

    void debug_print(int indent);
};
/**********************************************************************************************************************/

#endif //__process_sequence_h__
