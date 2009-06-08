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
#ifndef __led_h__
#define __led_h__

#include "libs/os/os.h"
#include "libs/error/error.h"
#include "libs/types/types.h"

extern const BootModule boot_module_led PROGMEM;

Error led_init   ();
void  led_set    (uint index, bool value);
void  led_set_all(bool value);
void  led_flash  (uint8 count, bool wide);
void  led_signal (uint8 narrow, uint8 wide);

#endif //__led_h__
