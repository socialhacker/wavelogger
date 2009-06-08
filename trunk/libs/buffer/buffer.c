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
#include "libs/error/error.h"
#include "libs/stdio/console.h"
#include "libs/stdio/debug.h"
#include "libs/buffer/buffer.h"

SetupError();

typedef struct
{
    bool	valid;
    bool	dirty;
    BlockIndex	block;
    uint32	base;
    uint32	address;
    uint8	buffer[BYTES_PER_BLOCK];
} Buffer;

static Buffer	_buffer;

/*********************************************************************************************************************/
static Error buffer_fill(uint16 offset)
{
    uint32	address = _buffer.base + (offset / BYTES_PER_BLOCK);

    if (_buffer.valid && (address == _buffer.address))
	return success;

    Check(buffer_sync());
    Check(block_read(_buffer.block, address, _buffer.buffer));

    _buffer.address = address;
    _buffer.valid   = true;

    return success;
}
/*********************************************************************************************************************/
Error buffer_set_address(BlockIndex index, uint32 base)
{
    if (_buffer.block != index ||
	_buffer.base  != base)
    {
	_buffer.block = index;
	_buffer.base  = base;
	_buffer.valid = false;
    }

    return success;
}
/*********************************************************************************************************************/
uint32 buffer_get_address()
{
    return _buffer.base;
}
/*********************************************************************************************************************/
Error buffer_sync()
{
    if (_buffer.dirty)
	Check(block_write(_buffer.block, _buffer.address, _buffer.buffer));

    _buffer.dirty = false;

    return success;
}
/*********************************************************************************************************************/
uint8 buffer_uint8(uint16 offset)
{
    if (buffer_fill(offset) != success) return 0;

    return _buffer.buffer[offset & (BYTES_PER_BLOCK - 1)];
}
/*********************************************************************************************************************/
uint16 buffer_uint16(uint16 offset)
{
    uint16	result;

    result = buffer_uint8(offset + 1);
    result <<= 8;
    result |= buffer_uint8(offset + 0);

    return result;
}
/*********************************************************************************************************************/
uint32 buffer_uint32(uint16 offset)
{
    uint32	result;

    result = buffer_uint8(offset + 3);
    result <<= 8;
    result |= buffer_uint8(offset + 2);
    result <<= 8;
    result |= buffer_uint8(offset + 1);
    result <<= 8;
    result |= buffer_uint8(offset + 0);

    return result;
}
/*********************************************************************************************************************/
Error buffer_set_uint8 (uint16 offset, uint8 value)
{
    Check(buffer_fill(offset));

    _buffer.buffer[offset & (BYTES_PER_BLOCK - 1)] = value;
    _buffer.dirty = true;

    return success;
}
/*********************************************************************************************************************/
Error buffer_set_uint16(uint16 offset, uint16 value)
{
    Check(buffer_set_uint8(offset + 0, (value >> 0) & 0xff));
    Check(buffer_set_uint8(offset + 1, (value >> 8) & 0xff));
    return success;
}
/*********************************************************************************************************************/
Error buffer_set_uint32(uint16 offset, uint32 value)
{
    Check(buffer_set_uint8(offset + 0, (value >>  0) & 0xff));
    Check(buffer_set_uint8(offset + 1, (value >>  8) & 0xff));
    Check(buffer_set_uint8(offset + 2, (value >> 16) & 0xff));
    Check(buffer_set_uint8(offset + 3, (value >> 24) & 0xff));
    return success;
}
/*********************************************************************************************************************/
