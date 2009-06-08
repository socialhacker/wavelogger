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
#ifndef __os_arch_h__
#define __os_arch_h__

#include "libs/types/types.h"

/*
 *
 */
typedef void (*ArchTickFunction)(void);

typedef struct ArchTickHandlerStruct
{
    ArchTickFunction			function;
    struct ArchTickHandlerStruct	*next;
} ArchTickHandler;

void arch_load_oscal();
void arch_tick_timer_init();
void arch_tick_timer_add(ArchTickHandler *handler);
uint8 arch_reset_reason();
uint16 arch_stack_bound();
uint64 arch_cycles();

#endif //__os_arch_h__
