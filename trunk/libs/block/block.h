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
#ifndef __block_block_h__
#define __block_block_h__

#include "libs/types/types.h"
#include "libs/error/error.h"

#define	BLOCK_COUNT_MAX		5
#define BYTES_PER_BLOCK		0x200

typedef uint8	BlockIndex;

typedef struct
{
    Error (*read)  (void *data, uint32 address, uint8 *buffer);
    Error (*write) (void *data, uint32 address, uint8 *buffer);
} BlockOps;

typedef struct
{
    BlockOps	*ops;
    void	*data;
} Block;

Error block_allocate(BlockIndex *index, BlockOps *ops, void *data);
Error block_read    (BlockIndex  index, uint32 address, uint8 *buffer);
Error block_write   (BlockIndex  index, uint32 address, uint8 *buffer);

#endif //__block_block_h__
