# -*- mode: makefile; -*-
#
# Copyright 2009 Anton Staaf
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
depth		:= ../../..
products	:= log test

log[type]	:= library
log[cpp_files]	:= log_interface.cpp log_default.cpp log_dispatch.cpp log_console.cpp log_file.cpp log_html.cpp
log[libs]	:= sys:sys error:error

test[type]	:= program
test[cpp_files]	:= test/test.cpp
test[libs]	:= log:log sys:sys

include $(depth)/build/Makefile.native
