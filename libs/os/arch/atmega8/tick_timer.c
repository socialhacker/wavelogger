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

#include "libs/os/arch.h"

#if   ((F_CPU / (   1 * 100)) < 256)
    #define ARCH_TIMER_PRESCALE	0x01
    #define ARCH_TIMER_BITS	0
#elif ((F_CPU / (   8 * 100)) < 256)
    #define ARCH_TIMER_PRESCALE	0x02
    #define ARCH_TIMER_BITS	3
#elif ((F_CPU / (  32 * 100)) < 256)
    #define ARCH_TIMER_PRESCALE	0x03
    #define ARCH_TIMER_BITS	5
#elif ((F_CPU / (  64 * 100)) < 256)
    #define ARCH_TIMER_PRESCALE	0x04
    #define ARCH_TIMER_BITS	6
#elif ((F_CPU / ( 128 * 100)) < 256)
    #define ARCH_TIMER_PRESCALE	0x05
    #define ARCH_TIMER_BITS	7
#elif ((F_CPU / ( 256 * 100)) < 256)
    #define ARCH_TIMER_PRESCALE	0x06
    #define ARCH_TIMER_BITS	8
#elif ((F_CPU / (1024 * 100)) < 256)
    #define ARCH_TIMER_PRESCALE	0x07
    #define ARCH_TIMER_BITS	10
#else
    #error "Unable to compute tick timer constants."
#endif

#define ARCH_TIMER_COUNT	(uint8)((uint32)(F_CPU) / ((uint32)(1 << ARCH_TIMER_BITS) * 100))

ArchTickHandler	*_tick_handler             = null;
volatile uint32	_arch_timer_overflow       = 0;
volatile bool	_arch_timer_overflow_check = false;

/**************************************************************************************************/
SIGNAL (SIG_OVERFLOW2)
{
    ++_arch_timer_overflow;

    _arch_timer_overflow_check = true;
}
/**************************************************************************************************/
SIGNAL (SIG_OUTPUT_COMPARE2)
{
    OCR2 += ARCH_TIMER_COUNT;

    for (ArchTickHandler *handler = _tick_handler; handler != null; handler = handler->next)
	handler->function();
}
/**************************************************************************************************/
void arch_tick_timer_init()
{
    TCCR2  = ARCH_TIMER_PRESCALE;
    SFIOR  = _BV(PSR2);
    TCNT2  = 0;
    OCR2   = ARCH_TIMER_COUNT;
    TIMSK |= _BV(TOIE2) | _BV(OCIE2);
}
/**************************************************************************************************/
void arch_tick_timer_add(ArchTickHandler *handler)
{
    /*
     * Add this handler to the list of handlers.
     */
    uint8 sreg_backup = SREG;
    cli();
    {
	handler->next = _tick_handler;
	_tick_handler = handler;
    }
    SREG = sreg_backup;
}
/**************************************************************************************************/
uint64 arch_cycles()
{
    uint32	overflow;
    uint8	value;

    /*
     * This while loop will attempt to read the full timer value.  If there is a timer overflow interrupt during
     * the read the loop will repeat.  This prevents the race condition of the overflow value being read and the
     * timer overflow interrupt happening before the low 8 bit value is read, thus invalidating the overflow value.
     */
    do
    {
	_arch_timer_overflow_check = false;
	overflow = _arch_timer_overflow;
	value    = TCNT2;
    } while (_arch_timer_overflow_check == true);

    return ((((uint64) overflow) << 8) | value) << ARCH_TIMER_BITS;
}
/**************************************************************************************************/
