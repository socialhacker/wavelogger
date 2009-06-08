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
#ifndef __command_line_command_line_h__
#define __command_line_command_line_h__

#include "libs/error/error.h"
#include "libs/string/string.h"
#include "libs/data/array.h"

namespace CommandLine
{
    /*******************************************************************************************************/
    class ValueBase
    {
    public:
	virtual ~ValueBase() {}

	virtual Err::Error read(int argc, const char **argv, int *index) = 0;
	virtual void usage() const = 0;
    };

    template <typename type> class Value : public ValueBase
    {
	type	_value;

    public:
	virtual Err::Error read(int argc, const char **argv, int *index);
	virtual void usage() const;

	void set(const type &value) { _value = value; }
	const type &get() const { return _value; }
    };

    /*******************************************************************************************************/
    class Argument
    {
    protected:
	bool		_set;
	String		_name;
	bool		_optional;
	String		_description;
	const ValueBase	&_value_base;

    public:
	Argument(String name, bool optional, String description, const ValueBase &value_base);
	virtual ~Argument();

	bool            set()      const;
	String          name()     const;
	bool            optional() const;
	const ValueBase &value()   const;

	virtual Err::Error read(int argc, const char **argv, int *index) = 0;

	virtual void usage(int prefix_length) const;
	virtual Err::Error apply_default();
    };

    template <typename type> class Scalar : public Argument
    {
	type		_default_value;
	Value<type>	_value;

    public:
	Scalar(String name, bool optional, type default_value, String description) :
	    Argument(name, optional, description, _value),
	    _default_value(default_value)
	{
	}

	virtual Err::Error read(int argc, const char **argv, int *index)
	{
	    *index += 1;

	    Check(_value.read(argc, argv, index));
	    _set = true;

	    return Err::success;
	}

	virtual Err::Error apply_default()
	{
	    _value.set(_default_value);
	    return Err::success;
	}

	const type &get() const
	{
	    return _value.get();
	}
    };

    template <typename type> class Array : public Argument
    {
	Value<type>		_value;
	Data::Array<type>	_value_array;

    public:
	Array(String name, bool optional, String description) :
	    Argument(name, optional, description, _value)
	{
	}

	virtual Err::Error read(int argc, const char **argv, int *index)
	{
	    *index += 1;

	    Check(_value.read(argc, argv, index));

	    _value_array.append(_value.get());
	    _set = true;

	    return Err::success;
	}

	const Data::Array<type> &get() const
	{
	    return _value_array;
	}
    };

    template <typename type> class Action : public Argument
    {
	typedef Err::Error (* Callback)(type value);

	Value<type>	_value;
	Callback	_callback;

    public:
	Action(String name, bool optional, Callback callback, String description) :
	    Argument(name, optional, description, _value),
	    _callback(callback)
	{
	}

	virtual Err::Error read(int argc, const char **argv, int *index)
	{
	    *index += 1;

	    Check(_value.read(argc, argv, index));
	    Check(_callback(_value.get()));

	    _set = true;

	    return Err::success;
	}
    };

    /*
     * Given a null terminated array of Argument pointers and the argc/argv from main read_command_line
     * will parse the command line and populate the values in the arguments array.
     */
    Err::Error parse(int argc, const char **argv, Argument **arguments);

    /*
     * Display simple usage information.
     */
    void usage(const char *program, Argument **arguments);
}

#endif //__command_line_command_line_h__
