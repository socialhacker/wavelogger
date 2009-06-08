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
#include "libs/log/log_interface.h"

/*********************************************************************************************************************/
void Log::LogInterface::notice(const char *string, ...)
{
    va_list	args;

    va_start(args, string);
    va_notice(string, args);
    va_end(args);
}
/*********************************************************************************************************************/
void Log::LogInterface::debug(uint level, const char *string, ...)
{
    va_list	args;

    va_start(args, string);
    va_debug(level, string, args);
    va_end(args);
}
/*********************************************************************************************************************/
void Log::LogInterface::warning(const char *string, ...)
{
    va_list	args;

    va_start(args, string);
    va_warning(string, args);
    va_end(args);
}
/*********************************************************************************************************************/
void Log::LogInterface::error(const char *string, ...)
{
    va_list	args;

    va_start(args, string);
    va_error(string, args);
    va_end(args);
}
/*********************************************************************************************************************/
