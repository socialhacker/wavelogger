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
 * ADC0 - LED 1
 * ADC1 - LED 2
 * ADC2 - 
 * ADC3 - 
 * ADC4 - 
 * ADC5 - 
 * ADC6 - 
 * ADC7 - 
 *
 * PB0 -
 * PB1 -
 * PB2 -
 * PB3 - LED 3
 * PB4 - LED 4
 * PB5 - MOSI (ISP interface)
 * PB6 - MISO (ISP interface)
 * PB7 - SCLK (ISP interface)
 *
 * RESET - RESET (ISP interface)
 *
 * PC0 - Stack SCL
 * PC1 - Stack SDA
 * PC2 - Stack Avcc
 * PC3 - 
 * PC4 - SW1
 * PC5 - 
 * PC6 -
 * PC7 -
 *
 * PD0 - Rx
 * PD1 - Tx
 * PD2 - SDCard MISO
 * PD3 - SDCard MOSI
 * PD4 - SDCard SCK
 * PD5 - SDCard CD
 * PD6 - SDCard !SS
 * PD7 -
 *
 */

#include "record.h"

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "libs/types/types.h"
#include "libs/error/error.h"

#include "libs/shell/shell.h"
#include "libs/stdio/debug.h"
#include "libs/stdio/write.h"
#include "libs/stdio/console.h"
#include "libs/serial/serial.h"
#include "libs/cylinder/cylinder.h"
#include "libs/spi/spi.h"
#include "libs/sdcard/sdcard.h"
#include "libs/sdcard/csd.h"
#include "libs/block/block.h"
#include "libs/twi/twi.h"
#include "libs/rtc/rtc.h"
#include "libs/analog_disc/analog_disc.h"
#include "libs/gain/gain.h"
#include "libs/adc/adc.h"
#include "libs/dac/dac.h"
#include "libs/power/power.h"
#include "libs/file_system/file_system.h"
#include "libs/led/led.h"
#include "libs/os/os.h"

SetupError();

uint16	fuses __attribute__((section (".fuse"))) = 0xd9e2;

/*********************************************************************************************************************/
const BootModule	*boot_module_table[] PROGMEM =
{
    &boot_module_led,
    &boot_module_serial_write_only,
    &boot_module_cylinder,

    /*
     * Boot the spi and sdcard modules.
     */
    &boot_module_spi,
    &boot_module_sdcard,
    &boot_module_file_system,

    /*
     * Boot the I2C bus.  This allows access to the analog front end as well as the real time clock,
     * battery monitor and temperature sensors.
     */
    &boot_module_twi,
    &boot_module_gain,
    &boot_module_dac,
    &boot_module_adc,
    &boot_module_analog
};
/*********************************************************************************************************************/
int main(void)
{
    Error	check_error = success;
    uint8	narrow;
    uint8	wide;

    os_sleep(100);

    CheckCleanup(os_boot(boot_module_table, LENGTH(boot_module_table)), boot_failure);
    CheckCleanup(record_command(0, null), record_failure);

    return 0;

  record_failure:	narrow = 2; wide = 1; goto failure;
  boot_failure:		narrow = 1; wide = os_boot_index(); goto failure;
  failure:
    error_stack_print();
    while (1) led_signal(narrow, wide);
    return 1;
}
/*********************************************************************************************************************/
