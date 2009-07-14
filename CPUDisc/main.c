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
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "libs/types/types.h"
#include "libs/error/error.h"

#include "libs/stdio/debug.h"
#include "libs/stdio/write.h"
#include "libs/stdio/console.h"
#include "libs/serial/serial.h"
#include "libs/cylinder/commands.h"
#include "libs/cylinder/cylinder.h"
#include "libs/spi/spi.h"
#include "libs/sdcard/commands.h"
#include "libs/file_system/commands.h"
#include "libs/file_system/file_system.h"
#include "libs/buffer/commands.h"
#include "libs/twi/commands.h"
#include "libs/twi/twi.h"
#include "libs/rtc/commands.h"
#include "libs/rtc/rtc.h"
#include "libs/analog_disc/commands.h"
#include "libs/analog_disc/analog_disc.h"
#include "libs/gain/gain.h"
#include "libs/adc/adc.h"
#include "libs/dac/dac.h"
#include "libs/power/commands.h"
#include "libs/power/power.h"
#include "libs/led/commands.h"
#include "libs/led/led.h"
#include "libs/os/os.h"
#include "libs/shell/shell.h"
#include "libs/shell/commands.h"

SetupError();

uint16	fuses __attribute__((section (".fuse"))) = 0xd9e2;

/*********************************************************************************************************************/
const BootModule	*boot_module_table[] PROGMEM =
{
    &boot_module_led,
    &boot_module_serial,
    &boot_module_cylinder,

    /*
     * Boot the I2C bus.  This allows access to the analog front end as well as the real time clock,
     * battery monitor and temperature sensors.
     */
    &boot_module_twi,
    &boot_module_gain,
    &boot_module_dac,
    &boot_module_adc,
    &boot_module_analog,

    /*
     * Boot the filesystem.  This requires the spi and sdcard modules.
     */
    &boot_module_spi,
    &boot_module_sdcard,
    &boot_module_file_system
};
/*********************************************************************************************************************/
const ShellCommand shell_command_time      PROGMEM = {"time",      time_command};
const ShellCommand shell_command_stack     PROGMEM = {"stack",     stack_command};
const ShellCommand shell_command_status    PROGMEM = {"status",    status_command};

const ShellCommand shell_command_led       PROGMEM = {"led",       led_command};

/*
 * Cylinder commands
 */
const ShellCommand shell_command_release   PROGMEM = {"release",   release_command};
const ShellCommand shell_command_master    PROGMEM = {"master",    master_command};
const ShellCommand shell_command_message   PROGMEM = {"message",   message_command};
const ShellCommand shell_command_twi       PROGMEM = {"twi",       twi_debug_command};
const ShellCommand shell_command_avcc      PROGMEM = {"avcc",      avcc_command};

/*
 * SDCard commands
 */
const ShellCommand shell_command_sd_csd    PROGMEM = {"sd_csd",    sd_csd_command};

/*
 * Filesystem commands
 */
const ShellCommand shell_command_ls        PROGMEM = {"ls",        ls_command};
const ShellCommand shell_command_cd        PROGMEM = {"cd",        cd_command};
const ShellCommand shell_command_cat       PROGMEM = {"cat",       cat_command};
const ShellCommand shell_command_hexdump   PROGMEM = {"hexdump",   hexdump_command};
const ShellCommand shell_command_dump      PROGMEM = {"dump",      dump_command};
const ShellCommand shell_command_write     PROGMEM = {"write",     write_command};
const ShellCommand shell_command_debug     PROGMEM = {"debug",     debug_command};
const ShellCommand shell_command_partition PROGMEM = {"partition", partition_command};

/*
 * Analog board commands
 */
const ShellCommand shell_command_gain      PROGMEM = {"gain",      gain_command};
const ShellCommand shell_command_adc       PROGMEM = {"adc",       adc_command};
const ShellCommand shell_command_dac       PROGMEM = {"dac",       dac_command};
const ShellCommand shell_command_search    PROGMEM = {"search",    search_command};
const ShellCommand shell_command_sample    PROGMEM = {"sample",    sample_command};
const ShellCommand shell_command_average   PROGMEM = {"average",   average_command};
const ShellCommand shell_command_test      PROGMEM = {"test",      test_command};
const ShellCommand shell_command_up        PROGMEM = {"up",        up_command};
const ShellCommand shell_command_down      PROGMEM = {"down",      down_command};

/*
 * Power board commands
 */
const ShellCommand shell_command_battery   PROGMEM = {"battery",   battery_command};
const ShellCommand shell_command_temp      PROGMEM = {"temp",      temp_command};
const ShellCommand shell_command_rtc       PROGMEM = {"rtc",       rtc_command};
const ShellCommand shell_command_rtc_date  PROGMEM = {"rtc_date",  rtc_set_date_command};
const ShellCommand shell_command_rtc_time  PROGMEM = {"rtc_time",  rtc_set_time_command};
const ShellCommand shell_command_reset     PROGMEM = {"reset",     reset_command};

/*
 * Record command
 */
const ShellCommand shell_command_record    PROGMEM = {"record",    record_command};

const ShellCommand	*shell_command_table[] PROGMEM =
{
    &shell_command_time,
    &shell_command_stack,
    &shell_command_status,

    &shell_command_led,

    &shell_command_release,	//boot_module twi
    &shell_command_master,	//boot_module twi
    &shell_command_message,	//boot_module twi
    &shell_command_twi,		//boot_module twi
    &shell_command_avcc,	//boot_module cylinder

    &shell_command_sd_csd,

    &shell_command_ls,		//boot_module file_system
    &shell_command_cd,		//boot_module file_system
    &shell_command_cat,		//boot_module file_system
    &shell_command_hexdump,	//boot_module file_system
    &shell_command_dump,	//boot_module file_system
    &shell_command_write,	//boot_module file_system
    &shell_command_debug,	//boot_module file_system
    &shell_command_partition,	//boot_module file_system

    &shell_command_gain,	//boot_module adc
    &shell_command_adc,		//boot_module adc
    &shell_command_dac,		//boot_module dac
    &shell_command_search,	//boot_module adc
    &shell_command_sample,	//boot_module adc
    &shell_command_average,	//boot_module adc
    &shell_command_test,	//boot_module adc
    &shell_command_up,		//boot_module adc
    &shell_command_down,	//boot_module adc

    &shell_command_battery,
    &shell_command_temp,
    &shell_command_rtc,
    &shell_command_rtc_date,
    &shell_command_rtc_time,
    &shell_command_reset,

    &shell_command_record,
};
/*********************************************************************************************************************/
int main(void)
{
    os_boot(boot_module_table, LENGTH(boot_module_table));
    error_stack_print();

    write(PSTR("\r\n\r\nWavelogger v4.0\r\n"));
    write(PSTR("Reset reason: 0x%hx\r\n"), os_reset_reason());

    shell_run(shell_command_table, LENGTH(shell_command_table));

    return 0;
}
/*********************************************************************************************************************/
