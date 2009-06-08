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

#include "libs/spi/spi.h"

#define SPI_SS_BIT	6
#define SPI_MOSI_BIT	3
#define SPI_MISO_BIT	2
#define SPI_SCLK_BIT	4

#define SPI_SS		(1 << SPI_SS_BIT)
#define SPI_MOSI	(1 << SPI_MOSI_BIT)
#define SPI_MISO	(1 << SPI_MISO_BIT)
#define SPI_SCLK	(1 << SPI_SCLK_BIT)

#define SPI_PORT	PORTD
#define SPI_DDR		DDRD

/*********************************************************************************************************************/
Error spi_init()
{
    /*
     * Baud rate register must be zero before enabling the USART SPI mode.  Leaving it at zero sets
     * the baud rate to F_CPU / 2.
     */
    UBRR1 = 0;

    /*
     * Setup the Data Direction Register so the SPI data input is an input and so that the clock, chip select
     * and data output are outputs.
     */
    const uint8	mask  = SPI_MISO | SPI_SCLK | SPI_MOSI | SPI_SS;
    const uint8 value =            SPI_SCLK | SPI_MOSI | SPI_SS;

    SPI_DDR = (SPI_DDR & ~mask) | value;
    UCSR1C  = _BV(UMSEL11) | _BV(UMSEL10);
    UCSR1B  = _BV(RXEN1) | _BV(TXEN1);
    UBRR1   = 7;

    return success;
}
/*********************************************************************************************************************/
const BootModule boot_module_spi PROGMEM =
{
    spi_init
};
/*********************************************************************************************************************/
void spi_enable()
{
    SPI_PORT &= ~SPI_SS;
}
/*********************************************************************************************************************/
void spi_disable()
{
    SPI_PORT |= SPI_SS;
}
/*********************************************************************************************************************/
uint8 spi_rw(uint8 data)
{
    while (!(UCSR1A & _BV(UDRE1)))
	;

    UDR1 = data;

    while (!(UCSR1A & _BV(RXC1)))
	;

    return UDR1;
}
/*********************************************************************************************************************/
