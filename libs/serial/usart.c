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

#include "libs/serial/serial.h"

#define BAUD_RATE		57600
#define BAUD_RATE_REGISTER	(F_CPU / (BAUD_RATE * 8l) - 1)

typedef struct
{
    uint8	head;
    uint8	tail;
    bool	overflow;
    uint8	data[16];
} Buffer;

volatile Buffer	read  = {0, 15, false};

/*********************************************************************************************************************/
uint8 uart_read()
{
    uint8	data;
    uint8	next = (read.tail + 1) & 0x0f;

    while (next == read.head)
	;

    data      = read.data[next];
    read.tail = next;

    return data;
}
/*********************************************************************************************************************/
bool uart_character_ready()
{
    uint8	next = (read.tail + 1) & 0x0f;

    return (next != read.head);
}
/*********************************************************************************************************************/
void uart_write(uint8 data)
{
    while(!(UCSR0A & _BV(UDRE0)))
	;

    UDR0 = data;
}
/*********************************************************************************************************************/
Error uart_init(void)
{
    /*
     * Set the baud rate.
     */
    UBRR0H = (uint8)((BAUD_RATE_REGISTER >> 8) & 0xff);
    UBRR0L = (uint8)((BAUD_RATE_REGISTER >> 0) & 0xff);

    /*
     * Enable double rate baud generation, the transmitter and receiver,
     * the reveiver interrupt and set the UART to asynchronous mode with
     * 8N1 data format.
     */
    UCSR0A = _BV(U2X0);
    UCSR0B = _BV(RXEN0)  | _BV(TXEN0)  | _BV(RXCIE0);
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);

    os_inhibit_deep_sleep(SIGNAL_INDEX(SIG_USART_RECV));

    return success;
}
/*********************************************************************************************************************/
Error uart_init_write_only(void)
{
    Check(uart_init());

    os_enable_deep_sleep(SIGNAL_INDEX(SIG_USART_RECV));

    return success;
}
/*********************************************************************************************************************/
SIGNAL(SIG_USART_RECV)
{
    uint8 data = UDR0;

    if (read.head == read.tail)
    {
	read.overflow = true;
	return;
    }

    read.data[read.head] = data;
    read.head = (read.head + 1) & 0x0f;
}
/*********************************************************************************************************************/
