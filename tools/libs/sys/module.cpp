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
#include <stdio.h>
#include <string.h>

#include "libs/error/error.h"
#include "libs/sys/module.h"

using namespace Err;
using namespace Sys;

ModuleInitializer *ModuleInitializer::_initializer_list = null;

/*********************************************************************************************************************/
ModuleInitializer::ModuleInitializer(Module *module) :
    _module(module),
    _error(success),
    _state(uninitialized),
    _next(_initializer_list)
{
    _initializer_list = this;

#ifndef MANUAL_MODULE_INITIALIZATION
    initialize_modules();
#endif
}
/*********************************************************************************************************************/
ModuleInitializer::~ModuleInitializer()
{
    // FIX FIX FIX: This should take into account the dependancies.

    _module->shutdown();
}
/*********************************************************************************************************************/
Error ModuleInitializer::find_uninitialized_module(ModuleInitializer *&module_initializer)
{
    for (ModuleInitializer *current = _initializer_list; current != null; current = current->_next)
    {
	if (current->_state == uninitialized)
	{
	    module_initializer = current;
	    return success;
	}
    }

    return failure;
}
/*********************************************************************************************************************/
Error ModuleInitializer::find_module(const char *name, ModuleInitializer *&module_initializer)
{
    for (ModuleInitializer *current = _initializer_list; current != null; current = current->_next)
    {
	if (strcmp(current->_module->name(), name) == 0)
	{
	    module_initializer = current;
	    return success;
	}
    }

    return failure;
}
/*********************************************************************************************************************/
Error ModuleInitializer::initialize_module(ModuleInitializer *module_initializer)
{
    const char	**dep = module_initializer->_module->dependancies();

    module_initializer->_state = initializing;

    for (uint i = 0; dep[i] != null; ++i)
    {
	ModuleInitializer	*parent;

	if (find_module(dep[i], parent) != success)
	{
	    module_initializer->_error = module_initialization_delayed;
	    return module_initialization_delayed;
	}

	/*
	 * If the module we depend on is currently beeing initialized that means there is a dependency cycle
	 * and we can't go any further.  Otherwise, if its already initialized we move on to the next one.
	 * and finaly, if its not currently beeing initialized or hasn't already been initialized we initialize
	 * it.
	 */
	switch (parent->_state)
	{
	case initializing:
	    module_initializer->_error = failure;
	    return failure;

	case initialized:
	    break;

	default:
	    if (initialize_module(parent) != success) return failure;
	    break;
	}
    }

    module_initializer->_error = module_initializer->_module->initialize();

    if (module_initializer->_error == success)
	module_initializer->_state = initialized;
    else
	module_initializer->_state = failed;

    return success;
}
/*********************************************************************************************************************/
Error ModuleInitializer::show_module_states(void)
{
    for (ModuleInitializer *current = _initializer_list; current != null; current = current->_next)
    {
        const char	*state_string = "";
	const char	*name         = current->_module->name();
	uint		length        = strlen(name);

	switch (current->_state)
	{
	    case uninitialized: state_string = "uninitialized"; break;
	    case initializing:  state_string = "initializing "; break;
	    case failed:        state_string = "failed       "; break;
	    case initialized:   state_string = "initialized  "; break;
	}

	printf("module: %s%.*s: %s (%s)\n",
	       name,
	       30 - length,
	       "..............................",
	       state_string,
	       error_string(current->_error));
    }

    return success;
}
/*********************************************************************************************************************/
Error ModuleInitializer::initialize_modules(void)
{
    for (ModuleInitializer *current = _initializer_list; current != null; current = current->_next)
	if (current->_state != initialized)
	    initialize_module(current);

    return success;
}
/*********************************************************************************************************************/
