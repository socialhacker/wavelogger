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
#include "libs/twi/trace.h"

TWITrace	_trace_buffer[TRACE_BUFFER_SIZE];
uint8		_trace_count;

/**************************************************************************************************/
void trace_reset()
{
    _trace_count = 0;
}
/**************************************************************************************************/
uint8 trace_count()
{
    return _trace_count;
}
/**************************************************************************************************/
TWITrace *trace_entry(uint8 index)
{
    if (index >= _trace_count)
	return null;

    return (_trace_buffer + index);
}
/**************************************************************************************************/
void trace_add(TWIState state, uint8 control)
{
    if (_trace_count < TRACE_BUFFER_SIZE)
    {
	_trace_buffer[_trace_count].state   = state;
	_trace_buffer[_trace_count].control = control;
	_trace_count++;
    }
}
/**************************************************************************************************/
