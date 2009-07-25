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
#include "libs/power/power.h"
#include "libs/stdio/write.h"
#include "libs/power/commands.h"

SetupError();

/*********************************************************************************************************************/
static Error battery_command(uint argc, const char **argv)
{
    uint16	battery;

    Check(power_read_battery(&battery));

    write(PSTR("Battery: %x\r\n"), battery);

    return success;
}
/*********************************************************************************************************************/
static Error temp_command(uint argc, const char **argv)
{
    uint16	temp;

    Check(power_read_temp(&temp));

    write(PSTR("Raw: %x\r\n"), temp);

    if (temp & 0x8000)
	write(PSTR("Temp: -%d\r\n"), (temp & ~0x8000) / 256);
    else
	write(PSTR("Temp: %d\r\n"), temp / 256);

    return success;
}
/*********************************************************************************************************************/
const ShellCommand shell_command_battery   PROGMEM = {"battery",   battery_command};
const ShellCommand shell_command_temp      PROGMEM = {"temp",      temp_command};
/*********************************************************************************************************************/
