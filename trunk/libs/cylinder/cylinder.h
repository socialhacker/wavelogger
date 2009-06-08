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
#ifndef __cylinder_h__
#define __cylinder_h__

#include "libs/os/os.h"
#include "libs/types/types.h"
#include "libs/error/error.h"

#if defined(__AVR_ATmega8__)
    #define CYLINDER_PULLUP_AVAILABLE	1
    #define CYLINDER_RESET_AVAILABLE	1

    #define CYLINDER_AVCC_BIT		2
    #define CYLINDER_AVCC_PORT		PORTD
    #define CYLINDER_AVCC_DDR		DDRD

    #define CYLINDER_SDA_PULLUP_BIT	2
    #define CYLINDER_SCL_PULLUP_BIT	1

    #define CYLINDER_RESET_BIT		3

    #define CYLINDER_SDA_BIT		4
    #define CYLINDER_SCL_BIT		5
#elif defined(__AVR_ATmega644P__)
    #define CYLINDER_PULLUP_AVAILABLE	0
    #define CYLINDER_RESET_AVAILABLE	0

    #define CYLINDER_AVCC_BIT		2
    #define CYLINDER_AVCC_PORT		PORTC
    #define CYLINDER_AVCC_PIN		PINC
    #define CYLINDER_AVCC_DDR		DDRC

    #define CYLINDER_SDA_BIT		1
    #define CYLINDER_SCL_BIT		0
#else
    #error "Unknown device for cylinder driver."
#endif

#define CYLINDER_AVCC		(1 << CYLINDER_AVCC_BIT)
#define CYLINDER_SDA_PULLUP	(1 << CYLINDER_SDA_PULLUP_BIT)
#define CYLINDER_SCL_PULLUP	(1 << CYLINDER_SCL_PULLUP_BIT)
#define CYLINDER_RESET		(1 << CYLINDER_RESET_BIT)
#define CYLINDER_SDA		(1 << CYLINDER_SDA_BIT)
#define CYLINDER_SCL		(1 << CYLINDER_SCL_BIT)

Error cylinder_init();
Error cylinder_pullups(bool enable);
Error cylinder_avcc(bool enable);
Error cylinder_reset(bool enable);

extern const BootModule boot_module_cylinder PROGMEM;

#endif //__cylinder_h__
