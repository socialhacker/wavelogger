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
    Check(match(Block::header));
    _callback(_old_block);

    printf("Reading sequence at 0x%08llx (0x%08x)\n", (int64)_block->offset(), _block->ticks());

    Sequence	*sequence = new Sequence(_old_block);

    while (_block->type() == Block::data ||
	   _block->type() == Block::data_broken_rtc)
    {
	switch (_block->type())
	{
	    case Block::data:
		Check(sequence->add_block(_block, _callback));
		break;

	    case Block::data_broken_rtc:
		Check(sequence->add_broken_block(_block, _callback));
		break;

	    default:
		CheckAssertB(false);
	}

	Check(match(_block->type()));
    }

    while (_block->type() == Block::empty)
	Check(match(Block::empty));

    _sequences.append(sequence);

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

    for (uint i = 0; i < _sequences.length(); ++i)
	_sequences[i]->debug_print(indent + 4);
}
/**********************************************************************************************************************/
