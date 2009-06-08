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
#include "libs/stdio/write.h"
#include "libs/block/block.h"
#include "libs/buffer/buffer.h"
#include "libs/partition/partition.h"

SetupError();

#define FAT_16_TYPE		0x06
#define FAT_32_TYPE		0x0b
#define BYTES_PER_SECTOR	0x0200
#define SIGNATURE		0xaa55

Partition		_partition_table[PARTITION_COUNT_MAX] = {{0, 0, 0}};

static const uint16	_partition_offset[4] PROGMEM =
{
    0x1ee,
    0x1de,
    0x1ce,
    0x1be,
};

/**************************************************************************************************/
static Error partition_allocate(BlockIndex *index, BlockIndex block, uint32 start, uint32 length)
{
    for (uint i = 0; i < PARTITION_COUNT_MAX; ++i)
    {
	if (_partition_table[i].length == 0)
	{
	    *index = i;
	    _partition_table[i].block  = block;
	    _partition_table[i].start  = start;
	    _partition_table[i].length = length;
	    return success;
	}
    }

    return failure;
}
/**************************************************************************************************/
static Error partition_block_read(void *data, uint32 address, uint8 *buffer)
{
    Partition	*partition = (Partition *)data;

    Check(block_read(partition->block,
		     (partition->start * BYTES_PER_SECTOR) / BYTES_PER_BLOCK + address,
		     buffer));

    return success;
}
/**************************************************************************************************/
static Error partition_block_write(void *data, uint32 address, uint8 *buffer)
{
    Partition	*partition = (Partition *)data;

    Check(block_write(partition->block,
		      (partition->start * BYTES_PER_SECTOR) / BYTES_PER_BLOCK + address,
		      buffer));

    return success;
}
/**************************************************************************************************/
static BlockOps	partition_block_ops =
{
    partition_block_read,
    partition_block_write
};
/**************************************************************************************************/
static bool parition_valid(uint16 offset, uint32 *start, uint32 *length)
{
    uint8	type = buffer_uint8(offset + 0x0004);

    if (type != FAT_16_TYPE &&
	type != FAT_32_TYPE)
	return false;

    *start  = buffer_uint32(offset + 0x0008);
    *length = buffer_uint32(offset + 0x000c);

    return true;
}
/**************************************************************************************************/
Error partition_block_device(BlockIndex index)
{
    Check(buffer_set_address(index, 0));

    CheckB(buffer_uint16(0x1fe) == SIGNATURE);

    for (uint i = 0; i < 4; ++i)
    {
	uint16	offset = pgm_read_word(&(_partition_offset[i]));
	uint32	start  = 0;
	uint32	length = 0;

	if (parition_valid(offset, &start, &length))
	{
	    BlockIndex	new_partition_index = 0xff;
	    BlockIndex	new_block_index     = 0xff;

	    Check(partition_allocate(&new_partition_index, index, start, length));
	    Check(block_allocate(&new_block_index,
				 &partition_block_ops,
				 _partition_table + new_partition_index));
	}
    }

    return success;
}
/**************************************************************************************************/
