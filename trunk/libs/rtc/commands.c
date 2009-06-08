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
