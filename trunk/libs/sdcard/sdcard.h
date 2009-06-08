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

#ifndef __sdcard_h__
#define __sdcard_h__

#include "libs/os/os.h"
#include "libs/types/types.h"
#include "libs/error/error.h"

#define SDCARD_MAX_CARDS	1
#define SDCARD_RESPONSE_TIMEOUT	100
#define SDCARD_READ_TIMEOUT	100
#define SDCARD_WRITE_TIMEOUT	250

typedef uint8 SDCardIndex;

typedef enum
{
    cmd_0   = 0,
    cmd_1   = 1,
    cmd_8   = 2,
    cmd_9   = 3,
    cmd_10  = 4,
    cmd_13  = 5,
    cmd_16  = 6,
    cmd_17  = 7,
    cmd_24  = 8,
    cmd_55  = 9,
    cmd_58  = 10,
    acmd_41 = 11,
    acmd_42 = 12,
    cmd_last
} CommandIndex;

Error sd_init     (SDCardIndex card);
Error sd_shutdown (SDCardIndex card);
Error sd_command  (SDCardIndex card, CommandIndex index, uint32 argument, uint8* response);
Error sd_read_base(SDCardIndex card, uint8 *data, uint16 count);

extern const BootModule boot_module_sdcard PROGMEM;

#endif //__sdcard_h__
