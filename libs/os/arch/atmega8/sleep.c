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
#include <avr/sleep.h>
#include <avr/interrupt.h>

#include "libs/os/sleep.h"

volatile static uint32	_inhibit_deep_sleep = 0;

/**************************************************************************************************/
void os_inhibit_deep_sleep(uint8 vector)
{
    os_set_deep_sleep(vector, false);
}
/**************************************************************************************************/
void os_enable_deep_sleep(uint8 vector)
{
    os_set_deep_sleep(vector, true);
}
/**************************************************************************************************/
void os_set_deep_sleep(uint8 vector, bool enable)
{
    if (enable)
	_inhibit_deep_sleep &= ~((uint32)1 << vector);
    else
	_inhibit_deep_sleep |= ((uint32)1 << vector);
}
/**************************************************************************************************/
void os_sleep_cpu(void *user_data, SleepCheck sleep_check)
{
    do
    {
	cli();

	if (sleep_check(user_data))
	{
	    if (_inhibit_deep_sleep) set_sleep_mode(SLEEP_MODE_IDLE);
	    else                     set_sleep_mode(SLEEP_MODE_ADC);

	    sleep_enable();
	    sei();
	    sleep_cpu();
	    sleep_disable();
	}

	sei();
    }
    while (sleep_check(user_data));
}
/**************************************************************************************************/
