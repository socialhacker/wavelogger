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
#include "libs/twi/twi.h"
#include "libs/twi/commands.h"
#include "libs/stdio/write.h"
#include "libs/error/error.h"
#include "libs/shell/shell.h"

SetupError();

/*********************************************************************************************************************/
Error release_command(uint argc, const char **argv)
{
    twi_release();
    return success;
}
/*********************************************************************************************************************/
Error master_command(uint argc, const char **argv)
{
    twi_initialize(true);
    return success;
}
/*********************************************************************************************************************/
Error message_command(uint argc, const char **argv)
{
    CheckB(argc == 2);

    uint8	address   = parse_number(argv[1]) & ~0x01;
    uint8	buffer[4] = {0x11, 0x23, 0x47, 0x8f};
    TWIMessage	message   = {buffer, address, LENGTH(buffer), LENGTH(buffer)};
    Error	error     = success;

    Check(twi_queue_message(&message));

    switch (error = twi_wait_message(&message, 100))
    {
	case success:
	    write(PSTR("success\r\n"));

	    for (uint8 i = 0; i < LENGTH(buffer); ++i)
		write(PSTR("    buffer[%d] = %hx\r\n"), i, buffer[i]);

	    break;

	default:
	    write(PSTR("message.state = %d\r\n"), message.state);
	    Check(error);
	    break;
    }

    return success;
}
/*********************************************************************************************************************/
Error twi_debug_command(uint argc, const char **argv)
{
    const TWI	*twi = twi_get();

    write(PSTR("twi.head    = %x\r\n"), twi->head);
    write(PSTR("twi.tail    = %x\r\n"), twi->tail);
    write(PSTR("twi.current = %hd\r\n"), twi->current);

    for (TWIMessage *current = twi->head; current != null; current = current->next)
    {
	write(PSTR("current     = %x\r\n"),      current);
	write(PSTR("    buffer      = %x\r\n"),  current->buffer);
	write(PSTR("    address     = %hx\r\n"), current->address);
	write(PSTR("    write_count = %hd\r\n"), current->write_count);
	write(PSTR("    read_count  = %hd\r\n"), current->read_count);
	write(PSTR("    state       = %hx\r\n"), current->state);
	write(PSTR("    next        = %x\r\n"),  current->next);
    }

    return success;
}
/*********************************************************************************************************************/
