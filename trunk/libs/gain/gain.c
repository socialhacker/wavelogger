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
#include "libs/gain/gain.h"
#include "libs/twi/master.h"

SetupError();

/**************************************************************************************************/
Error gain_init()
{
    return success;
}
/*********************************************************************************************************************/
const BootModule boot_module_gain PROGMEM =
{
    gain_init
};
/**************************************************************************************************/
Error gain_write(uint8 index, uint16 value)
{
    CheckB(index < 2);

    uint8	buffer[2] = {(index == 0 ? 0x00 : 0x80),
			     value & 0xff};
    TWIMessage	message   = {buffer, 0x5e, 2, 0};

    Check(twi_queue_message(&message));
    Check(twi_wait_message(&message, 50));

    return success;
}
/**************************************************************************************************/
Error gain_write_all(uint16 value)
{
    Check(gain_write(0, value));
    Check(gain_write(1, value));

    return success;
}
/**************************************************************************************************/
