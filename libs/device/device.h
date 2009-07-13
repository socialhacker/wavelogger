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
#ifndef __libs_device_h__
#define __libs_device_h__

#include "libs/error/error.h"

typedef enum
{
    message_state_new     = 0x00,
    message_state_success = 0x01,
    message_state_failure = 0x02,

    message_state_queued  = 0x83,
    message_state_reading = 0x84,
    message_state_writing = 0x85,

    message_state_last
} MessageState;

typedef struct MessageStruct
{
    MessageState volatile		state;
    struct MessageStruct * volatile	next;
} Message;

typedef struct DeviceStruct
{
    Message * volatile			head;
    Message * volatile			tail;
} Device;

void     device_initialize      (Device *device);
Message *device_get_next_message(Device *device);

bool  message_queue(Message *message, Device *device);
Error message_wait (Message *message);

#endif //__libs_device_h__
