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
#include "process/sequence.h"

/**********************************************************************************************************************/
class Wavefile
{
    typedef Err::Error	Error;

    Block				*_block;
    int					_file;
    Data::Array<class Sequence *>	_sequences;
    Sequence::ProcessBlockCallback	_callback;

    Error read_sequence();
    Error match(const Block::Type type);

public:
    Wavefile();
    virtual ~Wavefile();

    Error read(Files::Path path, Sequence::ProcessBlockCallback callback);
    void debug_print(int indent);
};
/**********************************************************************************************************************/

#endif //__process_wavefile_h__
