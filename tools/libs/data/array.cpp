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
#include "libs/data/array.h"

/*********************************************************************************************************************/
int Data::Comparison<uint>::compare(const void *one, const void *two)
{
    const uint	*type_one = reinterpret_cast<const uint *>(one);
    const uint	*type_two = reinterpret_cast<const uint *>(two);

    if (*type_one > *type_two)
	return 1;
    else if (*type_one < *type_two)
	return -1;
    else
	return 0;
}
/*********************************************************************************************************************/
