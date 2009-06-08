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
#ifndef __powerdisc_slave_h__
#define __powerdisc_slave_h__

#include "libs/types/types.h"
#include "libs/error/error.h"
#include "libs/os/os.h"
#include "device.h"

/*
 * Device driver state.
 */
typedef enum
{
    usi_check_address = 0,
    usi_send_data     = 1,
    usi_request_reply = 2,
    usi_check_reply   = 3,
    usi_request_data  = 4,
    usi_get_data      = 5,
    usi_nack          = 6,
    usi_nacked        = 7
} USIState;

/*
 *
 */
typedef struct USIMessageStruct
{
    Message		message;

    uint8 * volatile	buffer;
    uint8 volatile	write_count;
    uint8 volatile	read_count;
} USIMessage;

#define USI_REGISERS	8

typedef struct
{
    Device	device;

    USIState	state;
    uint8	address;
    uint8	current;
    USIMessage	*message;

    uint8	index;
    uint8	registers[USI_REGISERS];
} USI;

Error usi_initialise();
void  usi_set_address(uint8 address);
Error usi_queue_message(USIMessage *message);
Error usi_wait_message(USIMessage *message);

extern const BootModule boot_module_usi PROGMEM;

#endif //__powerdisc_slave_h__
