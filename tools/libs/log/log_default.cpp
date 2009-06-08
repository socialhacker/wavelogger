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
#include <stdarg.h>

#include "libs/error/error.h"
#include "libs/sys/module.h"

#include "libs/log/log_default.h"
#include "libs/log/log_dispatch.h"
#include "libs/log/log_console.h"

using namespace Err;

class LoggingModule : public Sys::Module
{
public:
    Log::LogDispatch	log_dispatch;
    Log::LogConsole	log_console;

    Err::Error initialize();
    Err::Error shutdown();

    const char *name() {return "Logging";}
    const char **dependancies() {static const char *dep[] = {null}; return dep;}
};

static LoggingModule	logging_module;
Sys::ModuleInitializer	logging_module_initializer(&logging_module);

/*********************************************************************************************************************/
Err::Error LoggingModule::initialize(void)
{
    Check(log_dispatch.add_log(log_console));

    return Err::success;
}
/*********************************************************************************************************************/
Err::Error LoggingModule::shutdown(void)
{
    return Err::success;
}
/*********************************************************************************************************************/
void Log::notice(const char *string, ...)
{
    va_list	args;

    va_start(args, string);
    logging_module.log_dispatch.va_notice(string, args);
    va_end(args);
}
/*********************************************************************************************************************/
void Log::debug(uint level, const char *string, ...)
{
    va_list	args;

    va_start(args, string);
    logging_module.log_dispatch.va_debug(level, string, args);
    va_end(args);
}
/*********************************************************************************************************************/
void Log::warning(const char *string, ...)
{
    va_list	args;

    va_start(args, string);
    logging_module.log_dispatch.va_warning(string, args);
    va_end(args);
}
/*********************************************************************************************************************/
void Log::error(const char *string, ...)
{
    va_list	args;

    va_start(args, string);
    logging_module.log_dispatch.va_error(string, args);
    va_end(args);
}
/*********************************************************************************************************************/
Log::LogDispatch &Log::log()
{
    return logging_module.log_dispatch;
}
/*********************************************************************************************************************/
