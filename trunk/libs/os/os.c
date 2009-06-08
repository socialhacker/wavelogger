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
#include <avr/interrupt.h>
#include <util/delay.h>

#include "libs/os/os.h"
#include "libs/os/arch.h"

SetupError();

static SystemData	_system_data;
static ThreadData	_thread_data;

/**************************************************************************************************/
static void os_tick()
{
    uint16	current_stack = SP;

    if (_thread_data.stack_bound > current_stack)
	_thread_data.stack_bound = current_stack;

    ++_system_data.ticks;
}
/**************************************************************************************************/
Error os_boot(const BootModule *boot_module_table[], uint boot_module_count)
{
    _system_data.reset_reason  = arch_reset_reason();
    _system_data.tick.function = os_tick;
    _thread_data.stack_bound   = RAMEND;

    arch_load_oscal();
    arch_tick_timer_init();
    arch_tick_timer_add(&(_system_data.tick));

    sei();

    for (_system_data.boot_index = 0;
	 _system_data.boot_index < boot_module_count;
	 _system_data.boot_index++)
    {
	uint			index = _system_data.boot_index;
	const BootModule	*boot_module;
	BootModuleInitialize	initialize;

	boot_module = (const BootModule *) pgm_read_word(&(boot_module_table[index]));
	initialize  = (BootModuleInitialize) pgm_read_word(&(boot_module->initialize));

	Check(initialize());
    }

    return success;
}
/**************************************************************************************************/
uint8 os_reset_reason()
{
    return _system_data.reset_reason;
}
/**************************************************************************************************/
uint os_boot_index()
{
    return _system_data.boot_index;
}
/**************************************************************************************************/
uint16 os_stack_bound()
{
    return _thread_data.stack_bound;
}
/**************************************************************************************************/
uint64 os_cycles()
{
    return arch_cycles();
}
/**************************************************************************************************/
uint32 os_ticks()
{
    return _system_data.ticks;
}
/**************************************************************************************************/
void os_sleep(uint ticks)
{
    /*
     * FIX FIX FIX: This is wrong.  It assumes no time is taken by interrupt handlers.  It should
     * be replaced with a real wait queue.
     */
    for (uint i = 0; i < ticks; ++i)
	_delay_ms(10);
}
/**************************************************************************************************/
void os_sleep_ms(uint ms)
{
    /*
     * FIX FIX FIX: This is wrong.  It assumes no time is taken by interrupt handlers.  It should
     * be replaced with a real wait queue.
     */
    for (uint i = 0; i < ms; ++i)
	_delay_ms(1);
}
/**************************************************************************************************/
