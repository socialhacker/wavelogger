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
#include <avr/pgmspace.h>

#include "libs/stdio/console.h"
#include "libs/serial/serial.h"

/*********************************************************************************************************************/
void puts(const char *s)
{
    for (uint8 i = 0;; ++i)
    {
	uint8	byte = s[i];

	if (byte == '\0')
	    break;

	uart_write(byte);
    }
}
/*********************************************************************************************************************/
void puts_pgm(const prog_char *s)
{
    for (uint8 i = 0;; ++i)
    {
	uint8	byte = pgm_read_byte(s + i);

	if (byte == '\0')
	    break;

	uart_write(byte);
    }
}
/*********************************************************************************************************************/
void putsn_pgm(const prog_char *s, uint8 count)
{
    for (uint8 i = 0; i < count; ++i)
    {
	uint8	byte = pgm_read_byte(s + i);

	if (byte == '\0')
	    break;

	uart_write(byte);
    }
}
/*********************************************************************************************************************/
void putc(const char c)
{
    uart_write(c);
}
/*********************************************************************************************************************/
void read_line(char *line, uint8 length)
{
    uint8	index = 0;

    puts_pgm(PSTR("> "));

    while (1)
    {
	uint8	byte = uart_read();

	if ((byte == '\r') ||
	    (byte == '\n'))
	{
	    puts_pgm(PSTR("\r\n"));
	    break;
	}
	else if ((byte == 0x7f) ||
		 (byte == '\b'))
	{
	    if (index > 0)
	    {
		puts_pgm(PSTR("\b \b"));
		--index;
	    }
	}
	else if (byte < 0x20)
	{
	    continue;
	}
	else if (index < (length - 1))
	{
	    line[index] = (char)byte;
	    ++index;
	    putc(byte);
	}
	else
	{
	    putc('\a');
	}
    }

    line[index] = '\0';
}
/*********************************************************************************************************************/
