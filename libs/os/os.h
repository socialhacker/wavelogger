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
#ifndef __os_h__
#define __os_h__

#include <avr/pgmspace.h>

#include "libs/error/error.h"
#include "libs/types/types.h"
#include "libs/os/arch.h"
#include "libs/os/sleep.h"

/*
 *
 */
typedef Error (*BootModuleInitialize)(void);

typedef struct
{
    BootModuleInitialize	initialize;
} BootModule;

Error os_boot(const BootModule *boot_module_table[], uint boot_module_count);

/*
 *
 */
uint8  os_reset_reason();
uint   os_boot_index();
uint16 os_stack_bound();
uint64 os_cycles();
uint32 os_ticks();
void   os_sleep(uint ticks);
void   os_sleep_ms(uint ms);

/*
 *
 */
typedef struct
{
    uint16	stack_bound;
} ThreadData;

/*
 *
 */
typedef struct
{
    uint		boot_index;
    uint8		reset_reason;
    ArchTickHandler	tick;
    uint32		ticks;
} SystemData;

#endif //__os_h__
