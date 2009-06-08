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

#include "device.h"
#include "libs/os/os.h"

/**************************************************************************************************/
void device_initialize(Device *device)
{
    device->head = null;
    device->tail = null;
}
/**************************************************************************************************/
void message_queue(Message *message, Device *device)
{
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
	/*
	 * Insert the message into the message queue.
	 */
	if (device->tail != null)
	    device->tail->next = message;
	else
	    device->head = message;

	device->tail = message;
    }
    SREG = sreg_backup;
}
/**************************************************************************************************/
bool message_next(Message *message, Device *device)
{
    Message	*current = device->head;

    /*
     * If the queue is empty to begin with it will certainly be empty after, so return false.
     */
    if (current == null)
	return false;

    /*
     * If this is the last message in the queue then clear the head and tail pointers and return
     * false indicating that the queue is empty.
     */
    if (current->next == null)
    {
	device->head = null;
	device->tail = null;

	return false;
    }

    /*
     * Move to the next message in the queue.  We know there is a next item at this point so we
     * can return true indicating that there is something left in the queue.
     */
    device->head  = current->next;
    current->next = null;

    return true;
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
