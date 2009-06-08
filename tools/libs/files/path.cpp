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
#include "libs/files/path.h"

using namespace Files;

class PathContents
{
    bool		_absolute;
    Data::Array<String>	_element;

public:
};

/*********************************************************************************************************************/
Path::Path() :
    _path(".")
{
}
/*********************************************************************************************************************/
Path::Path(String path) :
    _path(path)
{
}
/*********************************************************************************************************************/
String Path::get() const
{
    return _path;
}
/*********************************************************************************************************************/
void Path::set(String path)
{
    _path = path;
}
/*********************************************************************************************************************/
