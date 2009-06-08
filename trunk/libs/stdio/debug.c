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
#include "libs/stdio/debug.h"

static const char hex_table[16] PROGMEM = "0123456789abcdef";

static const uint32 places_table[] PROGMEM =
{
    1000000000,
    100000000,
    10000000,
    1000000,
    100000,
    10000,
    1000,
    100,
    10,
    1
};

/*********************************************************************************************************************/
void print_hexx(uint8 *value, uint8 count)
{
    for (int8 i = (count - 1); i >= 0; --i)
    {
	uint8	byte   = value[i];
	char	top    = pgm_read_byte(&(hex_table[(byte >> 4) & 0x0f]));
	char	bottom = pgm_read_byte(&(hex_table[(byte >> 0) & 0x0f]));

	putc(top);
	putc(bottom);
    }
}
/*********************************************************************************************************************/
void print_hex1(uint8 value)
{
    print_hexx(&value, 1);
}
/*********************************************************************************************************************/
void print_hex2(uint16 value)
{
    print_hexx((uint8 *)&value, 2);
}
/*********************************************************************************************************************/
void print_hex4(uint32 value)
{
    print_hexx((uint8 *)&value, 4);
}
/*********************************************************************************************************************/
void print_dec1(uint8 value)
{
    print_dec4(value);
}
/*********************************************************************************************************************/
void print_dec2(uint16 value)
{
    print_dec4(value);
}
/*********************************************************************************************************************/
void print_dec4(uint32 value)
{
    bool	zero = true;

    for (uint8 i = 0; i < (sizeof(places_table) / sizeof(places_table[0])); ++i)
    {
	uint32	place = pgm_read_dword(&(places_table[i]));
	char	digit;

	for (digit = '0'; value >= place; value -= place)
	    ++digit;

	if (digit != '0' || !zero)
	{
	    putc(digit);
	    zero = false;
	}
    }

    if (zero)
	putc('0');
}
/*********************************************************************************************************************/
void _debugx(const prog_char *name, uint8 *value, uint8 count)
{
    puts_pgm(name);

    if (count)
    {
	puts_pgm(PSTR(": 0x"));
	print_hexx(value, count);
    }

    puts_pgm(PSTR("\r\n"));
}
/*********************************************************************************************************************/
void _debug0(const prog_char *name)
{
    _debugx(name, 0, 0);
}
/*********************************************************************************************************************/
void _debug1(const prog_char *name, uint8 value)
{
    _debugx(name, &value, 1);
}
/*********************************************************************************************************************/
void _debug2(const prog_char *name, uint16 value)
{
    _debugx(name, (uint8 *)&value, 2);
}
/*********************************************************************************************************************/
void _debug4(const prog_char *name, uint32 value)
{
    _debugx(name, (uint8 *)&value, 4);
}
/*********************************************************************************************************************/
