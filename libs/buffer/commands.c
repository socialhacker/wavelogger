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
#include "libs/buffer/buffer.h"
#include "libs/buffer/commands.h"
#include "libs/shell/shell.h"
#include "libs/stdio/write.h"
#include "libs/stdio/console.h"
#include "libs/os/os.h"

SetupError();

/*********************************************************************************************************************/
Error read_bw_command(uint argc, const char **argv)
{
    CheckB(argc == 2);

    write(PSTR("Buffer read BW test\r\n"));

    uint8	count = parse_hex(argv[1]);
    uint32	begin = os_ticks();
    uint32	end;

    for (uint8 i = 0; i < count; ++i)
    {
	buffer_uint8(0);
	buffer_uint8(BYTES_PER_BLOCK);
    }

    end = os_ticks();

    write(PSTR("Elapsed Time: %ld.%hd seconds\r\n"),
	  (end - begin) / 100,
	  (uint8)((end - begin) % 100));

    return success;
}
/*********************************************************************************************************************/
Error dump_command(uint argc, const char **argv)
{
    CheckB(argc == 3);

    const uint		line_length = 16;
    const uint16	count_arg   = 512;
    uint32		address     = parse_hex(argv[2])              & ~(BYTES_PER_BLOCK - 1);
    uint16		count       = (count_arg + (line_length - 1)) & ~(line_length - 1);

    if (buffer_set_address(parse_hex(argv[1]), address >> 9)) return 1;

    for (uint16 i = 0; i < count; i += line_length)
    {
	write(PSTR("%lx: "), address);

	for (uint8 j = 0; j < line_length; ++j)
	{
	    write(PSTR("%hx"), buffer_uint8(i + j));

	    if (j & 1)
		putc(' ');
	}

	putc(' ');

	for (uint8 j = 0; j < line_length; ++j)
	{
	    uint8	value = buffer_uint8(i + j);

	    if (value >= 0x7f || value <= 0x1f)
		value = '.';

	    putc(value);
	}

	address += 16;

	write(PSTR("\r\n"));
    }

    return 0;
}
/*********************************************************************************************************************/
