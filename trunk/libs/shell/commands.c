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
#include <avr/pgmspace.h>

#include "libs/shell/commands.h"
#include "libs/stdio/write.h"
#include "libs/os/os.h"

/*********************************************************************************************************************/
Error time_command(uint argc, const char **argv)
{
    write(PSTR("Clock: %ld\r\n"), os_ticks());

    return success;
}
/*********************************************************************************************************************/
Error stack_command(uint argc, const char **argv)
{
    extern uint16 __attribute__((weak))	__stack;
    extern uint16 __attribute__((weak))	__bss_end;
    extern uint16 __attribute__((weak))	__bss_start;
    extern uint16 __attribute__((weak))	__data_end;
    extern uint16 __attribute__((weak))	__data_start;

    uint16				data_start  = (uint16)(&__data_start);
    uint16				data_end    = (uint16)(&__data_end);
    uint16				bss_start   = (uint16)(&__bss_start);
    uint16				bss_end     = (uint16)(&__bss_end);
    uint16				stack_end   = (uint16)(&__stack);
    uint16				stack_start = os_stack_bound();

    write(PSTR("Data  %x - %x (%x)\r\n"), data_start,  data_end,    data_end    - data_start);
    write(PSTR("BSS   %x - %x (%x)\r\n"), bss_start,   bss_end,     bss_end     - bss_start);
    write(PSTR("Free  %x - %x (%x)\r\n"), bss_end,     stack_start, stack_start - bss_end);
    write(PSTR("Stack %x - %x (%x)\r\n"), stack_start, stack_end,   stack_end   - stack_start);

    return success;
}
/*********************************************************************************************************************/
Error status_command(uint argc, const char **argv)
{
    write(PSTR("Boot index: %d\r\n"), os_boot_index());
    return success;
}
/*********************************************************************************************************************/
