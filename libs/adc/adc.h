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
#ifndef __adc_h__
#define __adc_h__

#include "libs/os/os.h"
#include "libs/types/types.h"
#include "libs/error/error.h"

Error adc_init();
Error adc_read(uint8 channel_index, uint16 *value);
Error adc_read_all(uint16 *value);

extern const BootModule boot_module_adc PROGMEM;

#endif //__adc_h__
