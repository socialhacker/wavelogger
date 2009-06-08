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
#ifndef __string_string_h__
#define __string_string_h__

#include <stdarg.h>
#include "globals.h"

/*
 * /brief Imutable String class.
 */
class String
{
    const class StringContents	*_contents;

public:
    /*!
     * /brief Construct an empty string.
     *
     * This constructs an empty string.  Its length will be zero.
     */
    String(void);

    /*!
     * /brief Construct a string from a C style string.
     *
     * The new String will contain a copy of /a string.  It's length will be the number of characters, not
     * including the NULL termination character.
     *
     * If manage    is true then the C style string will be deleted when the last reference to it is removed.
     * If use_free  is true then free is used to free the string instead of delete.
     * If make_copy is true then a copy of the C style string is made and stored.
     */
    String(char *string,
	   bool manage    = false,
	   bool use_free  = false,
	   bool make_copy = false);

    /*!
     * /brief Construct a string from a C style string.
     *
     * The new String will reference \a string.  No attempt to manage it will be made.
     */
    String(const char *string);

    /*!
     * /brief Construct a string from a String.
     *
     * The new string will point to the same Contents as /a string.  And the Contents pointed to by /a string
     * will have its reference count increased.
     */
    String(const String &string);

    /*!
     * /brief Construct a string from a Contents object.
     *
     * The new string will point to /a contents.  And /a contents will have its reference count increased.
     */
    String(const StringContents *contents);

    /*!
     * /brief Destruct a string.
     *
     * The Contents will only be destroyed if this String was its last referencer.
     */
    ~String(void);

    /*!
     * /brief Asign one string to another.
     */
    String &operator=(const String &source);

    /*!
     * /brief Query the String for a character.  Do not check the bounds of the string when accessing.
     */
    char operator[](unsigned int index);

    /*!
     * /brief Cast a String into a const char *.
     *
     * This just returns the internal representation of the string.  A copy is not made.  So this char * is
     * only valid as long as the String object itself is valid.  If you need a copy of the String use copy.
     */
    operator const char *(void) const;

    /*!
     * /brief Return a const char * that points to C version of the String.
     *
     * This just returns the internal representation of the string.  A copy is not made.  So this char * is
     * only valid as long as the String object itself is valid.  If you need a copy of the String use copy.
     */
     const char *str(void) const;

    /*!
     * /brief return the length of the string.
     *
     * The length of the string is returned.  This is the number of characters in the string.
     */
    unsigned int length(void) const;

    /*!
     * /brief return a newly allocated c style string.
     *
     * copy allocates and returns a c style copy of the contents of this string.  This means that any
     * string generated in this manner must be deleted by the caller.  If use_malloc is true then the
     * copy will be allocated using malloc.  Otherwise new will be used.
     */
    char *copy(bool use_malloc = false) const;

    /*!
     * /brief Return a string with a section replaced with /a string.
     *
     * Replace creates a new string.  It will hold the contents of the String, but the segment from
     * /a position to /a position + /a count will be replaced with /a string.  If /a string is not /a count
     * characters long then the second part of the String will be moved to fill in the gap or make room.
     */
    String replace(unsigned int position, unsigned int count, const String string) const;

    /*!
     * /brief Return a string with /a string inserted into the String at /a position.
     *
     * Insert /a string into the String at /a position.  Room is made by pushing any characters after
     * /a position out.  Return the new String.
     *
     * Insert is just syntactic sugar for s.replace(position, 0, string).
     */
    String insert(unsigned int position, const String string) const;

    /*!
     * /brief Return a string with /a string appended to the end of String.
     *
     * Append is just syntactic sugar for s.replace(s.length(), 0, string).
     */
    String append(const String string) const;

    /*!
     * /brief Return a string with /a character appended to the end of String.
     *
     * Append is just syntactic sugar for s.replace(s.length(), 0, character).  Or it would be if there was a char
     * version of replace.
     */
    String append(char character) const;

    /*!
     * /brief Return a string with /a count characters erased from String.
     *
     * Erase /a count characters from the String starting at /a position.  The gap is taken up by moving
     * any characters past the region erased, back into it.
     *
     * Erase is just syntactic sugar for s.replace(position, count, String::empty).
     */
    String erase(unsigned int position, unsigned int count) const;

    /*!
     * /brief Find /a string in the String.
     *
     * Find the first occurence of /a string in the String starting at /a position.  The position of the first
     * occurence is returned.  If /a string is not found in the String then -1 is returned.
     */
    int find(unsigned int position, const String string) const;

    /*!
     * /brief Find /a string in the String.
     *
     * Find the first occurence of /a string in the String starting at /a position and working backwars through
     * the string.  The contents of /a string are not looked for in reverse order in the String.  The starting
     * position is moved towards the beginning of the String from each compare.  The position of the first
     * occurence is returned.  If /a string is not found in the String then -1 is returned.
     */
    int rfind(unsigned int position, const String string) const;

    /*!
     * /brief Count the number of times /a character occures in /a string.
     *
     * Find all occurances of /a character in /a string and return the number of times the character was
     * found.
     */
    uint matches(char character) const;

    /*!
     * /brief Get a substring of the String.
     *
     * Substring gets the substring of String that overlaps with the region starting at /a position and
     * ending at /a position + /a count.  This might be the empty string if /a position is past the end of
     * the String.
     */
    String substring(unsigned int position, unsigned int count) const;

    /*!
     * /brief Compare two string for equality.
     *
     * If /a string is equal in length and contents to the String then true is returned, otherwise
     * false is returned.
     */
    bool equal(const String string) const;

    /*!
     * /brief Compare two string for order.
     *
     * Return one, zero or negative one, depending on whether /a one is greater than, equal to or
     * less than /a two.
     */
    static int order(const String one, const String two);

    /*!
     * /brief Create a new formatted string.
     *
     * Return a new string that is created using the printf formatting rules.  The format string can be a C style
     * const char * or a String object.  There are va_format versions of the format function as well.  These accept
     * va_list args instead of a variable number of arguments.
     */
    static String format(const char *format, ...);
    static String format(String      format, ...);

    static String va_format(const char *format, va_list args);
    static String va_format(String      format, va_list args);

    static const String	empty;
};

#endif //__string_string_h__
