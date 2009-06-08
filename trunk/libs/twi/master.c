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
/*
 * TWI Driver code.  Originally based on the Atmel AVR315 appnote.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "libs/twi/master.h"
#include "libs/twi/trace.h"
#include "libs/os/os.h"

SetupError();

static TWI	_twi = {null, null, 0};

/**************************************************************************************************/
const TWI *twi_get()
{
    return &_twi;
}
/**************************************************************************************************/
Error twi_initialize()
{
    /*
     * Initialize TWI bit rate.  The prescaler is always set to a value of 1.  Thus the bit rate
     * is F_CPU / (16 + 2 * TWBR).  Where TWBR must be at least 10.
     */
    TWBR = TWI_TWBR;
    TWSR = 0x00;

    /*
     * Default content = SDA released.
     */
    TWDR = 0xFF;

    /*
     * Enable TWI-interface and release TWI pins.
     * Disable Interupt.
     */
    TWCR = _BV(TWEN);

    return success;
}
/**************************************************************************************************/
void twi_release()
{
    PORTC &= ~(_BV(4) | _BV(5));
    DDRC  &= ~(_BV(4) | _BV(5));

    TWCR = 0x00;
}
/**************************************************************************************************/
Error twi_queue_message(TWIMessage *message)
{
    CheckB(message->write_count + message->read_count > 0);

    message->next = null;

    if (message->write_count)	message->state = twi_message_writing;
    else			message->state = twi_message_reading;

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
	if (_twi.tail != null)
	    _twi.tail->next = message;
	else
	    _twi.head = message;

	_twi.tail = message;

	/*
	 * If the TWI interrupt handler is not enabled we initiate a new START condition on the
	 * bus.
	 */
	if (!(TWCR & _BV(TWIE)))
	    TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWSTA);
	else
	    message->state = twi_message_queued;

	os_inhibit_deep_sleep(SIGNAL_INDEX(SIG_2WIRE_SERIAL));
    }
    SREG = sreg_backup;

    return success;
}
/**************************************************************************************************/
Error twi_wait_message(TWIMessage *message, uint8 timeout)
{
    uint32	limit = os_ticks() + timeout;

    while (message->state & 0x80)
	if (timeout && (os_ticks() >= limit))
	    Check(e_timeout);

    CheckB(message->state == twi_message_success);

    return success;
}
/**************************************************************************************************/
SIGNAL (SIG_2WIRE_SERIAL)
{
    /*
     * We never need to go to user space to resolve errors so we always set TWINT in the control
     * register.  This ensures that the TWI state machine is always running.
     */
    uint8	control_reg  = _BV(TWINT) | _BV(TWEN);
    TWIMessage	*current     = _twi.head;
    bool	next_message = false;

    switch (TWSR)
    {
	case TWI_START:
	case TWI_REP_START:
	    switch (current->state)
	    {
		case twi_message_writing:
		    TWDR           = current->address | TWI_WRITE;
		    _twi.current   = 0;
		    break;

		case twi_message_reading:
		    TWDR           = current->address | TWI_READ;
		    _twi.current   = 0;
		    break;

		default:
		    current->state = twi_message_failure;
		    next_message   = true;
		    break;
	    }
	    break;

	case TWI_MTX_ADR_ACK:
	case TWI_MTX_DATA_ACK:
	    if (_twi.current < current->write_count)
	    {
		TWDR = current->buffer[_twi.current++];
	    }
	    else if (current->read_count)
	    {
		current->state = twi_message_reading;
		control_reg |= _BV(TWSTA);
	    }
	    else
	    {
		current->state = twi_message_success;
		next_message   = true;
	    }
	    break;

	case TWI_MRX_ADR_ACK:
	    if (_twi.current < (current->read_count - 1))
		control_reg |= _BV(TWEA);
	    break;

	case TWI_MRX_DATA_ACK:
	    current->buffer[_twi.current++] = TWDR;

	    if (_twi.current < (current->read_count - 1))
		control_reg |= _BV(TWEA);
	    break;

	case TWI_MRX_DATA_NACK:
	    current->buffer[_twi.current++] = TWDR;
	    current->state = twi_message_success;
	    next_message   = true;
	    break;

	case TWI_MTX_DATA_NACK:
	    if (_twi.current == current->write_count)
	    {
		if (current->read_count)
		{
		    current->state = twi_message_reading;
		    control_reg |= _BV(TWSTA);
		}
		else
		{
		    current->state = twi_message_success;
		    next_message   = true;
		}
	    }
	    else
	    {
		current->state = twi_message_data_nack;
		next_message   = true;
	    }
	    break;

	case TWI_MTX_ADR_NACK:
	    current->state = twi_message_address_nack;
	    next_message   = true;
	    break;

	case TWI_MRX_ADR_NACK:
	    current->state = twi_message_address_nack;
	    next_message   = true;
	    break;

	case TWI_ARB_LOST:
	    current->state = twi_message_arbitration_lost;
	    next_message   = true;
	    break;

	default:
	    current->state = twi_message_failure;
	    next_message   = true;
	    break;
    }

    if (next_message)
    {
	if (current->next)
	{
	    /*
	     * Move to the next message in the queue.  Initiate a repeated start condition and
	     * generate an interrupt when it's done.
	     */
	    _twi.head = current->next;
	    current->next = null;

	    if (_twi.head == null)
		_twi.tail = null;

	    control_reg |= _BV(TWSTA) | _BV(TWIE);

	    if (_twi.head->write_count)	_twi.head->state = twi_message_writing;
	    else			_twi.head->state = twi_message_reading;
	}
	else
	{
	    /*
	     * We were processing the last message in the message queue.  Remove it from the queue
	     * and initiate a stop condition on the bus.  Don't generate an interrupt (we can't
	     * anyway because a stop condition doesn't generate an interrupt).  The message queuing
	     * code will start the interrupt handler again if it adds a new message to the queue.
	     */
	    _twi.head = null;
	    _twi.tail = null;
	    control_reg |= _BV(TWSTO);
	}
    }
    else
    {
	/*
	 * Continue sending the current message.  Generate an interrupt after the next operation.
	 */
	control_reg |= _BV(TWIE);
    }

    os_set_deep_sleep(SIGNAL_INDEX(SIG_2WIRE_SERIAL), (control_reg & _BV(TWIE)) == 0);

    TWCR = control_reg;
}
/**************************************************************************************************/
