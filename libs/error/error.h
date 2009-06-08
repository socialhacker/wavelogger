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
#ifndef __error_h__
#define __error_h__

#include <avr/pgmspace.h>

#include "libs/types/types.h"

#define ERROR_STACK_MAX_ENTRIES		32

typedef enum
{
    success     = 0,
    failure     = 1,
    e_timeout   = 2,
    end_of_file = 3
} Error;

typedef struct
{
    const prog_char	*filename;
    uint16		line_number;
    Error		error;
} ErrorEntry;

void       error_stack_add(const prog_char *filename,
			   uint16 line_number,
			   Error error);
void       error_stack_clear();
uint8      error_stack_count();
ErrorEntry *error_stack_entry(uint8 index);

#ifdef ERROR_STACK_ENABLE
#define SetupError()			static const prog_char error_file[] PROGMEM = __FILE__
#define ERROR_STACK_ADD(f, l, e)	error_stack_add(f, l, e)
#else
#define SetupError()
#define ERROR_STACK_ADD(f, l, e)
#endif

/*********************************************************************************************************************/
#define Check(statement)					     \
({								     \
    Error __error__ = (statement);				     \
    if (__error__)						     \
    {								     \
	ERROR_STACK_ADD(error_file, __LINE__, __error__);	     \
        return __error__;					     \
    }								     \
})
/*********************************************************************************************************************/
#define CheckB(statement)					     \
({								     \
    bool __bool__ = (statement);				     \
    if (!__bool__)						     \
    {								     \
	ERROR_STACK_ADD(error_file, __LINE__, !__bool__);	     \
        return failure;						     \
    }								     \
})
/*********************************************************************************************************************/
#define CheckCleanupB(statement, label)				     \
({								     \
    bool	__bool__ = (statement);				     \
    if (!__bool__)						     \
    {								     \
	check_error = failure;					     \
	ERROR_STACK_ADD(error_file, __LINE__, !__bool__);	     \
        goto label;						     \
    }								     \
})
/*********************************************************************************************************************/
#define CheckCleanup(statement, label)				     \
({								     \
    Error	__check_error__ = (statement);			     \
    if (__check_error__)					     \
    {								     \
	check_error = __check_error__;				     \
	ERROR_STACK_ADD(error_file, __LINE__, check_error);	     \
        goto label;						     \
    }								     \
})
/*********************************************************************************************************************/

#endif //__error_h__
