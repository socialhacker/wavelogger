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
#include "libs/shell/shell.h"
#include "libs/led/led.h"
#include "libs/led/commands.h"

SetupError();

/*********************************************************************************************************************/
Error led_command(uint argc, const char **argv)
{
    CheckB(argc == 3);

    uint8	index = parse_hex(argv[1]);
    uint8	value = parse_hex(argv[2]);

    if (index > 3)
	index = 3;

    led_set(index, value);

    return success;
}
/*********************************************************************************************************************/
