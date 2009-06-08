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
#include "libs/types/types.h"
#include "libs/stdio/debug.h"
#include "libs/stdio/console.h"
#include "libs/stdio/write.h"

/*********************************************************************************************************************/
void write(const prog_char *format, ...)
{
    va_list	args;
    uint8	byte   = pgm_read_byte(format);
    bool	escape = false;
    uint8	width  = 2;

    va_start(args, format);

    for (uint8 i = 1; byte != '\0'; ++i)
    {
	if (escape)
	{
	    switch (byte)
	    {
		case 'h':
		    width = 1;
		    break;

		case 'l':
		    width = 4;
		    break;

		case 'c':
		    putc(va_arg(args, uint16));
		    escape = false;
		    break;

		case 'x':
		    if (width == 1) print_hex1(va_arg(args, uint16));
		    if (width == 2) print_hex2(va_arg(args, uint16));
		    if (width == 4) print_hex4(va_arg(args, uint32));
		    escape = false;
		    break;

		case 'd':
		    if (width == 1) print_dec1(va_arg(args, uint16));
		    if (width == 2) print_dec2(va_arg(args, uint16));
		    if (width == 4) print_dec4(va_arg(args, uint32));
		    escape = false;
		    break;

		case 's':
		    puts_pgm(va_arg(args, prog_char *));
		    escape = false;
		    break;

		case 'S':
		    puts(va_arg(args, const char *));
		    escape = false;
		    break;

		case '%':
		    putc(byte);
		    escape = false;
		    break;
	    }
	}
	else
	{
	    switch (byte)
	    {
		case '%':
		    escape = true;
		    width  = 2;
		    break;

		default:
		    putc(byte);
		    break;
	    }
	}

	byte = pgm_read_byte(format + i);
    }

    va_end(args);
}
/*********************************************************************************************************************/
