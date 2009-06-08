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
#ifndef __debug_h__
#define __debug_h__

#include <avr/pgmspace.h>

#include "libs/types/types.h"

void _debugx(const prog_char *name, uint8 *value, uint8 count);
void _debug0(const prog_char *name);
void _debug1(const prog_char *name, uint8 value);
void _debug2(const prog_char *name, uint16 value);
void _debug4(const prog_char *name, uint32 value);

void print_hexx(uint8 *value, uint8 count);
void print_hex1(uint8 value);
void print_hex2(uint16 value);
void print_hex4(uint32 value);

void print_dec1(uint8 value);
void print_dec2(uint16 value);
void print_dec4(uint32 value);

#ifdef DEBUG
    #define debugx(name, value, count)	_debugx(PSTR(name), value, count)
    #define debug0(name)		_debug0(PSTR(name))
    #define debug1(name, value)		_debug1(PSTR(name), value)
    #define debug2(name, value)		_debug2(PSTR(name), value)
    #define debug4(name, value)		_debug4(PSTR(name), value)
#else
    #define debugx(name, value, count)
    #define debug0(name)
    #define debug1(name, value)
    #define debug2(name, value)
    #define debug4(name, value)
#endif

#endif //__debug_h__
