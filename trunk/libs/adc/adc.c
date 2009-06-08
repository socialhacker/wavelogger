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
#include "libs/adc/adc.h"
#include "libs/twi/master.h"

SetupError();

/**************************************************************************************************/
Error adc_init()
{
    uint8	buffer[2] = {0x02, 0xf8};
    TWIMessage	message   = {buffer, 0x42, 2, 0};

    /*
     * Write configuration register.  Enable conversions on all channels.
     */
    Check(twi_queue_message(&message));
    Check(twi_wait_message(&message, 50));

    return success;
}
/*********************************************************************************************************************/
const BootModule boot_module_adc PROGMEM =
{
    adc_init
};
/**************************************************************************************************/
static const uint8 table[4] = {1, 3, 2, 0};

Error adc_read(uint8 channel_index, uint16 *value)
{
    CheckB(channel_index < 4);

    volatile uint8	buffer[2];
    uint8		index   = table[channel_index];
    TWIMessage		message = {buffer, 0x42, 1, 2};

    buffer[0] = 1 << (index + 4);

    Check(twi_queue_message(&message));
    Check(twi_wait_message(&message, 50));

    CheckB(((buffer[0] >> 4) & 0x03) == index);

    *value = (((buffer[0] & 0x0f) << 8) | buffer[1]) >> 2;

    return success;
}
/**************************************************************************************************/
Error adc_read_all(uint16 *value)
{
    volatile uint8	buffer[8];
    TWIMessage		message = {buffer, 0x42, 1, 8};

    buffer[0] = 0xf0;

    Check(twi_queue_message(&message));
    Check(twi_wait_message(&message, 50));

    for (uint8 i = 0; i < 4; ++i)
    {
	uint8	index = table[i];

	CheckB(((buffer[index * 2] >> 4) & 0x03) == index);

	value[i] = (((buffer[index * 2] & 0x0f) << 8) | buffer[(index * 2) + 1]) >> 2;
    }

    return success;
}
/**************************************************************************************************/

