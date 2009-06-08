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

#include "libs/log/log_html.h"

using namespace Err;
using namespace Sys;
using namespace Log;

/*********************************************************************************************************************/
LogHTML::LogHTML() :
    _level(0),
    _prefix(null),
    _block_length(0),
    _file(null),
    _in_style(false)
{
    char	*new_prefix = new char[1];

    new_prefix[0] = '\0';

    _prefix = new_prefix;
}
/*********************************************************************************************************************/
LogHTML::~LogHTML()
{
    delete[] _prefix;

    if (_file)
    {
	if (_in_style)
	    fprintf(_file, "</font>");

	fprintf(_file,
		"  </pre>\n"
		"  </code>\n"
		"</html>\n");
	::fclose(_file);
    }
}
/*********************************************************************************************************************/
void LogHTML::va_notice(const char *string, va_list args)
{
    color(LogInterface::white);
    prefix();
    color(LogInterface::normal);
    block("Notice..: ");
    write(string, args);
    write("\n");
}
/*********************************************************************************************************************/
void LogHTML::va_debug(uint level, const char *string, va_list args)
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
void LogHTML::va_warning(const char *string, va_list args)
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
void LogHTML::va_error(const char *string, va_list args)
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
Err::Error LogHTML::open(const char *filename, bool overwrite)
{
    const char	*mode = "a";

    if (overwrite)
      mode = "w";

    CheckB(_file = ::fopen(filename, mode));

    fprintf(_file,
	    "<html>\n"
	    "  <title>\n"
	    "    %s\n"
	    "  </title>\n"
	    "  <body>\n"
	    "  <code>\n"
	    "  <pre>\n", filename);

    return Err::success;
}
/*********************************************************************************************************************/
void LogHTML::prefix(void)
{
    if (strlen(_prefix))
	_block_length = fprintf(_file, "%s ", _prefix);
    else
	_block_length = 0;
}
/*********************************************************************************************************************/
void LogHTML::block(const char *string, ...)
{
    va_list	args;

    va_start(args, string);
    block(string, args);
    va_end(args);
}
/*********************************************************************************************************************/
void LogHTML::block(const char *string, va_list args)
{
    _block_length += vfprintf(_file, string, args);
}
/*********************************************************************************************************************/
void LogHTML::color(Color color)
{
    if (_in_style)
	fprintf(_file, "</font>");

    _in_style = true;

    switch (color)
    {
    case red:     fprintf(_file, "<font color=\"red\">");     break;
    case green:   fprintf(_file, "<font color=\"green\">");   break;
    case yellow:  fprintf(_file, "<font color=\"yellow\">");  break;
    case blue:    fprintf(_file, "<font color=\"blue\">");    break;
    case magenta: fprintf(_file, "<font color=\"magenta\">"); break;
    case cyan:    fprintf(_file, "<font color=\"cyan\">");    break;
    case white:   fprintf(_file, "<font color=\"white\">");   break;
    case normal:  _in_style = false; break;
    case black:   _in_style = false; break;
    }
}
/*********************************************************************************************************************/
void LogHTML::write(const char *string, ...)
{
    va_list	args;

    va_start(args, string);
    write(string, args);
    va_end(args);
}
/*********************************************************************************************************************/
void LogHTML::write(const char *string, va_list args)
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
void LogHTML::set_level(uint level)
{
    _level = level;
}
/*********************************************************************************************************************/
uint LogHTML::get_level(void)
{
    return _level;
}
/*********************************************************************************************************************/
void LogHTML::set_prefix(const char *prefix)
{
    char	*new_prefix = new char[strlen(prefix) + 1];

    strcpy(new_prefix, prefix);

    delete[] _prefix;

    _prefix = new_prefix;
}
/*********************************************************************************************************************/
const char *LogHTML::get_prefix(void)
{
    return _prefix;
}
/*********************************************************************************************************************/
