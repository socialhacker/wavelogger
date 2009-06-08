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
#ifndef __log_log_html_h__
#define __log_log_html_h__

#include <stdio.h>

#include "libs/error/error.h"
#include "libs/sys/sys.h"
#include "libs/log/log_interface.h"

namespace Log
{
    class LogHTML : public LogInterface
    {
    private:
	uint		_level;
	const char	*_prefix;
	uint		_block_length;
	FILE		*_file;
	bool		_in_style;

	/*
	 * No copying of LogHTML objects
	 */
	LogHTML(const LogHTML &other);
	LogHTML &operator=(const LogHTML &other);

	void prefix(void);
	void block(const char *string, ...);
	void block(const char *string, va_list args);
	void color(Color color);
	void write(const char *string, ...);
	void write(const char *string, va_list args);

    public:
	LogHTML();
	virtual ~LogHTML();

	/*
	 * Implement the LogInterface.
	 */
	void va_notice (            const char *string, va_list args);
	void va_debug  (uint level, const char *string, va_list args);
	void va_warning(            const char *string, va_list args);
	void va_error  (            const char *string, va_list args);

	void set_level(uint level);
	uint get_level(void);
	void set_prefix(const char *prefix);

	/*
	 * These methods are specific to the LogConsole object.
	 */
	Err::Error open(const char *filename, bool overwrite);
	const char *get_prefix(void);
    };
}

#endif /*__log_log_html_h__*/
