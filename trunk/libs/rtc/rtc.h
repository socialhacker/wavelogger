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
#ifndef __rtc_h__
#define __rtc_h__

#include "libs/types/types.h"
#include "libs/error/error.h"

typedef struct
{
    uint8 year;
    uint8 month;
    uint8 date;
    uint8 hour;
    uint8 minute;
    uint8 second;
    uint8 tick;
} RTC;

Error rtc_init();
Error rtc_read();
Error rtc_write();
Error rtc_reset_stack();
uint32 rtc_ticks(const uint8 *data);

extern RTC	rtc;

#endif //__rtc_h__
