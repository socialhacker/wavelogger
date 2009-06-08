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
#ifndef __log_dispatch_h__
#define __log_dispatch_h__

#include <stdarg.h>

#include "libs/error/error.h"
#include "libs/sys/sys.h"
#include "libs/log/log_interface.h"

namespace Log
{
    class LogDispatch : public LogInterface
    {
    private:
	LogInterface	**_log_list;
	uint		_num_logs;

	/*
	 * No copying of LogDispatch objects
	 */
	LogDispatch(const LogDispatch &other);

    public:
	LogDispatch();
	virtual ~LogDispatch();

	/*
	 * Implement the Log interface.
	 */
	void va_notice (            const char *string, va_list args);
	void va_debug  (uint level, const char *string, va_list args);
	void va_warning(            const char *string, va_list args);
	void va_error  (            const char *string, va_list args);

	void set_level(uint level);
	uint get_level(void);
	void set_prefix(const char *prefix);

	/*
	 * These functions are specific to the LogDispatch object.
	 */
	Err::Error add_log(LogInterface &log);
    };
}

#endif /*__log_dispatch_h__*/
