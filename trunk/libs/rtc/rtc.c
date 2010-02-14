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
#include "libs/os/os.h"
#include "libs/twi/master.h"
#include "libs/rtc/rtc.h"

SetupError();

RTC	rtc;

/**************************************************************************************************/
Error rtc_init()
{
    uint8	buffer[4] = {0x0a, 0x00, 0x00, 0x00};
    TWIMessage	message   = {buffer, 0xd0, 4, 0};

    Check(twi_queue_message(&message));
    Check(twi_wait_message(&message, 50));

    return success;
}
/**************************************************************************************************/
Error rtc_read()
{
    volatile uint8	buffer[8] = {0x00};
    TWIMessage		message   = {buffer, 0xd0, 1, 8};

    Check(twi_queue_message(&message));
    Check(twi_wait_message(&message, 50));

    rtc.tick   = buffer[0];
    rtc.second = buffer[1];
    rtc.minute = buffer[2];
    rtc.hour   = buffer[3];

    rtc.date   = buffer[5];
    rtc.month  = buffer[6];
    rtc.year   = buffer[7];

    return success;
}
/**************************************************************************************************/
Error rtc_write()
{
    uint8	buffer[9] = {0x00,
			     rtc.tick,
			     rtc.second,
			     rtc.minute,
			     rtc.hour,
			     0x00,
			     rtc.date,
			     rtc.month,
			     rtc.year};
    TWIMessage	message   = {buffer, 0xd0, 9, 0};

    Check(twi_queue_message(&message));
    Check(twi_wait_message(&message, 50));

    return success;
}
/**************************************************************************************************/
Error rtc_reset_stack()
{
    uint8	buffer[6] = {0x08,
			     0x01,  // Watchdog Hundredths
			     0x00,  // Watchdog Seconds
			     0x00,  // Trickle Charge
			     0x00,  // Flags
			     0x03}; // Control
    TWIMessage	message   = {buffer, 0xd0, 6, 0};

    Check(twi_queue_message(&message));
    Check(twi_wait_message(&message, 50));

    /*
     * Sleep for half a second once the command has been issued to the Watchdog timer.  If we fail
     * to reset during this period then we return an error.
     */
    os_sleep(50);

    return failure;
}
/**************************************************************************************************/

static const uint16 days_month[2][13] =
{
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },	// Normal year
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }	// Leap year
};

/**************************************************************************************************/
static inline bool leapyear(uint8 year)
{
    return ((year & 3) == 0 && (year % 100 != 0 || ((year / 100) & 3) == 0));
}
/**************************************************************************************************/
static inline uint8 bcd_to_binary(uint8 bcd)
{
    return ((bcd >> 4) * 10 + (bcd & 0x0f));
}
/**************************************************************************************************/
uint32 rtc_ticks(const uint8 *data)
{
    uint8	year  = bcd_to_binary(data[7]);
    uint8	month = bcd_to_binary(data[6] & 0x1f) - 1;
    uint16	days  = days_month[leapyear(year)][month];
    uint8	hours = 0;
    uint32	ticks = 0;

    /*
     * Compute the hour of the day.  Take into account 12 vs. 24 hour time formats.
     */
    if (data[3] & 0x40)
	hours = bcd_to_binary(data[3] & ~0x60) - 1;
    else
	hours = bcd_to_binary(data[3]);

    if ((data[3] & 0x60) == 0x60)
	hours += 12;

    ticks = days + bcd_to_binary(data[5]);
    ticks = ticks *  24 + hours;
    ticks = ticks *  60 + bcd_to_binary(data[2]); // minutes
    ticks = ticks *  60 + bcd_to_binary(data[1]); // seconds
    ticks = ticks * 100 + bcd_to_binary(data[0]); // hundredths

    return ticks;
}
/**************************************************************************************************/
