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
#include "libs/stdio/write.h"
#include "libs/twi/master.h"
#include "libs/power/power.h"

SetupError();

/**************************************************************************************************/
Error power_init()
{
    return success;
}
/**************************************************************************************************/
Error power_read_battery(uint16 *battery)
{
    uint8 volatile	buffer[4]    = {0, 0, 0, 0};
    uint16 volatile *	short_buffer = (uint16 volatile *)(buffer);
    TWIMessage		message      = {buffer, 0x12};

    message.write_count = 4;
    message.read_count  = 0;
    Check(twi_queue_message(&message));
    Check(twi_wait_message(&message, 50));

    message.write_count = 0;
    message.read_count  = 4;
    Check(twi_queue_message(&message));
    Check(twi_wait_message(&message, 50));

    *battery = short_buffer[1];

    return success;
}
/**************************************************************************************************/
Error power_read_temp(uint16 *temp)
{
    uint8 volatile	buffer[4]    = {0, 0, 0, 0};
    uint16 volatile *	short_buffer = (uint16 volatile *)(buffer);
    TWIMessage		message      = {buffer, 0x12};

    message.write_count = 4;
    message.read_count  = 0;
    Check(twi_queue_message(&message));
    Check(twi_wait_message(&message, 50));

    message.write_count = 0;
    message.read_count  = 4;
    Check(twi_queue_message(&message));
    Check(twi_wait_message(&message, 50));

    *temp = short_buffer[0];

    return success;
}
/**************************************************************************************************/
