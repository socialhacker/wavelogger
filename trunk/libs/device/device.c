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
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "libs/device/device.h"
#include "libs/os/os.h"

SetupError();

/**************************************************************************************************/
void device_initialize(Device *device)
{
    device->head = null;
    device->tail = null;
}
/**************************************************************************************************/
bool message_queue(Message *message, Device *device)
{
    bool	was_empty = false;

    message->next  = null;
    message->state = message_state_queued;

    /*
     * Add the message to the end of the message queue.  This must be an atomic operation as the
     * message queue will be modified by the interrupt handler as well.
     *
     * FIX FIX FIX: I should be able to do this without turning off interrupts.
     */
    uint8 sreg_backup = SREG;
    cli();
    {
	was_empty = (device->head == null);

	/*
	 * Insert the message into the message queue.
	 */
	if (was_empty)
	    device->head = message;
	else
	    device->tail->next = message;

	device->tail = message;
    }
    SREG = sreg_backup;

    return was_empty;
}
/**************************************************************************************************/
Message *device_get_next_message(Device *device)
{
    Message	*current = device->head;

    /*
     * If the queue is empty return null.  No further pointer work needs to be done.
     */
    if (current == null)
	return null;

    /*
     * Pop the head of the queue.
     */
    device->head = current->next;

    /*
     * If this is the last message in the queue then clear the tail pointer.
     */
    if (current->next == null)
	device->tail = null;

    /*
     * Zero the next pointer so that there is no confusion later.
     */
    current->next = null;

    return current;
}
/**************************************************************************************************/
static bool message_sleep_check(void *user_data)
{
    Message	*message = (Message *) user_data;

    return ((message->state & 0x80) == 0x80);
}
/**************************************************************************************************/
Error message_wait(Message *message)
{
    os_sleep_cpu(message, message_sleep_check);

    CheckB(message->state == message_state_success);

    return success;
}
/**************************************************************************************************/
