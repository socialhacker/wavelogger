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
#ifndef __buffer_h__
#define __buffer_h__

#include "libs/types/types.h"
#include "libs/error/error.h"
#include "libs/block/block.h"

Error  buffer_set_address(BlockIndex index, uint32 address);
uint32 buffer_get_address();
Error  buffer_sync();

uint8  buffer_uint8 (uint16 offset);
uint16 buffer_uint16(uint16 offset);
uint32 buffer_uint32(uint16 offset);

Error buffer_set_uint8 (uint16 offset, uint8 value);
Error buffer_set_uint16(uint16 offset, uint16 value);
Error buffer_set_uint32(uint16 offset, uint32 value);

#endif //__buffer_h__
