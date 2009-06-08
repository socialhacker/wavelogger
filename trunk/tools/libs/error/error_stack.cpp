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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libs/error/error_stack.h"

using namespace Err;

ErrorStack *get_stack();

static uint			trace_indent = 0;

class ErrorStackChecker
{
public:
    ErrorStackChecker()
    {
    }

    ~ErrorStackChecker()
    {
	if (get_stack() != NULL)
	{
	    if (get_stack()->empty() == false)
	    {
		printf("The global ErrorStack is not empty.\n");

		get_stack()->simple_print();
	    }

	    delete get_stack();
	}
    }
};

static ErrorStackChecker	error_stack_checker;

/*********************************************************************************************************************/
static char *va_string(const char *string, va_list args)
{
    uint	size    = vsnprintf(NULL, 0, string, args);
    char	*result = new char[size + 1];

    if (result)
	vsnprintf(result, size + 1, string, args);

    return result;
}
/*********************************************************************************************************************/
static char *string_copy(const char *string)
{
    uint	size  = strlen(string) + 1;
    char	*copy = new char[size];

    if (copy)
	memcpy(copy, string, size);

    return copy;
}
/*********************************************************************************************************************/
ErrorStack *get_stack()
{
#ifdef __APPLE__
    static ErrorStack		*error_stack = NULL;
#else
    static __thread ErrorStack	*error_stack = NULL;
#endif

    if (error_stack == NULL)
	error_stack = new ErrorStack();

    return error_stack;
}
/*********************************************************************************************************************/
ErrorStack::ErrorStack() :
    _current(0),
    _enabled(true)
{
    for (uint i = 0; i < check_type_last; ++i)
	_will_print[i] = false;

    _will_print[check_type_check_exit]   = true;
    _will_print[check_type_check_assert] = true;
}
/*********************************************************************************************************************/
ErrorStack::~ErrorStack()
{
    if (_current != 0)
	print();
}
/*********************************************************************************************************************/
void ErrorStack::push(const Element &element)
{
    if (_current < 32)
    {
	_array[_current] = element;
	++_current;
    }
}
/*********************************************************************************************************************/
void ErrorStack::update(const Element &element, CheckType type)
{
    if (_enabled)
	push(element);

    if (_will_print[type])
    {
	if (_enabled)
	    print();
	else
	    element.print();
    }
}
/*********************************************************************************************************************/
void ErrorStack::simple_print()
{
    for (uint i = _current; i > 0; --i)
	_array[i - 1].print();
}
/*********************************************************************************************************************/
void ErrorStack::print()
{
    printf("STACK TRACE\n");
    print_trace();

    printf("ERROR STACK\n");
    simple_print();

    clear();
}
/*********************************************************************************************************************/
void ErrorStack::print_control(CheckType type, bool enabled)
{
    _will_print[type] = enabled;
}
/*********************************************************************************************************************/
void ErrorStack::clear()
{
    _current = 0;
}
/*********************************************************************************************************************/
bool ErrorStack::empty()
{
    return (_current == 0);
}
/*********************************************************************************************************************/
void ErrorStack::push_stack(Top &top)
{
    top._new_bottom = _current;
}
/*********************************************************************************************************************/
void ErrorStack::pop_stack(Top &top)
{
    _current = top._new_bottom;
}
/*********************************************************************************************************************/
ErrorStack::Element::Element(Error error,
			     const char *function,
			     const char *contents,
			     const char *file_name,
			     uint line_number,
			     const char *optional_string,
			     ...) :
    _type(element_type_sys),
    _error(error),
    _function(function),
    _contents(contents),
    _file_name(file_name),
    _line_number(line_number),
    _optional_string(NULL)
{
    va_list	args;

    if (optional_string)
    {
	va_start(args, optional_string);
	_optional_string = va_string(optional_string, args);
	va_end(args);
    }
}
/*********************************************************************************************************************/
ErrorStack::Element::Element(int error,
			     const char *function,
			     const char *contents,
			     const char *file_name,
			     uint line_number,
			     const char *optional_string,
			     ...) :
    _type(element_type_posix),
    _error(error == -1 ? errno : error),
    _function(function),
    _contents(contents),
    _file_name(file_name),
    _line_number(line_number),
    _optional_string(NULL)
{
    va_list	args;

    if (optional_string)
    {
	va_start(args, optional_string);
	_optional_string = va_string(optional_string, args);
	va_end(args);
    }
}
/*********************************************************************************************************************/
ErrorStack::Element::Element(bool error,
			     const char *function,
			     const char *contents,
			     const char *file_name,
			     uint line_number,
			     const char *optional_string,
			     ...) :
    _type(element_type_boolean),
    _error(error),
    _function(function),
    _contents(contents),
    _file_name(file_name),
    _line_number(line_number),
    _optional_string(NULL)
{
    va_list	args;

    if (optional_string)
    {
	va_start(args, optional_string);
	_optional_string = va_string(optional_string, args);
	va_end(args);
    }
}
/*********************************************************************************************************************/
ErrorStack::Element::Element() :
    _type(element_type_sys),
    _error(success),
    _function(""),
    _contents(""),
    _file_name(""),
    _line_number(0),
    _optional_string(NULL)
{
}
/*********************************************************************************************************************/
ErrorStack::Element::~Element()
{
    delete[] _optional_string;
}
/*********************************************************************************************************************/
ErrorStack::Element &ErrorStack::Element::operator=(const ErrorStack::Element &other)
{
    _type            = other._type;
    _error           = other._error;
    _function        = other._function;
    _contents        = other._contents;
    _file_name       = other._file_name;
    _line_number     = other._line_number;

    if (other._optional_string)
	_optional_string = string_copy(other._optional_string);
    else
	_optional_string = NULL;

    return *this;
}
/*********************************************************************************************************************/
void ErrorStack::Element::print() const
{
    const char	*error_as_string = "unknown error type";

    switch (_type)
    {
    case element_type_sys:
	error_as_string = error_string(Error(_error));
	break;

    case element_type_posix:
	error_as_string = strerror(_error);
	break;

    case element_type_boolean:
	error_as_string = _error ? "true" : "false";
	break;
    }

    printf("%s:%d in %s\n", _file_name, _line_number, _function);
    printf("  %s = %s (%d)\n", _contents, error_as_string, _error);

    if (_optional_string != NULL)
    {
	printf("  %p\n", _optional_string);
	printf("  %s\n", _optional_string);
    }

    printf("\n");
}
/*********************************************************************************************************************/
void ErrorStack::Element::simple_print() const
{
    const char	*error_as_string = "unknown error type";

    switch (_type)
    {
    case element_type_sys:
	error_as_string = error_string(Error(_error));
	break;

    case element_type_posix:
	error_as_string = strerror(_error);
	break;

    case element_type_boolean:
	error_as_string = _error ? "true" : "false";
	break;
    }

    printf("%s:%d in %s\n", _file_name, _line_number, _function);
    printf("  %s = %s (%d)\n", _contents, error_as_string, _error);

    if (_optional_string != NULL)
    {
	printf("  %p\n", _optional_string);
	printf("  %s\n", _optional_string);
    }

    printf("\n");
}
/*********************************************************************************************************************/
ErrorStack::Top::Top() :
    _new_bottom(0)
{
    get_stack()->push_stack(*this);
}
/*********************************************************************************************************************/
ErrorStack::Top::~Top()
{
    get_stack()->pop_stack(*this);
}
/*********************************************************************************************************************/
void Err::print_trace()
{
#if !defined(__APPLE__)

    void	*addresses[64];
    uint	length    = backtrace(addresses, 64);
    char	**strings = backtrace_symbols(addresses, length);

    for (int i = length - 1; i >= 0; --i)
	printf("%s\n", strings[i]);

    free(strings);

#else

    printf("Stack Trace unavailable on MacOS X.\n");

#endif
}
/*********************************************************************************************************************/
void Err::error_stack_update(const ErrorStack::Element &element, ErrorStack::CheckType type)
{
    get_stack()->update(element, type);
}
/*********************************************************************************************************************/
void Err::error_stack_print()
{
    get_stack()->print();
}
/*********************************************************************************************************************/
void Err::error_stack_print_control(ErrorStack::CheckType type, bool enabled)
{
    get_stack()->print_control(type, enabled);
}
/*********************************************************************************************************************/
void Err::error_stack_clear()
{
    get_stack()->clear();
}
/*********************************************************************************************************************/
void Err::error_stack_trace_before(const ErrorStack::Element &element, ErrorStack::CheckType type)
{
    printf("Trace before (%d)\n", trace_indent);
    element.simple_print();

    ++trace_indent;
}
/*********************************************************************************************************************/
void Err::error_stack_trace_after(const ErrorStack::Element &element, ErrorStack::CheckType type)
{
    --trace_indent;

    printf("Trace after (%d)\n", trace_indent);
    element.simple_print();
}
/*********************************************************************************************************************/
