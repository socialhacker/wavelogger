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
#ifndef __sys_globals_h__
#define __sys_globals_h__

#define null 0

typedef unsigned int		uint;
typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned		uint32;
typedef unsigned long long	uint64;

typedef signed char		int8;
typedef signed short		int16;
typedef signed			int32;
typedef signed long long	int64;

typedef float			real32;
typedef double			real64;

/*!
 * \brief access to a real32 as either a real32 or a uint32.
 *
 * It can be usefull to access the bits of a floating point number directly.  A union is used because
 * casting and dereferencing a pointer violates ANSII aliasing rules.
 */
union Real32Access
{
    /*!
     * This integer contains the same bits as the real32 in this union.  It is not the integer part of the
     * real32.  It is used to access the bits of the real32 directly.
     */
    uint32	integer;

    /*!
     * This is the real32 that can be accessed using the integer field.
     */
    real32	real;
};

/*!
 * \brief access to a real64 as either a real64 or an int32 and a uint32.
 *
 * It can be usefull to access the bits of a floating point number directly.  A union is used because
 * casting and dereferencing a pointer violates ANSII aliasing rules.
 */
union Real64Access
{
    struct
    {
	/*!
	 * This integer contains the top 32-bits as the real64 in this union.
	 */
	int32	high;

	/*!
	 * This integer contains the bottom 32-bits as the real64 in this union.
	 */
	uint32	low;
    };

    /*!
     * This is the real64 that can be accessed using the integer fields.
     */
    real64		real;
};

/*
 * Simple static assert based loosly on Boosts version.
 */
namespace StaticAssert
{
    template <bool> struct STATIC_ASSERTION_FAILURE;

    template <> struct STATIC_ASSERTION_FAILURE<true>
    {
    };

    template <int> struct StaticAssertTest
    {
    };
}

#define static_assert_join(a, b)	static_assert_join1(a, b)
#define static_assert_join1(a, b)	static_assert_join2(a, b)
#define static_assert_join2(a, b)	a##b

#define static_assert(expression, message)		\
    typedef ::StaticAssert::StaticAssertTest< sizeof(::StaticAssert::STATIC_ASSERTION_FAILURE< (bool)(expression) >)> \
    static_assert_join(_static_assert_, __LINE__)

/*
 * Template swap function.
 */
template<class type> void swap(type &a, type &b)
{
    type	temp(a);

    a = b;
    b = temp;
}

#endif //__sys_globlas_h__
