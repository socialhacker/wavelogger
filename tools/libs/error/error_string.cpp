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
#include "libs/error/error_string.h"

#define DEFINE_ERROR(index, name)	#name,

static const char *error_names[] =
{
    #include "libs/error/error_list.h"
};

#undef DEFINE_ERROR

/*********************************************************************************************************************/
const char *Err::error_string(Err::Error error)
{
    return error_names[error];
}
/*********************************************************************************************************************/
