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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libs/log/log_console.h"

using namespace Log;

/*********************************************************************************************************************/
LogConsole::LogConsole() :
    _level(0),
    _prefix(null),
    _block_length(0)
{
    char	*new_prefix = new char[1];

    new_prefix[0] = '\0';

    _prefix = new_prefix;
}
/*********************************************************************************************************************/
LogConsole::~LogConsole()
{
    delete[] _prefix;
}
/*********************************************************************************************************************/
void LogConsole::va_notice(const char *string, va_list args)
{
    color(LogInterface::white);
    prefix();
    color(LogInterface::normal);
    block("Notice..: ");
    write(string, args);
    write("\n");
}
/*********************************************************************************************************************/
void LogConsole::va_debug(uint level, const char *string, va_list args)
{
    if (level <= _level)
    {
	color(LogInterface::white);
	prefix();
	color(LogInterface::blue);
	block("Debug...: %*s", level * 2, "");
	color(LogInterface::normal);
	write(string, args);
	write("\n");
    }
}
/*********************************************************************************************************************/
void LogConsole::va_warning(const char *string, va_list args)
{
    color(LogInterface::white);
    prefix();
    color(LogInterface::yellow);
    block("Warning.: ");
    color(LogInterface::normal);
    write(string, args);
    write("\n");
}
/*********************************************************************************************************************/
void LogConsole::va_error(const char *string, va_list args)
{
    color(LogInterface::white);
    prefix();
    color(LogInterface::red);
    block("Error...: ");
    color(LogInterface::normal);
    write(string, args);
    write("\n");
}
/*********************************************************************************************************************/
void LogConsole::prefix(void)
{
    _block_length = printf("%s ", _prefix);
}
/*********************************************************************************************************************/
void LogConsole::block(const char *string, ...)
{
    va_list	args;

    va_start(args, string);
    block(string, args);
    va_end(args);
}
/*********************************************************************************************************************/
void LogConsole::block(const char *string, va_list args)
{
    _block_length += vprintf(string, args);
}
/*********************************************************************************************************************/
void LogConsole::color(Color color)
{
    switch (color)
    {
    case black:   printf("%c[1;30m", 27); break;
    case red:     printf("%c[1;31m", 27); break;
    case green:   printf("%c[1;32m", 27); break;
    case yellow:  printf("%c[1;33m", 27); break;
    case blue:    printf("%c[1;34m", 27); break;
    case magenta: printf("%c[1;35m", 27); break;
    case cyan:    printf("%c[1;36m", 27); break;
    case white:   printf("%c[1;37m", 27); break;
    case normal:  printf("%c[0m", 27); break;
    }
}
/*********************************************************************************************************************/
void LogConsole::write(const char *string, ...)
{
    va_list	args;

    va_start(args, string);
    write(string, args);
    va_end(args);
}
/*********************************************************************************************************************/
void LogConsole::write(const char *string, va_list args)
{
    char	*allocated;
    char	*output;
    char	*end;

    vasprintf(&allocated, string, args);

    output = allocated;

    while ((end = index(output, '\n')) != null)
    {
	*end = '\0';
	printf("%s\n", output);
	output = end + 1;

	if (*output != '\0')
	    printf("%*s", _block_length, "");
    }

    printf("%s", output);
    free(allocated);
}
/*********************************************************************************************************************/
void LogConsole::set_level(uint level)
{
    _level = level;
}
/*********************************************************************************************************************/
uint LogConsole::get_level(void)
{
    return _level;
}
/*********************************************************************************************************************/
void LogConsole::set_prefix(const char *prefix)
{
    char	*new_prefix = new char[strlen(prefix) + 1];

    strcpy(new_prefix, prefix);

    delete[] _prefix;

    _prefix = new_prefix;
}
/*********************************************************************************************************************/
const char *LogConsole::get_prefix(void)
{
    return _prefix;
}
/*********************************************************************************************************************/
