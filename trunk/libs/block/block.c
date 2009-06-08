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
#include "libs/block/block.h"

SetupError();

Block	_block_table[BLOCK_COUNT_MAX] = {{null, null}};

/**************************************************************************************************/
Error block_allocate(BlockIndex *index, BlockOps *ops, void *data)
{
    for (uint i = 0; i < BLOCK_COUNT_MAX; ++i)
    {
	if (_block_table[i].ops == null)
	{
	    *index = i;
	    _block_table[i].ops  = ops;
	    _block_table[i].data = data;
	    return success;
	}
    }

    return failure;
}
/**************************************************************************************************/
Error block_read(BlockIndex index, uint32 address, uint8 *buffer)
{
    Block	*block;

    CheckB(index < BLOCK_COUNT_MAX);
    CheckB(_block_table[index].ops != null);

    block = &(_block_table[index]);

    block->ops->read(block->data, address, buffer);

    return success;
}
/**************************************************************************************************/
Error block_write(BlockIndex index, uint32 address, uint8 *buffer)
{
    Block	*block;

    CheckB(index < BLOCK_COUNT_MAX);
    CheckB(_block_table[index].ops != null);

    block = &(_block_table[index]);

    block->ops->write(block->data, address, buffer);

    return success;
}
/**************************************************************************************************/
