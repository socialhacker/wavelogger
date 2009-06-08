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
#include "libs/error/error.h"

static ErrorEntry	error_stack[ERROR_STACK_MAX_ENTRIES];
static uint8		error_count;

/*********************************************************************************************************************/
void error_stack_add(const prog_char *filename,
		     uint16 line_number,
		     Error error)
{
    if (error_count < ERROR_STACK_MAX_ENTRIES)
    {
	error_stack[error_count].filename    = filename;
	error_stack[error_count].line_number = line_number;
	error_stack[error_count].error       = error;

	++error_count;
    }
}
/*********************************************************************************************************************/
void error_stack_clear()
{
    error_count = 0;
}
/*********************************************************************************************************************/
uint8 error_stack_count()
{
    return error_count;
}
/*********************************************************************************************************************/
ErrorEntry *error_stack_entry(uint8 index)
{
    if (index < error_count)
	return (error_stack + index);

    return null;
}
/*********************************************************************************************************************/
