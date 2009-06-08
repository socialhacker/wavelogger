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
#ifndef __twi_trace_h__
#define __twi_trace_h__

#include "libs/twi/master.h"

typedef struct
{
    TWIState	state;
    uint8	control;
} TWITrace;

#define TRACE_BUFFER_SIZE	16

#define RECORD_TRACE(_state, _control)	trace_add(_state, _control)

void trace_reset();
uint8 trace_count();
TWITrace *trace_entry(uint8 index);
void trace_add(TWIState state, uint8 control);

#endif //__twi_trace_h__
