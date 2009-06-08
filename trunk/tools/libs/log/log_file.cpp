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
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "libs/log/log_file.h"

using namespace Err;
using namespace Sys;
using namespace Log;

/*********************************************************************************************************************/
LogFile::LogFile() :
    _level(0),
    _prefix(null),
    _block_length(0),
    _file(null)
{
    char	*new_prefix = new char[1];

    new_prefix[0] = '\0';

    _prefix = new_prefix;
}
/*********************************************************************************************************************/
LogFile::~LogFile()
{
    delete[] _prefix;

    if (_file)
	::fclose(_file);
}
/*********************************************************************************************************************/
void LogFile::va_notice(const char *string, va_list args)
{
    prefix();
    block("Notice..: ");
    write(string, args);
    write("\n");
}
/*********************************************************************************************************************/
void LogFile::va_debug(uint level, const char *string, va_list args)
{
    if (level <= _level)
    {
	prefix();
	block("Debug...: %*s", level * 2, "");
	write(string, args);
	write("\n");
    }
}
/*********************************************************************************************************************/
void LogFile::va_warning(const char *string, va_list args)
{
    prefix();
    block("Warning.: ");
    write(string, args);
    write("\n");
}
/*********************************************************************************************************************/
void LogFile::va_error(const char *string, va_list args)
{
    prefix();
    block("Error...: ");
    write(string, args);
    write("\n");
}
/*********************************************************************************************************************/
Err::Error LogFile::open(const char *filename, bool overwrite)
{
    const char	*mode = "a";

    if (overwrite)
        mode = "w";

    CheckB(_file = ::fopen(filename, mode));

    return Err::success;
}
/*********************************************************************************************************************/
void LogFile::prefix(void)
{
    if (strlen(_prefix))
	_block_length = fprintf(_file, "%s ", _prefix);
    else
	_block_length = 0;
}
/*********************************************************************************************************************/
void LogFile::block(const char *string, ...)
{
    va_list	args;

    va_start(args, string);
    block(string, args);
    va_end(args);
}
/*********************************************************************************************************************/
void LogFile::block(const char *string, va_list args)
{
    _block_length += vfprintf(_file, string, args);
}
/*********************************************************************************************************************/
void LogFile::write(const char *string, ...)
{
    va_list	args;

    va_start(args, string);
    write(string, args);
    va_end(args);
}
/*********************************************************************************************************************/
void LogFile::write(const char *string, va_list args)
{
    char	*output;
    char	*end;

    vasprintf(&output, string, args);

    while ((end = index(output, '\n')) != null)
    {
	*end = '\0';
	fprintf(_file, "%s\n", output);
	output = end + 1;

	if (*output != '\0')
	    fprintf(_file, "%*s", _block_length, "");
    }

    fprintf(_file, "%s", output);
}
/*********************************************************************************************************************/
void LogFile::set_level(uint level)
{
    _level = level;
}
/*********************************************************************************************************************/
uint LogFile::get_level(void)
{
    return _level;
}
/*********************************************************************************************************************/
void LogFile::set_prefix(const char *prefix)
{
    char	*new_prefix = new char[strlen(prefix) + 1];

    strcpy(new_prefix, prefix);

    delete[] _prefix;

    _prefix = new_prefix;
}
/*********************************************************************************************************************/
const char *LogFile::get_prefix(void)
{
    return _prefix;
}
/*********************************************************************************************************************/
