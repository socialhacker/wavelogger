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
static void null_callback(Block *block)
{
}
/**********************************************************************************************************************/
Wavefile::Wavefile() :
    _block(new Block()),
    _file(-1),
    _callback(null_callback)
{
}
/**********************************************************************************************************************/
Wavefile::~Wavefile()
{
    _block->remove_reference();

    for (uint i = 0; i < _sequences.length(); ++i)
	delete _sequences[i];
}
/**********************************************************************************************************************/
Error Wavefile::read(Path path, Sequence::ProcessBlockCallback callback)
{
    const char	*filename = path.get().str();

    CheckB(callback);

    _callback = callback;

    CheckStringP(_file = open(filename, O_RDONLY), "Failed to open file %s", filename);

    Check(match(Block::invalid));

    while (_block->type() != Block::eof)
	Check(read_sequence());

    return success;
}
/**********************************************************************************************************************/
Error Wavefile::read_sequence()
{
    Error	check_error = success;
    off_t	offset      = static_cast<off_t>(-1);

    if (_block->type() == Block::header)
    {
	offset = _block->offset();
	_callback(_block);
    }

    Check(match(Block::header));

    printf("Reading sequence at 0x%08llx (%llu)\n", (int64)offset, _block->ticks());

    Sequence *	sequence = new Sequence(offset);

    while (_block->type() == Block::data ||
	   _block->type() == Block::data_broken_rtc)
    {
	Block::Type	type = _block->type();

	switch (type)
	{
	    case Block::data:
		CheckCleanup(sequence->add_block(_block, _callback), failure);
		break;

	    case Block::data_broken_rtc:
		CheckCleanup(sequence->add_broken_block(_block, _callback), failure);
		break;

	    default:
		CheckAssertB(false);
	}

	CheckCleanup(match(type), failure);
    }

    while (_block->type() == Block::empty)
	CheckCleanup(match(Block::empty), failure);

    _sequences.append(sequence);

    return success;

  failure:
    delete sequence;

    return check_error;
}
/**********************************************************************************************************************/
Error Wavefile::match(const Block::Type type)
{
    CheckStringB(type == _block->type(), "Missmatched block type %d != %d", type, _block->type());

    _block->remove_reference();

    _block = new Block();

    _block->read(_file);

    return success;
}
/**********************************************************************************************************************/
void Wavefile::debug_print(int indent)
{
    printf("%*sWavefile\n",              indent, "");

    for (uint i = 0; i < _sequences.length(); ++i)
	_sequences[i]->debug_print(indent + 4);
}
/**********************************************************************************************************************/
