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
#ifndef __shell_commands_h__
#define __shell_commands_h__

#include "libs/types/types.h"
#include "libs/error/error.h"

Error time_command(uint argc, const char **argv);
Error stack_command(uint argc, const char **argv);
Error status_command(uint argc, const char **argv);

#endif //__shell_commands_h__
