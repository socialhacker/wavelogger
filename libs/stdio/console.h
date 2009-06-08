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
#ifndef __console_h__
#define __console_h__

#include <avr/pgmspace.h>

#include "libs/types/types.h"

void puts(const char *s);
void puts_pgm(const prog_char *s);
void putsn_pgm(const prog_char *s, uint8 count);
void putc(const char c);
void read_line(char *line, uint8 length);

#endif //__console_h__
