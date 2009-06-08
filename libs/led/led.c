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
#include "libs/led/led.h"

/*********************************************************************************************************************/
const BootModule boot_module_led PROGMEM =
{
    led_init
};
/*********************************************************************************************************************/
Error led_init()
{
    DDRA = 0x03;
    DDRB = 0x18;

    for (uint i = 0; i < 20; ++i)
    {
	led_set(i & 3, true);
	led_set((i + 3) & 3, false);
	os_sleep(10);
    }

    return success;
}
/*********************************************************************************************************************/
void led_set_all(bool value)
{
    if (value)
    {
	PORTA |= 0x03;
	PORTB |= 0x18;
    }
    else
    {
	PORTA &= ~0x03;
	PORTB &= ~0x18;
    }
}
/*********************************************************************************************************************/
void led_set(uint index, bool value)
{
    if (value)
    {
	switch (index)
	{
	    case 0: PORTA |= 0x02; break;
	    case 1: PORTA |= 0x01; break;
	    case 2: PORTB |= 0x08; break;
	    case 3: PORTB |= 0x10; break;
	}
    }
    else
    {
	switch (index)
	{
	    case 0: PORTA &= ~0x02; break;
	    case 1: PORTA &= ~0x01; break;
	    case 2: PORTB &= ~0x08; break;
	    case 3: PORTB &= ~0x10; break;
	}
    }
}
/*********************************************************************************************************************/
void led_flash(uint8 count, bool wide)
{
    led_set(0, false);
    led_set(1, false);
    led_set(2, false);
    led_set(3, false);

    for (uint8 i = 0; i < count; ++i)
    {
	if (wide) led_set(0, true);
	          led_set(1, true);
		  led_set(2, true);
	if (wide) led_set(3, true);

	os_sleep(25);

	led_set(0, false);
	led_set(1, false);
	led_set(2, false);
	led_set(3, false);

	os_sleep(25);
    }
}
/*********************************************************************************************************************/
void led_signal(uint8 narrow, uint8 wide)
{
    led_flash(narrow, false);
    led_flash(wide,   true);
    os_sleep(200);
}
/*********************************************************************************************************************/
