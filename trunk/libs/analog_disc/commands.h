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
#ifndef __analog_disc_commands_h__
#define __analog_disc_commands_h__

#include "libs/shell/shell.h"

Error search_command(uint argc, const char **argv);
Error sample_command(uint argc, const char **argv);
Error gain_command(uint argc, const char **argv);
Error dac_command(uint argc, const char **argv);
Error adc_command(uint argc, const char **argv);
Error up_command(uint argc, const char **argv);
Error down_command(uint argc, const char **argv);
Error test_command(uint argc, const char **argv);

#endif //__analog_disc_commands_h__
