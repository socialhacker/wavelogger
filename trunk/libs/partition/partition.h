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
#ifndef __partition_partition_h__
#define __partition_partition_h__

#include "libs/types/types.h"
#include "libs/error/error.h"
#include "libs/block/block.h"

#define	PARTITION_COUNT_MAX		2

typedef uint8	PartitionIndex;

typedef struct
{
    BlockIndex	block;
    uint32	start;
    uint32	length;
} Partition;

Error partition_block_device(BlockIndex index);

#endif //__partition_partition_h__
