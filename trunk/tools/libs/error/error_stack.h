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
#ifndef __error_error_stack_h__
#define __error_error_stack_h__

#include <stdarg.h>
#include <assert.h>

#include "globals.h"
#include "libs/error/error.h"

/*!
 * \def ENABLE_ERROR_STACK
 * Enable the error stack.  This will cause Error to track errors as they are passed back up a call chain.  At any time
 * the contents of the error stack can be displayed to better debug the error.  If this macro is not defined then
 * Error' Check* and IgnoreError functions still work, they just don't record the error.
 *
 * \def ENABLE_TRACE
 * This will slow you down considerably.  But it will generate a trace of all calls to the Check* family of macros.
 *
 * \def CHECK_WILL_PRINT
 * While initially writing a project it can be usefull to print out what would be placed on the error stack, even if
 * its not really an error, or the stack is not enabled.  Examples of where it might not be an error is a failure
 * returned from a lookup function.  It might be expected that you not find the element that your looking for.
 *
 * \def CHECK_EXIT_WILL_PRINT
 * Print the error stack just before exiting.
 *
 * \def CHECK_CLEANUP_WILL_PRINT
 * Similar to CHECK_WILL_PRINT, but for CheckCleanup.
 *
 * \def IGNORE_ERROR_WILL_PRINT
 * Print the error stack just before any errors that were pushed onto it during the processing of the code inside
 * of the scope of the IgnoreError are popped.
 */
/*********************************************************************************************************************
 *
 */

namespace Err
{
    class ErrorStack
    {
    public:
	/*
	 * The error stack is made up of Element objects.  The element object encapsulates all of the info about an
	 * error.
	 */
	class Element
	{
	    enum ElementType
	    {
		element_type_sys,
		element_type_posix,
		element_type_boolean
	    };

	    ElementType	_type;
	    int		_error;
	    const char	*_function;
	    const char	*_contents;
	    const char	*_file_name;
	    uint	_line_number;
	    const char	*_optional_string;

	    /*
	     * No copying of ErrorStack ELements.
	     */
	    Element(const Element &other);

	public:
	    Element(Error error,
		    const char *function,
		    const char *contents,
		    const char *file_name,
		    uint line_number,
		    const char *optional_string,
		    ...);
	    Element(int error,
		    const char *function,
		    const char *contents,
		    const char *file_name,
		    uint line_number,
		    const char *optional_string,
		    ...);
	    Element(bool error,
		    const char *function,
		    const char *contents,
		    const char *file_name,
		    uint line_number,
		    const char *optional_string,
		    ...);
	    Element();
	    ~Element();

	    Element &operator=(const Element &other);

	    void print() const;
	    void simple_print() const;
	};

	/*
	 * Top is used to save our place in the error stack when we know that errors from deeper calls can be ignored.
	 * This lets us back the error stack up without losing any errors that might be important.
	 */
	class Top
	{
	    friend class ErrorStack;

	    uint	_new_bottom;

	public:
	    Top();
	    ~Top();
	};

	enum CheckType
	{
	    check_type_check         = 0,
	    check_type_check_exit    = 1,
	    check_type_check_assert  = 2,
	    check_type_check_cleanup = 3,
	    check_type_ignore        = 4,
	    check_type_last          = 5
	};

    private:
	Element		_array[32];
	uint		_current;

	bool		_enabled;
	bool		_will_print[check_type_last];

    public:
	ErrorStack();
	~ErrorStack();

	void update(const Element &element, CheckType type);
	void push(const Element &element);
	void simple_print();
	void print();
	void print_control(CheckType type, bool enabled);
	void clear();
	bool empty();
	void push_stack(Top &top);
	void pop_stack(Top &top);
    };

    void print_trace();

    void error_stack_update(const ErrorStack::Element &element, ErrorStack::CheckType type);
    void error_stack_print();
    void error_stack_print_control(ErrorStack::CheckType type, bool enabled);
    void error_stack_clear();
    void error_stack_trace_before(const ErrorStack::Element &element, ErrorStack::CheckType type);
    void error_stack_trace_after(const ErrorStack::Element &element, ErrorStack::CheckType type);

    inline bool sys_check(Error error)    { return (error != success); }
    inline bool posix_check(int error)    { return (error < 0); }
    inline bool boolean_check(bool error) { return !error; }

    inline Error sys_map(Error error)    { return error; }
    inline Error posix_map(int error)    { return ((error < 0) ? failure : success); }
    inline Error boolean_map(bool error) { return ((error) ? success : failure); }
}

/*********************************************************************************************************************/
#define	ELEMENT(check_error, call, string...)								\
	Err::ErrorStack::Element __element(check_error, __FUNCTION__, call, __FILE__, __LINE__, string)	\

/*********************************************************************************************************************/
#ifdef ENABLE_TRACE
#define TRACE_BEFORE(call, type, string...)		\
{							\
    ELEMENT(success, call, string);			\
    error_stack_trace_before(__element, type);		\
}

#define TRACE_AFTER(check_error, call, type, string...)	\
{							\
    ELEMENT(check_error, call, string);			\
    error_stack_trace_after(__element, type);		\
}
#else
#define TRACE_BEFORE(call, type, string...)
#define TRACE_AFTER(check_error, call, type, string...)
#endif

/*********************************************************************************************************************/
#define	Check(error)					CheckBase(sys,     Err::Error, error, null)
#define	CheckP(error)					CheckBase(posix,   int,        error, null)
#define	CheckB(error)					CheckBase(boolean, bool,       error, null)
#define	CheckString(error, string...)			CheckBase(sys,     Err::Error, error, string)
#define	CheckStringP(error, string...)			CheckBase(posix,   int,        error, string)
#define	CheckStringB(error, string...)			CheckBase(boolean, bool,       error, string)

#define CheckBase(domain, type, error, string...)						\
({												\
    TRACE_BEFORE(#error, Err::ErrorStack::check_type_check, string)				\
    type	__check_error__ = (error);							\
    TRACE_AFTER(__check_error__, #error, Err::ErrorStack::check_type_check, string)		\
    if (Err::domain##_check(__check_error__))							\
    {												\
	ELEMENT(__check_error__, #error, string);						\
	error_stack_update(__element, Err::ErrorStack::check_type_check);			\
	return Err::domain##_map(__check_error__);						\
    }												\
})

/*********************************************************************************************************************/
#define	CheckAssert(error)				CheckAssertBase(sys,     Err::Error, error, null)
#define	CheckAssertP(error)				CheckAssertBase(posix,   int,        error, null)
#define	CheckAssertB(error)				CheckAssertBase(boolean, bool,       error, null)
#define	CheckAssertString(error, string...)		CheckAssertBase(sys,     Err::Error, error, string)
#define	CheckAssertStringP(error, string...)		CheckAssertBase(posix,   int,        error, string)
#define	CheckAssertStringB(error, string...)		CheckAssertBase(boolean, bool,       error, string)

#define CheckAssertBase(domain, type, error, string...)						\
({												\
    TRACE_BEFORE(#error, Err::ErrorStack::check_type_check_assert, string)			\
    type	__check_error__ = (error);							\
    TRACE_AFTER(__check_error__, #error, Err::ErrorStack::check_type_check_assert, string)	\
												\
    if (Err::domain##_check(__check_error__))							\
    {												\
	ELEMENT(__check_error__, #error, string);						\
	error_stack_update(__element, Err::ErrorStack::check_type_check_assert);		\
	assert(0);										\
    }												\
})

/*********************************************************************************************************************/
#define	CheckExit(error)				CheckExitBase(sys,     Err::Error, error, null)
#define	CheckExitP(error)				CheckExitBase(posix,   int,        error, null)
#define	CheckExitB(error)				CheckExitBase(boolean, bool,       error, null)
#define	CheckExitString(error, string...)		CheckExitBase(sys,     Err::Error, error, string)
#define	CheckExitStringP(error, string...)		CheckExitBase(posix,   int,        error, string)
#define	CheckExitStringB(error, string...)		CheckExitBase(boolean, bool,       error, string)

#define CheckExitBase(domain, type, error, string...)						\
({												\
    TRACE_BEFORE(#error, Err::ErrorStack::check_type_check_exit, string)			\
    type	__check_error__ = (error);							\
    TRACE_AFTER(__check_error__, #error, Err::ErrorStack::check_type_check_exit, string)	\
												\
    if (Err::domain##_check(__check_error__))							\
    {												\
	ELEMENT(__check_error__, #error, string);						\
	error_stack_update(__element, Err::ErrorStack::check_type_check_exit);			\
	exit(int(Err::domain##_map(__check_error__)));						\
    }												\
})

/*********************************************************************************************************************/
#define	CheckCleanup(error, label)			CheckCleanupBase(sys,     Err::Error, error, label, null)
#define	CheckCleanupP(error, label)			CheckCleanupBase(posix,   int,        error, label, null)
#define	CheckCleanupB(error, label)			CheckCleanupBase(boolean, bool,       error, label, null)
#define	CheckCleanupString(error, label, string...)	CheckCleanupBase(sys,     Err::Error, error, label, string)
#define	CheckCleanupStringP(error, label, string...)	CheckCleanupBase(posix,   int,        error, label, string)
#define	CheckCleanupStringB(error, label, string...)	CheckCleanupBase(boolean, bool,       error, label, string)

#define	CheckCleanupBase(domain, type, error, lable, string...)					\
({												\
    TRACE_BEFORE(#error, Err::ErrorStack::check_type_check_cleanup, string)			\
    type	__check_error__ = (error);							\
    TRACE_AFTER(__check_error__, #error, Err::ErrorStack::check_type_check_cleanup, string)	\
												\
    if (Err::domain##_check(__check_error__))							\
    {												\
	ELEMENT(__check_error__, #error, string);						\
	error_stack_update(__element, Err::ErrorStack::check_type_check_cleanup);		\
	check_error = Err::domain##_map(__check_error__);					\
	goto lable;										\
    }												\
})

/*********************************************************************************************************************
 *
 */

#define	IgnoreError(error)				IgnoreErrorBase(sys,     Err::Error, error, null)
#define	IgnoreErrorP(error)				IgnoreErrorBase(posix,   int,        error, null)
#define	IgnoreErrorB(error)				IgnoreErrorBase(boolean, bool,       error, null)
#define	IgnoreErrorString(error, string...)		IgnoreErrorBase(sys,     Err::Error, error, string)
#define	IgnoreErrorStringP(error, string...)		IgnoreErrorBase(posix,   int,        error, string)
#define	IgnoreErrorStringB(error, string...)		IgnoreErrorBase(boolean, bool,       error, string)

#define IgnoreErrorBase(domain, type, error, string...)						\
({												\
    Err::ErrorStack::Top	__error_stack_top__;						\
    TRACE_BEFORE(#error, Err::ErrorStack::check_type_ignore, string)				\
    type			__check_error__ = (error);					\
    TRACE_AFTER(__check_error__, #error, Err::ErrorStack::check_type_ignore, string)		\
												\
    if (Err::domain##_check(__check_error__))							\
    {												\
	ELEMENT(__check_error__, #error, string);						\
	Err::error_stack_update(__element, Err::ErrorStack::check_type_ignore);			\
    }												\
												\
    __check_error__;										\
})

/*********************************************************************************************************************/
#endif //__error_error_stack_h__
