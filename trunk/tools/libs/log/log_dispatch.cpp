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

#include "libs/log/log_dispatch.h"

using namespace Err;
using namespace Sys;
using namespace Log;

/*********************************************************************************************************************/
LogDispatch::LogDispatch() :
    _log_list(null),
    _num_logs(0)
{
}
/*********************************************************************************************************************/
LogDispatch::~LogDispatch()
{
    delete[] _log_list;
}
/*********************************************************************************************************************/
void LogDispatch::va_notice(const char *string, va_list args)
{
    for (uint i = 0; i < _num_logs; ++i)
	_log_list[i]->va_notice(string, args);
}
/*********************************************************************************************************************/
void LogDispatch::va_debug(uint level, const char *string, va_list args)
{
    for (uint i = 0; i < _num_logs; ++i)
	_log_list[i]->va_debug(level, string, args);
}
/*********************************************************************************************************************/
void LogDispatch::va_warning(const char *string, va_list args)
{
    for (uint i = 0; i < _num_logs; ++i)
	_log_list[i]->va_warning(string, args);
}
/*********************************************************************************************************************/
void LogDispatch::va_error(const char *string, va_list args)
{
    for (uint i = 0; i < _num_logs; ++i)
	_log_list[i]->va_error(string, args);
}
/*********************************************************************************************************************/
void LogDispatch::set_level(uint level)
{
    for (uint i = 0; i < _num_logs; ++i)
	_log_list[i]->set_level(level);
}
/*********************************************************************************************************************/
uint LogDispatch::get_level(void)
{
    uint	level = 0;

    for (uint i = 0; i < _num_logs; ++i)
	if (level < _log_list[i]->get_level())
	    level = _log_list[i]->get_level();

    return level;
}
/*********************************************************************************************************************/
void LogDispatch::set_prefix(const char *prefix)
{
    for (uint i = 0; i < _num_logs; ++i)
	_log_list[i]->set_prefix(prefix);
}
/*********************************************************************************************************************/
Err::Error LogDispatch::add_log(LogInterface &log)
{
    uint		new_length = _num_logs + 1;
    LogInterface	**new_list = new LogInterface *[new_length];

    memcpy(new_list, _log_list, _num_logs * sizeof(LogInterface *));

    new_list[_num_logs] = &log;

    _num_logs = new_length;
    _log_list = new_list;

    return success;
}
/*********************************************************************************************************************/
