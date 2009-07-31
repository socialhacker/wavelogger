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
#include <string.h>

#include "libs/command_line/command_line.h"
#include "libs/files/path.h"

using namespace Err;
using namespace CommandLine;

/*********************************************************************************************************************/
namespace CommandLine
{
    template <> Err::Error Value<bool>::read(int argc, const char **argv, int *index)
    {
	_value = true;

	return success;
    }

    template <> void Value<bool>::usage() const
    {
    }
    /*****************************************************************************************************************/
    template <> Err::Error Value<uint64>::read(int argc, const char **argv, int *index)
    {
	CheckB(*index < argc);

	_value = atoll(argv[*index]);

	*index += 1;

	return success;
    }

    template <> void Value<uint64>::usage() const
    {
	printf(" <unsigned int 64-bit>");
    }
    /*****************************************************************************************************************/
    template <> Err::Error Value<int>::read(int argc, const char **argv, int *index)
    {
	CheckB(*index < argc);

	_value = atoi(argv[*index]);

	*index += 1;

	return success;
    }

    template <> void Value<int>::usage() const
    {
	printf(" <int>");
    }
    /*****************************************************************************************************************/
    template <> Err::Error Value<String>::read(int argc, const char **argv, int *index)
    {
	CheckB(*index < argc);

	_value = argv[*index];

	*index += 1;

	return success;
    }

    template <> void Value<String>::usage() const
    {
	printf(" <text>");
    }
    /*****************************************************************************************************************/
    template <> Err::Error Value<float>::read(int argc, const char **argv, int *index)
    {
	CheckB(*index < argc);

	_value = atof(argv[*index]);

	*index += 1;

	return success;
    }

    template <> void Value<float>::usage() const
    {
	printf(" <float>");
    }
    /*****************************************************************************************************************/
    template <> Err::Error Value<Files::Path>::read(int argc, const char **argv, int *index)
    {
	CheckB(*index < argc);

	_value.set(argv[*index]);

	*index += 1;

	return success;
    }

    template <> void Value<Files::Path>::usage() const
    {
	printf(" <path>");
    }
}
/*********************************************************************************************************************/
Argument::Argument(String name, bool optional, String description, const ValueBase &value_base) :
    _set(false),
    _name(name),
    _optional(optional),
    _description(description),
    _value_base(value_base)
{
}
/*********************************************************************************************************************/
Argument::~Argument()
{
}
/*********************************************************************************************************************/
bool Argument::set() const
{
    return _set;
}
/*********************************************************************************************************************/
String Argument::name() const
{
    return _name;
}
/*********************************************************************************************************************/
bool Argument::optional() const
{
    return _optional;
}
/*********************************************************************************************************************/
const ValueBase &Argument::value() const
{
    return _value_base;
}
/*********************************************************************************************************************/
void Argument::usage(int prefix_length) const
{
    printf("%*s", prefix_length, "");
    if (_optional) printf("[");
    printf("-%s", _name.str());
    value().usage();
    if (_optional) printf("]");
    printf("\n");
}
/*********************************************************************************************************************/
Error Argument::apply_default()
{
    return success;
}
/*********************************************************************************************************************/
Error CommandLine::parse(int argc, const char **argv, Argument **arguments)
{
    /*
     * Parse the command line.
     */
    for (int i = 1; i < argc;)
    {
	int	j;

	CheckStringB(argv[i][0] == '-', "Expected an option, got \"%s\" instead.", argv[i]);

	for (j = 0; arguments[j] != null; ++j)
	    if (arguments[j]->name().equal(argv[i] + 1))
		break;

	if (arguments[j] != null)
	{
	    Check(arguments[j]->read(argc, argv, &i));
	}
	else
	{
	    printf("Unknown argument \"%s\".\n\n", argv[i]);
	    usage(argv[0], arguments);
	    Check(failure);
	}
    }

    /*
     * Check that all required arguments were passed.
     */
    for (int i = 0; arguments[i] != null; ++i)
    {
	if (arguments[i]->set() == false && arguments[i]->optional() == false)
	{
	    printf("Argument %s is required and was not set.\n\n", arguments[i]->name().str());
	    usage(argv[0], arguments);
	    Check(failure);
	}
    }

    /*
     * Apply default values to optional arguments.
     */
    for (int i = 0; arguments[i] != null; ++i)
    {
	if (arguments[i]->set() == false)
	    arguments[i]->apply_default();
    }

    return success;
}
/*********************************************************************************************************************/
void CommandLine::usage(const char *program, Argument **arguments)
{
    int		prefix_length = strlen(program) + 8;

    printf("Usage: %s ", program);

    for (int i = 0; arguments[i] != null; ++i)
	arguments[i]->usage(i == 0 ? 0 : prefix_length);
}
/*********************************************************************************************************************/
