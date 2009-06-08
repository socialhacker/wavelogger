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
#ifndef __types_h__
#define __types_h__

#include <avr/io.h>
#include <avr/pgmspace.h>

typedef int8_t		int8;
typedef int16_t		int16;
typedef int32_t		int32;
typedef int64_t		int64;

typedef unsigned int	uint;
typedef uint8_t		uint8;
typedef uint16_t	uint16;
typedef uint32_t	uint32;
typedef uint64_t	uint64;
typedef unsigned char   bool;

#define true	1
#define false	0

#define null	0

#define LENGTH(array)	(sizeof(array) / sizeof(array[0]))

#define STATIC_ASSERT_HELPER(expr, msg) \
    (!!sizeof(struct { unsigned int STATIC_ASSERTION__##msg: (expr) ? 1 : -1; }))
#define STATIC_ASSERT(expr, msg) \
    extern int (*assert_function__##msg(void)) [STATIC_ASSERT_HELPER(expr, msg)]

#endif //__types_h__
