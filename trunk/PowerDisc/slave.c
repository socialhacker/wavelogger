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
#include <avr/io.h>
#include <avr/interrupt.h>

#include "slave.h"
#include "libs/types/types.h"
#include "libs/error/error.h"

#define DDR_USI			DDRB
#define PORT_USI		PORTB
#define PIN_USI			PINB
#define PORT_USI_SDA		PORTB0
#define PORT_USI_SCL		PORTB2
#define USI_START_COND_INT	USISIF
#define USI_START_VECTOR	SIG_USI_START
#define USI_OVERFLOW_VECTOR	SIG_USI_OVERFLOW

#define USICR_START		(_BV(USISIE) | _BV(USIWM1) | _BV(USICS1))
#define USISR_1_BIT		(_BV(USIOIF) | _BV(USIPF)  | _BV(USIDC) | (0x0E << USICNT0))
#define USISR_8_BITS		(_BV(USIOIF) | _BV(USIPF)  | _BV(USIDC))

USI	_usi;

/**************************************************************************************************/
const BootModule boot_module_usi PROGMEM =
{
    usi_initialise
};
/*********************************************************************************************************************/
Error usi_initialise()
{
    device_initialize(&_usi.device);

    /*
     * Initialize the USI specific device fields.
     */
    _usi.state   = usi_check_address;
    _usi.address = 0;
    _usi.current = 0;
    _usi.message = null;
    _usi.index   = 0;

    for (uint8 i = 0; i < USI_REGISERS; ++i)
	_usi.registers[i] = 0;

    /*
     * Initialize the hardware.
     */
    PORT_USI |=  _BV(PORT_USI_SCL);	// Set SCL high
    PORT_USI |=  _BV(PORT_USI_SDA);	// Set SDA high
    DDR_USI  |=  _BV(PORT_USI_SCL);	// Set SCL as output
    DDR_USI  &= ~_BV(PORT_USI_SDA);	// Set SDA as input
    USICR     =  USICR_START;
    USISR     =  0xF0;			// Clear all flags and reset overflow counter

    return success;
}
/*********************************************************************************************************************/
void usi_set_address(uint8 address)
{
    _usi.address = address;
}
/**************************************************************************************************/
Error usi_queue_message(USIMessage *message)
{
    CheckB(message->write_count + message->read_count > 0);

    /*
     * If there are any bytes to write, then place them directly into the registers.  The first
     * byte of the message is used as the initial offset location to write to in the registers.
     */
    if (message->write_count)
    {
	uint8	offset = message->buffer[0];
	uint8	count  = (message->write_count < (USI_REGISERS - offset) ?
			  message->write_count :
			  (USI_REGISERS - offset));

	CheckB(offset < USI_REGISERS);

	message->message.state = message_state_writing;

	for (uint8 i = 0; i < count; ++i)
	    _usi.registers[i + offset] = message->buffer[i + 1];
    }

    /*
     * If there are any bytes to be read, then queue the message up so that the interupt handler
     * will fill it with bytes that come from the master.
     */
    if (message->read_count)
	message_queue(&(message->message), &(_usi.device));
    else
	message->message.state = message_state_success;

    return success;
}
/**************************************************************************************************/
Error usi_wait_message(USIMessage *message)
{
    return message_wait(&(message->message));
}
/*********************************************************************************************************************/
SIGNAL (USI_START_VECTOR)
{
    if (_usi.message)
    {
	/*
	 * Since we can't get an interrupt on stop conditions then we have to assume that the master
	 * will write a full message to us.  If it doesn't then we can't do anything until the next
	 * start condition.  At which point it's sort of too late.
	 */
	_usi.message->message.state = message_state_failure;
	_usi.message                = null;
    }

    _usi.state = usi_check_address;

    /*
     * Set the SDA (Data) line as input in preperation to receive the address and data direction
     * flag.
     */
    DDR_USI &= ~_BV(PORT_USI_SDA);

    /*
     * Wait for SCL to go low to ensure the "Start Condition" has completed.
     * If a Stop condition arises then leave the interrupt to prevent waiting forever.
     *
     * FIX FIX FIX: Do we really need to do this here?  Is it possible that we would miss the
     * stop condition as well and wait forever?
     */
    while ((PIN_USI & _BV(PORT_USI_SCL)) &
	   !(USISR & _BV(USIPF)))
    {
    }

    USICR = USICR_START  | _BV(USIOIE) | _BV(USIWM0);
    USISR = USISR_8_BITS | _BV(USI_START_COND_INT);

    os_inhibit_deep_sleep(SIGNAL_INDEX(USI_OVERFLOW_VECTOR));
}
/*********************************************************************************************************************/
SIGNAL (USI_OVERFLOW_VECTOR)
{
    switch (_usi.state)
    {
	case usi_check_address:
	{
	    if ((USIDR == 0) || ((USIDR & ~0x01) == _usi.address))
	    {
		_usi.index   = 0;
		_usi.current = 0;

		if (USIDR & 0x01)
		{
		    _usi.state = usi_send_data;
		}
		else if (_usi.device.head)
		{
		    Message	*message = device_get_next_message(&_usi.device);

		    message->state = message_state_reading;
		    _usi.message   = (USIMessage *) message;
		    _usi.state     = usi_request_data;
		}
		else
		{
		    _usi.state = usi_nack;
		}
	    }
	    break;
	}

	/******************************************************************************************/
	case usi_request_data:
	{
	    _usi.state = usi_get_data;
	    break;
	}

	case usi_get_data:
	{
	    if (_usi.current < _usi.message->read_count)
		_usi.message->buffer[_usi.current++] = USIDR;

	    if (_usi.current < _usi.message->read_count)
	    {
		_usi.state = usi_request_data;
	    }
	    else
	    {
		_usi.message->message.state = message_state_success;
		_usi.message                = null;
		_usi.state                  = usi_nack;
	    }
	    break;
	}

	/**************************************************************************************/
	case usi_request_reply:
	{
	    _usi.state = usi_check_reply;
	    break;
	}

	case usi_check_reply:
	{
	    /*
	     * If the master replied with a NACK then we are done sending data.  Otherwise
	     * we fall through to the usi_send_data handler.
	     */
	    if (USIDR != 0)
	    {
		_usi.state = usi_check_address;
		break;
	    }
	}

	case usi_send_data:
	{
	    USIDR = _usi.registers[_usi.index++];

	    if (_usi.index >= USI_REGISERS)
		_usi.index = 0;

	    _usi.state = usi_request_reply;
	    break;
	}

	/**************************************************************************************/
	case usi_nack:
	{
	    _usi.state = usi_nacked;
	    break;
	}

	case usi_nacked:
	{
	    /*
	     * Once we have nacked the master we are done until a new start condition arises.
	     */
	    break;
	}
    }

    /*
     * Manage transitions to the new state.
     */
    {
	typedef enum
	{
	    _sda_input       = 0x01,
	    _clear_usidr     = 0x02,
	    _sda_output      = 0x04,
	    _start_condition = 0x08,
	    _prepare_1_bit   = 0x10,
	    _prepare_8_bits  = 0x20,
	    _set_usidr       = 0x40
	} StateAction;

	const StateAction	state_action[] =
	{
	    /* usi_check_address */ _start_condition | _sda_input  | _prepare_8_bits,
	    /* usi_send_data     */ _clear_usidr     | _sda_output | _prepare_1_bit,
	    /* usi_request_reply */                    _sda_output | _prepare_8_bits,
	    /* usi_check_reply   */ _clear_usidr     | _sda_input  | _prepare_1_bit,
	    /* usi_request_data  */ _clear_usidr     | _sda_output | _prepare_1_bit,
	    /* usi_get_data      */                    _sda_input  | _prepare_8_bits,
	    /* usi_nack          */ _set_usidr       | _sda_output | _prepare_1_bit,
	    /* usi_nacked        */ _start_condition | _sda_input  | _prepare_8_bits,
	};

	StateAction	action = state_action[_usi.state];

	if (action & _sda_input)	DDR_USI &= ~_BV(PORT_USI_SDA);
	if (action & _clear_usidr)	USIDR    = 0x00;
	if (action & _set_usidr)	USIDR    = 0x80;
	if (action & _sda_output)	DDR_USI |= _BV(PORT_USI_SDA);
	if (action & _start_condition)	USICR    = USICR_START;
	if (action & _prepare_1_bit)	USISR    = USISR_1_BIT;
	if (action & _prepare_8_bits)	USISR    = USISR_8_BITS;
    }

    os_set_deep_sleep(SIGNAL_INDEX(USI_OVERFLOW_VECTOR), !(USICR & _BV(USIOIE)));
}
/*********************************************************************************************************************/
