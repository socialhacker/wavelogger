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
#include "libs/error/error.h"
#include "libs/command_line/command_line.h"

using namespace Err;

/**********************************************************************************************************************/
Error show_things(bool verbose)
{
    printf("Things... %d\n", verbose);
    return success;
}
/**********************************************************************************************************************/
namespace CommandLine
{
    static Scalar<bool>		add	("add",
					 true,
					 false,
					 "Add a new item to the todo list.");
    static Scalar<float>	timestep("timestep",
					 false,
					 0.1f,
					 "The amount of time between physics updates.");
    static Action<bool>		thing	("thing",
					 true,
					 show_things,
					 "Show the things...");
    static Array<int>		array   ("array",
					 true,
					 "Test the array capabilities.");

    static Argument	*arguments[] =
    {
	&add,
	&timestep,
	&thing,
	&array,
	null
    };
}
/**********************************************************************************************************************/
int main(int argc, const char **argv)
{
    Error	check_error = success;

    CheckCleanup(CommandLine::parse(argc, argv, CommandLine::arguments), failure);

    printf("add      %d\n", CommandLine::add.get());
    printf("timestep %f\n", CommandLine::timestep.get());

    {
	const Data::Array<int>		&ints = CommandLine::array.get();
	int				length = ints.length();

	for (int i = 0; i < length; ++i)
	    printf("array[%d] = %d\n", i, ints[i]);
    }

    return 0;

  failure:
    error_stack_print();
    return -1;
}
/**********************************************************************************************************************/
