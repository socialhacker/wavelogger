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
#include "libs/stdio/write.h"
#include "libs/shell/shell.h"
#include "libs/sdcard/sdcard.h"
#include "libs/sdcard/csd.h"
#include "libs/sdcard/commands.h"

SetupError();

/*********************************************************************************************************************/
Error sd_init_command(uint argc, const char **argv)
{
    CheckB(argc == 2);

    SDCardIndex	card = parse_number(argv[1]);

    return sd_init(card);
}
/*********************************************************************************************************************/
Error sd_csd_command(uint argc, const char **argv)
{
    uint8	csd[16];

    Check(sd_read_csd(0, csd));

    for (uint8 i = 0; i < 16; ++i)
	write(PSTR("csd[%d] 0x%hx\r\n"), i, csd[i]);

    write(PSTR("TAAC %d\r\n"), TAAC(csd));
    write(PSTR("NSAC %d\r\n"), NSAC(csd));
    write(PSTR("VDD_R_CURR_MIN %d\r\n"), VDD_R_CURR_MIN(csd));
    write(PSTR("VDD_R_CURR_MAX %d\r\n"), VDD_R_CURR_MAX(csd));
    write(PSTR("VDD_W_CURR_MIN %d\r\n"), VDD_W_CURR_MIN(csd));
    write(PSTR("VDD_W_CURR_MAX %d\r\n"), VDD_W_CURR_MAX(csd));

    return success;
}
/*********************************************************************************************************************/
