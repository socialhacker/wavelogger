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
#ifndef __twi_master_h__
#define __twi_master_h__

#include "libs/types/types.h"
#include "libs/error/error.h"
#include "libs/twi/state.h"

/*
 * Set TWI_TWBR to the value that will be used for TWBR.
 * The value of TWSR will always be 0.
 * TWBR = (F_CPU / (2 * RATE)) - 8
 * At 8MHz the following values are valid:
 *
 * 152  25KHz
 *  72  50KHz
 *  32 100KHz
 *  12 200KHz
 *   8 250KHz
 *   2 400KHz
 */
#define TWI_TWBR	12

/*
 *
 */
#define TWI_READ		0x01
#define TWI_WRITE		0x00

typedef enum
{
    twi_message_new              = 0x00,

    twi_message_success          = 0x01,
    twi_message_failure          = 0x02,
    twi_message_address_nack     = 0x03,
    twi_message_data_nack        = 0x04,
    twi_message_arbitration_lost = 0x05,

    twi_message_queued           = 0x80,
    twi_message_writing          = 0x81,
    twi_message_reading          = 0x82,
} TWIMessageState;

typedef struct TWIMessageStruct
{
    uint8 volatile			*buffer;
    uint8 volatile			address;
    uint8 volatile			write_count;
    uint8 volatile			read_count;
    TWIMessageState volatile		state;

    struct TWIMessageStruct * volatile	next;
} TWIMessage;

typedef struct
{
    TWIMessage * volatile	head;
    TWIMessage * volatile	tail;
    uint8 volatile		current;
} TWI;

/*
 *
 */
Error twi_initialize();
void  twi_release();
Error twi_queue_message(TWIMessage *message);
Error twi_wait_message(TWIMessage *message, uint8 timeout);
const TWI *twi_get();

#endif //__twi_master_h__
