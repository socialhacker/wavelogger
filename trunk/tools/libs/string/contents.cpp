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
#include <stdlib.h>
#include <string.h>

#include "libs/error/error.h"
#include "libs/string/contents.h"

using namespace Err;

enum StringContentsFlagBit
{
    string_contents_flag_bit_manage   = 0,
    string_contents_flag_bit_use_free = 1
};

enum StringContentsFlag
{
    string_contents_flag_manage   = (1 << string_contents_flag_bit_manage),
    string_contents_flag_use_free = (1 << string_contents_flag_bit_use_free)
};

const StringContents	StringContents::empty("", 0, 1);

/********************************************************************************************************************/
StringContents::StringContents(unsigned int length) :
    _references(0),
    _length(length),
    _flags(string_contents_flag_manage),
    _data(new char[_length + 1])
{
    CheckExitStringB(_data, "Failed to allocate space for StringContents");

    _data[_length] = '\0';
}
/********************************************************************************************************************/
StringContents::StringContents(const char *data,
			       unsigned int length,
			       unsigned int references) :
    _references(references),
    _length(length),
    _flags(0),
    _data(const_cast<char *>(data))
{
    if (_data == NULL)
	_length = 0;
}
/********************************************************************************************************************/
StringContents::StringContents(char *data,
			       unsigned int length,
			       unsigned int references,
			       bool manage,
			       bool use_free) :
    _references(references),
    _length(length),
    _flags((manage   << string_contents_flag_bit_manage) |
	   (use_free << string_contents_flag_bit_use_free)),
    _data(data)
{
    if (_data == NULL)
	_length = 0;
}
/********************************************************************************************************************/
StringContents::~StringContents(void)
{
    if ((_data != NULL) &&
	(string_contents_flag_manage & _flags))
    {
	if (string_contents_flag_use_free & _flags)
	    free((void *) _data);
	else
	    delete[] _data;
    }
}
/********************************************************************************************************************/
void StringContents::add_reference(void) const
{
    ++_references;
}
/********************************************************************************************************************/
void StringContents::remove_reference(void) const
{
    --_references;

    if (_references == 0)
	delete this;
}
/********************************************************************************************************************/
unsigned int StringContents::update(unsigned int position, const char *data, unsigned int count)
{
    if (position < _length)
    {
	unsigned int	delta     = _length - position;
	unsigned int	min_count = count < delta ? count : delta;

	if (min_count > 0)
	    memcpy(_data + position, data, min_count);
    }

    return position + count;
}
/********************************************************************************************************************/
unsigned int StringContents::pad(unsigned int position, char value, unsigned int count)
{
    if (position < _length)
    {
	unsigned int	delta     = _length - position;
	unsigned int	min_count = count < delta ? count : delta;

	if (min_count > 0)
	    memset(_data + position, value, min_count);
    }

    return position + count;
}
/********************************************************************************************************************/
