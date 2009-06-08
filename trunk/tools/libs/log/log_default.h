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
#ifndef __log_log_default_h__
#define __log_log_default_h__

#include "libs/log/log_dispatch.h"

namespace Log
{
    /*
     * These non-method versions of the basic logging calls will call the default log.  The default log is created
     * at system initialization time and writes to the console.
     */
    void notice  (            const char *string, ...);
    void debug   (uint level, const char *string, ...);
    void warning (            const char *string, ...);
    void error   (            const char *string, ...);
    LogDispatch &log();
}

#endif //__log_log_default_h__
