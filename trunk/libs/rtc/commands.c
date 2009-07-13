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
#include "libs/rtc/rtc.h"
#include "libs/rtc/commands.h"
#include "libs/stdio/write.h"

SetupError();

/*********************************************************************************************************************/
static uint8 binary_to_bcd(uint8 binary)
{
    if (binary > 99)
	return 99;

    return ((binary / 10) << 4) + (binary % 10);
}
/*********************************************************************************************************************/
Error rtc_command(uint argc, const char **argv)
{
    Check(rtc_read());

    write(PSTR("%hx/%hx/%hx %hx:%hx %hx.%hx\r\n"),
	  rtc.date,
	  rtc.month,
	  rtc.year,
	  rtc.hour,
	  rtc.minute,
	  rtc.second,
	  rtc.tick);

    return success;
}
/*********************************************************************************************************************/
Error rtc_set_date_command(uint argc, const char **argv)
{
    CheckB(argc == 4);

    uint8	date  = parse_number(argv[1]);
    uint8	month = parse_number(argv[2]);
    uint8	year  = parse_number(argv[3]);

    if (date  > 31) date  = 31;
    if (month > 12) month = 12;
    if (year  > 99) year  = 99;

    Check(rtc_read());
    rtc.date  = binary_to_bcd(date);
    rtc.month = binary_to_bcd(month);
    rtc.year  = binary_to_bcd(year);
    Check(rtc_write());

    return success;
}
/*********************************************************************************************************************/
Error rtc_set_time_command(uint argc, const char **argv)
{
    CheckB(argc == 4);

    uint8	hour   = parse_number(argv[1]);
    uint8	minute = parse_number(argv[2]);
    uint8	second = parse_number(argv[3]);

    if (hour   > 23) hour   = 23;
    if (minute > 59) minute = 59;
    if (second > 59) second = 59;

    Check(rtc_read());
    rtc.hour   = binary_to_bcd(hour);
    rtc.minute = binary_to_bcd(minute);
    rtc.second = binary_to_bcd(second);
    rtc.tick   = 0;
    Check(rtc_write());

    return success;
}
/*********************************************************************************************************************/
Error reset_command(uint argc, const char **argv)
{
    Check(rtc_reset_stack());

    /*
     * Obviously we should never get here.  If we do we failed to reset and the reset function
     * failed to notice.
     */
    return failure;
}
/*********************************************************************************************************************/
