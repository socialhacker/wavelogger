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
#ifndef __string_contents_h__
#define __string_contents_h__

class StringContents
{
    /*
     * Prevent copy construction and assignment.
     */
    StringContents(const StringContents &other);
    StringContents &operator=(const StringContents &other);

public:
    mutable unsigned int	_references;
    unsigned int		_length;
    unsigned int		_flags;
    char			*_data;

    StringContents(unsigned int length);
    StringContents(const char *data,
		   unsigned int length,
		   unsigned int references);
    StringContents(char *data,
		   unsigned int length,
		   unsigned int references,
		   bool manage,
		   bool use_free);
    ~StringContents(void);

    void add_reference(void) const;
    void remove_reference(void) const;
    unsigned int update(unsigned int position, const char *data, unsigned int count);
    unsigned int pad(unsigned int position, char value, unsigned int count);

    static const StringContents	empty;
};

#endif //__string_contents_h__
