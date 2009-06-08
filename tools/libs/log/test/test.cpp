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
#include <stdlib.h>

#include "libs/error/error.h"
#include "libs/sys/sys.h"
#include "libs/log/log_default.h"
#include "libs/log/log_file.h"
#include "libs/log/log_html.h"

using namespace Err;
using namespace Sys;
using namespace Log;

/*********************************************************************************************************************/
int main(int argc, char **argv)
{
    LogFile	log_file;
    LogHTML	log_html;
    Err::Error	check_error;

    CheckCleanup(log_file.open("test.log", true), open_failed);
    CheckCleanup(log_html.open("test.html", true), open_failed);

    log().set_level(2);
    log().set_prefix("Test");
    log().add_log(log_file);
    log().add_log(log_html);

    notice("This is a test message 0x%0x8", 0x12345678);
    debug(0, "This is a test debug message %d", 100);
    debug(1, "This is a deep test debug message");
    debug(2, "This is a deeper test debug message");
    debug(3, "This message shouldn't show up");
    debug(2, "This debug message takes\n"
	  "Multiple lines");
    notice("This notice also\n"
	   "takes a number\n"
	   "of lines.");
    notice("This is a notice from the logging library that should probably be wrapped because it is longer than "
	   "most screens are wide.  It is possible that someone has a screen this wide, but its not likely.");
    warning("This is a test warning %s", "(plus a string)");
    notice("And here-we-have-a-message-that-attempts-to-trigger-the-failure-mode-of-the-line-wrapper-by-having-"
	   "a-sequence-of-characters-with-no-breaks-so-the-wrapper-has-no-where-to-break-the-line.  I hope it works.");
    error("This is a test error (%d)", 15);

    return 0;

  open_failed:
    error_stack_print();
    return 1;
}
/*********************************************************************************************************************/
