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
#ifndef __sys_module__
#define __sys_module__

#include "globals.h"
#include "libs/error/error.h"

namespace Sys
{
    /*!
     * \brief A collection of code
     *
     * A module is a collection of code that can be initialized and shutdown using a simple interface.  Modules
     * have unique names and can depend on each other.  Thus modules must be initialized in the correct order.
     * This ordering is taken care of by the ModuleInitializer.
     */
    class Module
    {
    public:
	/*!
	 * Module destructor is empty.
	 *
	 * \todo Is this needed or can it be pure as well?
	 */
	virtual ~Module() {};

	/*!
	 * Initialize the module.  This will be called by the ModuleInitializer::initialize_modules function.  It
	 * is used to initialize global data and build whatever datastructures the module needs.
	 */
	virtual Err::Error initialize() = 0;

	/*!
	 * Shutdown the module.  This is currently not called, under unix all resources will go away when the
	 * program exits, but it might be nice to clean up after ourselvs anyway.
	 */
	virtual Err::Error shutdown() = 0;

	/*!
	 * The string returned is the name of the module.  Other modules will use this name to declare dependence.
	 */
	virtual const char *name() = 0;

	/*!
	 * Dependancies returns a NULL terminated array of strings.  Each string is the name of a module that this
	 * module depends on.
	 */
	virtual const char **dependancies() = 0;
    };

    /*!
     * \brief Collect and initialize modules
     *
     * ModuleInitializer decouples the module and its initialization.
     */
    class ModuleInitializer
    {
	/*
	 * The current state of the Module as understood by the ModuleInitializer.  This is used to track the
	 * initialization of modules and to prevent multiple initializations from occuring.
	 */
	enum ModuleState
	{
	    uninitialized,
	    initializing,
	    failed,
	    initialized
	};

	Module			*_module;
	Err::Error		_error;
	ModuleState		_state;
	ModuleInitializer	*_next;

	/*
	 *
	 */
	static ModuleInitializer	*_initializer_list;

	static Err::Error find_uninitialized_module(ModuleInitializer *&module_initializer);
	static Err::Error find_module(const char *name, ModuleInitializer *&module_initializer);
	static Err::Error initialize_module(ModuleInitializer *module_initializer);

	ModuleInitializer(const ModuleInitializer &other);
	ModuleInitializer &operator=(const ModuleInitializer &other);

    public:
	/*!
	 * Create a new ModuleInitializer that will place \a module in the table of Modules to initialize.
	 */
	ModuleInitializer(Module *module);

	/*!
	 * Destroy the ModuleInitializer.  This actualy has no effect at all.  The ModuleInitializer object is
	 * just used to place modules into the table of modules that will be initialized.
	 */
	~ModuleInitializer();

	/*!
	 * Print the state of all of the registered modules to the console.
	 */
	static Err::Error show_module_states(void);

	/*!
	 * Initialize all of the registered Modules.  Normally this function should not need to be called by the
	 * user because modules are initialized as they are registered (or once all of thier dependencies are
	 * registered).  But in some cases it is nice to have control (in main) before the Sys modules are
	 * initialized.  In these cases you can define MANUAL_MODULE_INITIALIZATION when compiling Sys and then
	 * call this function before making any Sys calls (this includes automatic variable constructors).
	 */
	static Err::Error initialize_modules(void);
    };
}

#endif //__sys_module__
