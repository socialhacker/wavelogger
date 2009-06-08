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
#ifndef __data_array_h__
#define __data_array_h__

#include <stdio.h>
#include <stdlib.h>

#include "globals.h"
#include "libs/error/error.h"

namespace Data
{
    template<class type> class List;

    template <class type> class Comparison
    {
    public:
	static int compare(const void *one, const void *two)
	{
	    const type	*type_one = reinterpret_cast<const type *>(one);
	    const type	*type_two = reinterpret_cast<const type *>(two);

	    return type::compare(type_one, type_two);
	}
    };

    template <> class Comparison<uint>
    {
    public:
	static int compare(const void *one, const void *two);
    };

    //! A simple array class.
    /*!
     *
     */
    template <class type> class Array
    {
	uint	_size;
	type	*_array;

	void swap_contents(Array &other)
	{
	    swap(_size,  other._size);
	    swap(_array, other._array);
	}

    public:
	/*!
	 * The Array constructor creates an empty array.
	 */
	Array() :
	    _size(0),
	    _array(null)
	{
	}

	/*!
	 * The Array constructor creates an array of size \a size.
	 */
	explicit Array(uint initial_size) :
	    _size(initial_size),
	    _array(null)
	{
	    if (_size > 0)
		_array = new type[_size];
	}

	/*!
	 * The Array constructor creates an array that wrappes a C array.
	 */
	Array(uint initial_size, type *array) :
	    _size(initial_size),
	    _array(array)
	{
	}

	/*!
	 * Initialize this array with the contents of \a other.
	 */
	Array(const Array &other) :
	    _size(other._size),
	    _array(null)
	{
	    if (_size > 0)
		_array = new type[_size];

	    for (uint i = 0; i < _size; ++i)
		_array[i] = other[i];
	}

	/*!
	 * Copy the contents of \a other into this Array, overwriting the existing contents.
	 */
	Array &operator=(const Array &other)
	{
	    if (this != &other)
	    {
		Array	new_array(other.size());

		for (uint i = 0; i < new_array.size(); ++i)
		    new_array[i] = other[i];

		swap_contents(new_array);
	    }

	    return *this;
	}

	/*!
	 * The Array destructor deletes all of the elements in the array.  If the type of the array is a pointer
	 * the object pointed to by the pointer is not deleted.
	 */
	virtual ~Array() { delete[] _array; }

	/*!
	 * Return a pointer to the array.
	 */
	inline operator type*() { return _array; }

	/*!
	 * Return a const pointer to the array.
	 */
	inline operator const type*() const { return _array; }

	/*!
	 * Size returns the number of elements in the array.
	 */
	uint size() const	{return _size;}

	/*!
	 * Count returns the number of elements in the array.
	 */
	uint count() const	{return _size;}

	/*!
	 * Length returns the number of elements in the array.
	 */
	uint length() const	{return _size;}

	/*!
	 * Empty returns true if the Array has no elements and false otherwise.
	 */
	uint empty() const	{return (_size == 0);}

	/*!
	 * Access the array without any bounds checking.
	 */
	type &operator [] (int index) { return _array[index]; }

	/*!
	 * Access the const array without any bounds checking.
	 */
	const type &operator [] (int index) const { return _array[index]; }

	/*!
	 * Sort the array in place.
	 */
	void sort() { qsort(_array, _size, sizeof(type), Comparison<type>::compare); }

	/*!
	 * Resize the array.  If the array is increased in size the original data is preserved.  If the array
	 * is reduced in size, only the elements that can fit in the new array are coppied over.
	 */
	void resize(uint new_size)
	{
	    if (new_size != _size)
	    {
		Array	new_array(new_size);
		uint	min_size = new_size < _size ? new_size : _size;

		for (uint i = 0; i < min_size; ++i)
		    new_array[i] = _array[i];

		swap_contents(new_array);
	    }
	}

	/*!
	 * Clear the contents of the array.  This sets everything to zero.
	 *
	 * \todo This isn't the right behavior...
	 */
	void clear() { memset(_array, 0, size() * sizeof(type)); }

	/*!
	 * Append the elements in /a list to the Array, increasing the size of the array by the size of /a list.
	 */
	void append(const List<type> &list);

	/*!
	 * Append the elements in /a array to the Array, increasing the size of the array by the size of /a array.
	 */
	void append(const Array &array)
	{
	    uint	index = size();

	    resize(size() + array.size());

	    /*
	     * This can not be a memcpy because the asignment operator may be overloaded.
	     */
	    for (uint i = 0; i < array.size(); ++i)
		_array[index + i] = array[i];
	}

	/*!
	 * Append /a data to the Array, increasing the size of the array by one.
	 */
	void append(const type &data)
	{
	    uint	index = size();

	    resize(size() + 1);

	    _array[index] = data;
	}

	/*!
	 * Check if \a data is in any location in the array.  This is a linear operation in the array size.
	 */
	bool exists(const type &data) const
	{
	    for (uint i = 0; i < _size; ++i)
		if (_array[i] == data)
		    return true;

	    return false;
	}
    };
}

#endif //__data_array_h__
