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
#include "libs/cylinder/cylinder.h"

SetupError();

/*********************************************************************************************************************/
Error cylinder_init()
{
    DDRC &= ~(CYLINDER_SDA |
	      CYLINDER_SCL);

#if CYLINDER_PULLUP_AVAILABLE
    Check(cylinder_pullups(true));
#endif

    Check(cylinder_avcc(false));

#if CYLINDER_RESET_AVAILABLE
    Check(cylinder_reset(true));
    Check(cylinder_reset(false));
#endif

    return success;
}
/*********************************************************************************************************************/
const BootModule boot_module_cylinder PROGMEM =
{
    cylinder_init
};
/*********************************************************************************************************************/
Error cylinder_pullups(bool enable)
{
#if CYLINDER_PULLUP_AVAILABLE
    if (enable)
    {
	PORTC |= (CYLINDER_SDA_PULLUP | CYLINDER_SCL_PULLUP);
	DDRC  |= (CYLINDER_SDA_PULLUP | CYLINDER_SCL_PULLUP);
    }
    else
    {
	DDRC  &= ~(CYLINDER_SDA_PULLUP | CYLINDER_SCL_PULLUP);
	PORTC &= ~(CYLINDER_SDA_PULLUP | CYLINDER_SCL_PULLUP);
    }

    return success;
#else
    return failure;
#endif
}
/*********************************************************************************************************************/
Error cylinder_avcc(bool enable)
{
    if (enable)
    {
	CYLINDER_AVCC_PORT |= CYLINDER_AVCC;
	CYLINDER_AVCC_DDR  |= CYLINDER_AVCC;
    }
    else
    {
	CYLINDER_AVCC_DDR  &= ~CYLINDER_AVCC;
	CYLINDER_AVCC_PORT &= ~CYLINDER_AVCC;
    }

    return success;
}
/*********************************************************************************************************************/
Error cylinder_reset(bool enable)
{
#if CYLINDER_RESET_AVAILABLE
    if (enable)
    {
	DDRC  |= CYLINDER_RESET;
	PORTC &= ~CYLINDER_RESET;
    }
    else
    {
	PORTC |= CYLINDER_RESET;
	DDRC  &= ~CYLINDER_RESET;
    }

    return success;
#else
    return failure;
#endif
}
/*********************************************************************************************************************/
