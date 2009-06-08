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
#ifndef __log_log_interface_h__
#define __log_log_interface_h__

#include <stdarg.h>

#include "libs/sys/sys.h"

namespace Log
{
    class LogInterface
    {
    public:
	enum Color
	{
	    black,
	    red,
	    green,
	    yellow,
	    blue,
	    magenta,
	    cyan,
	    white,
	    normal
	};

	virtual void notice (            const char *string, ...);
	virtual void debug  (uint level, const char *string, ...);
	virtual void warning(            const char *string, ...);
	virtual void error  (            const char *string, ...);

	virtual void va_notice (            const char *string, va_list args) = 0;
	virtual void va_debug  (uint level, const char *string, va_list args) = 0;
	virtual void va_warning(            const char *string, va_list args) = 0;
	virtual void va_error  (            const char *string, va_list args) = 0;

	virtual void set_level(uint level)          = 0;
	virtual uint get_level(void)                = 0;
	virtual void set_prefix(const char *prefix) = 0;

	virtual ~LogInterface() {};
    };
}

#endif /*__log_log_interface_h__*/
