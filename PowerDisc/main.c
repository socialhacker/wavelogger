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
 * PB0 - SDA         / MOSI (ISP interface)
 * PB1 - AVCC_ENABLE / MISO (ISP interface)
 * PB2 - SCL         / SCLK (ISP interface)
 * PB3 - Battery Voltage
 * PB4 - Temperature
 * PB5 -               RESET (ISP interface)
 */

#include <avr/io.h>
#include <avr/sleep.h>

#include "libs/types/types.h"
#include "libs/error/error.h"
#include "libs/os/os.h"

#include "slave.h"

SetupError();

uint16		fuses __attribute__((section (".fuse"))) = 0xdfe2;

/*********************************************************************************************************************/
const BootModule	*boot_module_table[] PROGMEM =
{
    &boot_module_usi
};
/*********************************************************************************************************************/
static uint16 sample(uint8 mux)
{
    ADCSRA &= ~_BV(ADIF);
    ADMUX   = mux;
    ADCSRA |=  _BV(ADSC);

    while ((ADCSRA & _BV(ADIF)) == 0);

    return ADC;
}
/*********************************************************************************************************************/
int main(void)
{
    Error	check_error = success;

    DDRB  = 0x02;
    PORTB = 0x02;

    CheckCleanup(os_boot(boot_module_table, LENGTH(boot_module_table)), boot_failure);

    usi_set_address(0x12);

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    /*
     * Setup A/D converter
     */
    DIDR0  = _BV(ADC2D) | _BV(ADC3D);
    ADCSRA = _BV(ADEN) | 0x06;
    ADCSRB = 0x00;

    /*
     *
     */
    uint8	buffer[5];
    USIMessage	message;

    message.buffer      = buffer;
    message.write_count = 0;
    message.read_count  = 4;

    while (1)
    {
	volatile uint16	*short_buffer = (volatile uint16 *)(message.buffer + 1);

	CheckCleanup(usi_queue_message(&message), queue_failure);
	CheckCleanup(usi_wait_message (&message), wait_failure);

	message.buffer[0]   = 0;
	message.write_count = 4;
	message.read_count  = 4;

	os_sleep_ms(1);
	short_buffer[0] = sample(0x03); //PB4 - Battery Voltage
	os_sleep_ms(1);
	short_buffer[1] = sample(0x02); //PB3 - Temperature
    }

    return 0;

  wait_failure:
  queue_failure:
  boot_failure:
    return 1;
}
/*********************************************************************************************************************/
