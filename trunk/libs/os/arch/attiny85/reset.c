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
#include <avr/io.h>

#include "libs/os/arch.h"
#include "libs/eeprom/eeprom.h"

/*********************************************************************************************************************/
void arch_load_oscal()
{
    /*
     * Nothing to do here.  THe Tiny85 automatically loads it's OSCAL register.
     */
}
/**************************************************************************************************/
uint8 arch_reset_reason()
{
    static uint8	result = 0;

    if (result == 0)
    {
	result = MCUSR;
	MCUSR  = 0;
    }

    return result;
}
/**************************************************************************************************/
