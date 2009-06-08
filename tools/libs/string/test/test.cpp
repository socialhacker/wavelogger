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
#include "libs/string/string.h"
#include "libs/log/log_default.h"

using namespace Log;

/*********************************************************************************************************************/
int main(int argc, char **argv)
{
    String	test("This is a test string");
    String	another("Another test string");
    String	temp;

    notice("test......: %s", (const char *)test);
    notice("another...: %s", (const char *)another);

    temp = test;

    notice("temp......: %s", (const char *)temp);

    temp = another;

    notice("temp......: %s", (const char *)temp);
    notice("test......: %s", (const char *)test);
    notice("another...: %s", (const char *)another);

    temp = test.substring(5, 10);

    notice("temp......: %s", (const char *)temp);

    temp = temp.erase(2, 2).erase(7, 1).append("ing");

    notice("temp......: %s", (const char *)temp);
    notice("equal.....: %d", test.equal(another));
    notice("equal.....: %d", test.equal("This is a test string"));

    temp = test.substring(5, 10).substring(2, 2);

    notice("temp......: %s", (const char *)temp);

    return 0;
}
/*********************************************************************************************************************/
