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
#if !defined(__APPLE__)
#include <execinfo.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "libs/error/error.h"
#include "libs/sys/module.h"
#include "libs/sys/error_module.h"

using namespace Err;
using namespace Sys;

namespace Sys
{
    class ErrorModule : public Module
    {
	static void print_trace();
	static void segfault_handler(int signal);

#if !defined(__APPLE__)
	sighandler_t	_previous;
#endif

    public:
	Err::Error initialize();
	Err::Error shutdown();

	const char *name() {return "Error";}
	const char **dependancies() {static const char *dep[] = {NULL}; return dep;}

	Err::Error call_previous(int signal)
        {
#if !defined(__APPLE__)
	    if (_previous)
	        _previous(signal);
#endif

	    return success;
	}
    };
}

static ErrorModule		error_module;
ModuleInitializer	error_module_initializer(&error_module);

/*********************************************************************************************************************/
Err::Error ErrorModule::initialize()
{
#if !defined(__APPLE__)
    sighandler_t	previous;

    previous  = signal(SIGSEGV, segfault_handler);
    _previous = 0;

    if (previous != segfault_handler)
	_previous = previous;
#endif

    return success;
}
/*********************************************************************************************************************/
Err::Error ErrorModule::shutdown()
{
    return success;
}
/*********************************************************************************************************************/
void ErrorModule::print_trace()
{
#if !defined(__APPLE__)
    void	*addresses[64];
    uint	length    = backtrace(addresses, 64);
    char	**strings = backtrace_symbols(addresses, length);

    for (int i = length - 1; i >= 0; --i)
	printf("  %s\n", strings[i]);

    free(strings);
#else
    printf("  Stack trace not supported on MacOS.\n");
#endif
}
/*********************************************************************************************************************/
void ErrorModule::segfault_handler(int signal)
{
    printf("Segfault caught\n"
	   "STACK TRACE\n");
    print_trace();
    printf("\n"
	   "Previous handler:\n");

    fflush(stdout);

    error_module.call_previous(signal);

    exit(1);
}
/*********************************************************************************************************************/
Err::Error Sys::register_segfault_signal_handler(void)
{
    return error_module.initialize();
}
/*********************************************************************************************************************/
