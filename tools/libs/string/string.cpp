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
#include <stdlib.h>
#include <string.h>

#include "libs/error/error.h"
#include "libs/string/string.h"
#include "libs/string/contents.h"

using namespace Err;

const String	String::empty(&StringContents::empty);

/*********************************************************************************************************************/
String::String(void) :
    _contents(&StringContents::empty)
{
    _contents->add_reference();
}
/*********************************************************************************************************************/
String::String(char *string, bool manage, bool use_free, bool make_copy) :
    _contents(null)
{
    StringContents	*contents;

    if (make_copy)
    {
	uint	size = strlen(string);

	contents = new StringContents(size);
	contents->update(0, string, size);
    }
    else
    {
	contents = new StringContents(string, strlen(string), 0, manage, use_free);
    }

    _contents = contents;

    CheckExitStringB(_contents, "Failed to allocate space for new String");

    _contents->add_reference();
}
/*********************************************************************************************************************/
String::String(const char *string) :
    _contents(new StringContents(string, strlen(string), 0))
{
    CheckExitStringB(_contents, "Failed to allocate space for new String");

    _contents->add_reference();
}
/*********************************************************************************************************************/
String::String(const String &string):
    _contents(string._contents)
{
    _contents->add_reference();
}
/*********************************************************************************************************************/
String::String(const StringContents *contents):
    _contents(contents)
{
    _contents->add_reference();
}
/*********************************************************************************************************************/
String::~String(void)
{
    _contents->remove_reference();
}
/*********************************************************************************************************************/
String &String::operator=(const String &other)
{
    if (this != &other)
    {
	_contents->remove_reference();
	_contents = other._contents;
	_contents->add_reference();
    }

    return *this;
}
/*********************************************************************************************************************/
char String::operator[](unsigned int index)
{
    return _contents->_data[index];
}
/*********************************************************************************************************************/
String::operator const char *(void) const
{
    return _contents->_data;
}
/*********************************************************************************************************************/
const char *String::str(void) const
{
    return _contents->_data;
}
/*********************************************************************************************************************/
unsigned int String::length() const
{
    return _contents->_length;
}
/*********************************************************************************************************************/
char *String::copy(bool use_malloc) const
{
    char	*new_string;

    if (use_malloc) new_string = (char *) malloc(_contents->_length + 1);
    else	    new_string = new char[_contents->_length + 1];

    CheckExitStringB(new_string, "Failed to allocate space for copy of String");

    memcpy(new_string, _contents->_data, _contents->_length);

    new_string[_contents->_length] = '\0';

    return new_string;
}
/*********************************************************************************************************************/
String String::replace(unsigned int position, unsigned int count, const String string) const
{
    int			prefix   = position;
    int			padding  = position - length();
    int			addition = string.length();
    int			suffix   = length() - (position + count);
    unsigned int	index    = 0;
    unsigned int	total;
    StringContents	*contents;

    padding = padding > 0 ? padding : 0;
    suffix  = suffix  > 0 ? suffix  : 0;
    total   = prefix + padding + addition + suffix;

    if (total == 0)
	return String::empty;

    contents = new StringContents(prefix + padding + addition + suffix);

    CheckExitStringB(contents, "Failed to allocate space for new StringContents");

    if (prefix)   index = contents->update(index, _contents->_data, prefix);
    if (padding)  index = contents->pad(index, '\0', padding);
    if (addition) index = contents->update(index, string, addition);
    if (suffix)   index = contents->update(index, _contents->_data + position + addition, suffix);

    return String(contents);
}
/*********************************************************************************************************************/
String String::insert(unsigned int position, const String string) const
{
    return replace(position, 0, string);
}
/*********************************************************************************************************************/
String String::append(const String string) const
{
    return replace(length(), 0, string);
}
/*********************************************************************************************************************/
String String::append(char character) const
{
    unsigned int	index     = 0;
    StringContents	*contents = new StringContents(length() + 1);

    CheckExitStringB(contents, "Failed to allocate space for new StringContents");

    if (length())
	index = contents->update(index, _contents->_data, length());

    contents->pad(index, character, 1);

    return String(contents);
}
/*********************************************************************************************************************/
String String::erase(unsigned int position, unsigned int count) const
{
    return replace(position, count, String::empty);
}
/*********************************************************************************************************************/
int String::find(unsigned int position, const String string) const
{
    char	*result = strstr(*this, string);

    if (result)
	return (result - _contents->_data);

    return -1;
}
/*********************************************************************************************************************/
int String::rfind(unsigned int position, const String string) const
{
    return -1;
}
/*********************************************************************************************************************/
uint String::matches(char character) const
{
    uint	count = 0;

    for (uint i = 0; i < _contents->_length; ++i)
	if (_contents->_data[i] == character)
	    ++count;

    return count;
}
/*********************************************************************************************************************/
String String::substring(unsigned int position, unsigned int count) const
{
    if (position >= length())
	return String::empty;

    unsigned int	delta  = length() - position;
    unsigned int	length = count < delta ? count : delta;
    StringContents	*contents;

    if (length == 0)
	return String::empty;

    contents = new StringContents(length);

    CheckExitStringB(contents, "Failed to allocate space for new StringContents");

    contents->update(0, _contents->_data + position, count);

    return String(contents);
}
/*********************************************************************************************************************/
bool String::equal(const String string) const
{
    if (length() != string.length())
	return false;

    if (strncmp(*this, string, length()))
	return false;

    return true;
}
/*********************************************************************************************************************/
int String::order(const String one, const String two)
{
    int	min_length = one.length() < two.length() ? one.length() : two.length();

    return strncmp(one, two, min_length);
}
/*********************************************************************************************************************/
String String::format(const char *format_string, ...)
{
    String	result;

    va_list	args;

    va_start(args, format_string);
    result = va_format(format_string, args);
    va_end(args);

    return result;
}
/*********************************************************************************************************************/
String String::format(String format_string, ...)
{
    String	result;

    va_list	args;

    va_start(args, format_string);
    result = va_format((const char *) format_string, args);
    va_end(args);

    return result;
}
/*********************************************************************************************************************/
String String::va_format(const char *format_string, va_list args)
{
    char	*contents;

    if (vasprintf(&contents, format_string, args) < 0)
	return String::empty;
    else
	return String(contents, true, true);
}
/*********************************************************************************************************************/
String String::va_format(String format_string, va_list args)
{
    return va_format((const char *) format_string, args);
}
/*********************************************************************************************************************/
