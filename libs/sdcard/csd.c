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
#include "libs/sdcard/sdcard.h"
#include "libs/sdcard/csd.h"

SetupError();

/*********************************************************************************************************************
 * Read the CSD the SD card.
 */
Error sd_read_csd(SDCardIndex card, uint8 *csd)
{
    uint8	response;

    CheckB(card < SDCARD_MAX_CARDS);
    Check(sd_command(card, cmd_9, 0, &response));
    Check(sd_read_base(card, csd, 16));

    return success;
}
/*********************************************************************************************************************/
