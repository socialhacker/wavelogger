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
#include <ctype.h>

#include "libs/os/os.h"
#include "libs/stdio/console.h"
#include "libs/stdio/write.h"
#include "libs/error/error.h"
#include "libs/shell/shell.h"

/*********************************************************************************************************************/
uint32 parse_hex(const char *string)
{
    uint32	result = 0;

    for (uint8 i = 0; string[i] != '\0'; ++i)
    {
	uint8	byte = string[i];

	if (byte == ' ')
	    continue;

	if (byte >= '0' && byte <= '9')
	{
	    result <<= 4;
	    result |= byte - '0';
	    continue;
	}

	if (byte >= 'a' && byte <= 'f')
	{
	    result <<= 4;
	    result |= byte - 'a' + 10;
	    continue;
	}

	if (byte >= 'A' && byte <= 'F')
	{
	    result <<= 4;
	    result |= byte - 'A' + 10;
	    continue;
	}

	break;
    }

    return result;
}
/*********************************************************************************************************************/
uint32 parse_dec(const char *string)
{
    uint32	result = 0;

    for (uint8 i = 0; string[i] != '\0'; ++i)
    {
	uint8	byte = string[i];

	if (byte == ' ')
	    continue;

	if (byte >= '0' && byte <= '9')
	{
	    result *= 10;
	    result += byte - '0';
	    continue;
	}

	break;
    }

    return result;
}
/*********************************************************************************************************************/
uint32 parse_oct(const char *string)
{
    uint32	result = 0;

    for (uint8 i = 0; string[i] != '\0'; ++i)
    {
	uint8	byte = string[i];

	if (byte == ' ')
	    continue;

	if (byte >= '0' && byte <= '7')
	{
	    result <<= 3;
	    result |= byte - '0';
	    continue;
	}

	break;
    }

    return result;
}
/*********************************************************************************************************************/
uint32 parse_number(const char *string)
{
    if (string[0] == '0' &&
	string[1] == 'x')
	return parse_hex(string + 2);

    return parse_dec(string);
}
/*********************************************************************************************************************/
void error_stack_print()
{
    uint8	count = error_stack_count();

    if (count >= ERROR_STACK_MAX_ENTRIES)
    {
	puts_pgm(PSTR("ErrorStack overflow detected.\r\n"));
	count = ERROR_STACK_MAX_ENTRIES;
    }

    if (count)
    {
	write(PSTR("\r\nErrorStack:\r\n"));
    }

    for (uint8 i = count; i > 0; --i)
    {
	ErrorEntry	*entry = error_stack_entry(i - 1);

	write(PSTR("%s:%d:%hd\r\n"), entry->filename, entry->line_number, entry->error);
    }

    error_stack_clear();
}
/*********************************************************************************************************************/
static void shell_usage(const ShellCommand *shell_command_table[], uint shell_command_count)
{
    puts_pgm(PSTR("Valid commands are:\r\n"));

    for (uint i = 0; i < shell_command_count; ++i)
    {
	const ShellCommand	*command  = (const ShellCommand *) pgm_read_word(shell_command_table + i);

	write(PSTR("    %s\r\n"), command->name);
    }
}
/*********************************************************************************************************************/
static void parse_line(char *line, uint line_length, uint *argc, const char **argv, uint argv_length)
{
    uint	write_index = 0;
    uint	index       = 0;
    bool	quoted      = false;
    bool	scanning    = true;

    for (uint read_index = 0; line[read_index] != '\0'; ++read_index)
    {
	if (scanning)
	{
	    /*
	     * Record this argument into the argv array.
	     */
	    if (index < argv_length)
	    {
		argv[index] = line + write_index;
		++index;
	    }

	    scanning = false;

	    /*
	     * Scan to the beginning of the next argument of the end of the string.
	     */
	    while (isspace(line[read_index]) &&
		   line[read_index] != '\0')
		++read_index;

	    if (line[read_index] == '\0')
		break;
	}

	if (quoted)
	{
	    switch (line[read_index])
	    {
		case '"':
		    quoted = false;
		    break;

		default:
		    line[write_index++] = line[read_index];
		    break;
	    }
	}
	else
	{
	    switch (line[read_index])
	    {
		case ' ':
		case '\r':
		case '\n':
		case '\t':
		    line[write_index++] = '\0';
		    scanning = true;
		    break;

		case '"':
		    quoted = true;
		    break;

		default:
		    line[write_index++] = line[read_index];
		    break;
	    }
	}
    }

    line[write_index] = '\0';

    *argc = index;
}
/*********************************************************************************************************************/
static bool match(const char *one, const prog_char *two)
{
    for (uint i = 0;; ++i)
    {
	uint8	byte_one = one[i];
	uint8	byte_two = pgm_read_byte(two + i);

	if (byte_one != byte_two)
	    return false;

	if (byte_one == '\0')
	    return true;
    }
}
/*********************************************************************************************************************/
void shell_run(const ShellCommand *shell_command_table[], uint shell_command_count)
{
    while (1)
    {
	uint			i;
	char			line[32];
	uint			argc = 0;
	const char		*argv[4];
	ShellCommandFunction	function;

	read_line(line, 32);
	parse_line(line, 32, &argc, argv, 4);

	if (argc == 0)
	    continue;

	for (i = 0; i < shell_command_count; ++i)
	{
	    const ShellCommand	*command;

	    command  = (const ShellCommand *) pgm_read_word(shell_command_table + i);
	    function = (ShellCommandFunction) pgm_read_word(&(command->function));

	    if (match(argv[0], command->name)) break;
	}

	if (i >= shell_command_count)
	{
	    write(PSTR("Command not found: %S\r\n"), argv[0]);
	    shell_usage(shell_command_table, shell_command_count);
	    continue;
	}

	{
	    Error	error = function(argc, argv);

	    if (error != success)
		write(PSTR("Command failed (%d).\r\n"), error);

	    error_stack_print();
	}
    }
}
/*********************************************************************************************************************/
