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

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/crc16.h>

#include "libs/types/types.h"
#include "libs/error/error.h"

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

typedef struct
{
    char	filename[13];
    char	metadata[13];
    uint16	sample_per_second;
    bool	auto_zero;
    bool	compute_checksum;
} ConfigData;

typedef struct
{
    int32	position;
} MetaData;

typedef struct
{
    uint8	type;
    uint8	reset_reason;
    uint8	padding0[2];
    uint16	offset[4];
    uint8	padding1[512 - 12];
} InfoData;

STATIC_ASSERT(sizeof(InfoData)   == 512, sizeof_InfoData_not_512);
STATIC_ASSERT(sizeof(AnalogData) == 512, sizeof_AnalogData_not_512);

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
static Error file_open(const char *filename, FileIndex *file)
{
    Error	check_error = success;
    DirIndex	dir;
    DirEntry	*entry;

    CheckCleanup(fs_dir_open(0, &dir), dir_open_failure);
    CheckCleanupB((entry = fs_dir_lookup(dir, filename)) != null, lookup_failure);
    CheckCleanup(fs_file_open(0, entry, file), file_open_failure);
    CheckCleanup(fs_dir_close(dir), dir_close_failure);

    return success;

  dir_close_failure:
    fs_file_close(*file);

  file_open_failure:
  lookup_failure:
    fs_dir_close(dir);

  dir_open_failure:
    return check_error;
}
/*********************************************************************************************************************/
static Error read_config_data(const char *filename, ConfigData *config_data)
{
    Error	check_error = success;
    uint	read        = 0;
    FileIndex	config_file;

    Check(file_open(filename, &config_file));
    CheckCleanup(fs_file_read(config_file,
			      (uint8 *) config_data,
			      sizeof(ConfigData),
			      &read), failure);
    CheckCleanupB(read == sizeof(ConfigData), failure);
    CheckCleanup(fs_file_close(config_file), failure);

    return success;

  failure:
    fs_file_close(config_file);

    return check_error;
}
/*********************************************************************************************************************/
static void error_stack_print()
{
    uint8	count = error_stack_count();

    if (count >= ERROR_STACK_MAX_ENTRIES)
    {
	puts_pgm(PSTR("ErrorStack overflow detected.\r\n"));
	count = ERROR_STACK_MAX_ENTRIES;
    }

    if (count)
    {
	puts_pgm(PSTR("\r\nErrorStack:\r\n"));
    }

    for (uint8 i = count; i > 0; --i)
    {
	ErrorEntry	*entry = error_stack_entry(i - 1);

	WRITE("%s:%d:%hd\r\n", entry->filename, entry->line_number, entry->error);
    }

    error_stack_clear();
}
/*********************************************************************************************************************/
static void wait_on_switch()
{
    uint8	count = 0;

    while (count < 10)
    {
	if (PINC & 0x10)
	    ++count;
	else
	    count = 0;
    }
}
/*********************************************************************************************************************/
static Error check_avcc()
{
    if ((CYLINDER_AVCC_PIN & _BV(CYLINDER_AVCC_BIT)) == 0)
    {
	led_flash(1, false);
	Check(cylinder_avcc(true));
    }

    return success;
}
/*********************************************************************************************************************/
static Error offset_search(InfoData *info)
{
    for (uint8 i = 0; i < 4; ++i)
    {
	uint16	offset = 0;

	Check(analog_offset_search(i, &offset));
	Check(dac_write(i, offset));

	WRITE("DAC %d offset = %d\r\n", i, offset);

	info->offset[i] = offset;
	led_flash(1, true);
    }

    return success;
}
/*********************************************************************************************************************/
static Error check_file_position(FileIndex meta, uint32 *file_position)
{
    Check(fs_file_read(meta, file_position, sizeof(*file_position), null));

    if (*file_position == (uint32) 0xffffffff)
    {
	/*
	 * If the file position is invalid then we wait for the user to activate the magnetic
	 * switch before we start writing to the data file at its beginning.
	 */
	led_flash(2, true);
	wait_on_switch();
	led_flash(2, true);

	*file_position = 0;
    }
    else
    {
	/*
	 * If the file position is valid it means we have already been writing to the data file
	 * and we need to move ahead 100 sample blocks on disk so as not to overwrite any data
	 * that was recorded after the last meta data file write.
	 */
	*file_position += (uint32) 512 * 100;
    }

    /*
     * Write the new file position to the meta data file so that if we restart before the meta
     * write interval we won't clobber the initial blocks that we write.  This is usefull
     * during testing where the device is restarted frequently and in case of catastrophic
     * failure in the field that causes the device to reset over and over again.  We will at
     * least be able to detect that condition if the first info block is written.
     */
    Check(fs_file_seek(meta, 0, seek_type_beginning));
    Check(fs_file_write(meta, file_position, sizeof(*file_position), null));
    Check(fs_file_sync(meta));

    return success;
}
/*********************************************************************************************************************/
int main(void)
{
    const uint32	meta_interval   = 100 * 100; // 100 sample blocks

    Error		check_error     = success;
    uint8		index           = 0;
    FileIndex		file;
    FileIndex		meta;
    uint32		last_meta_ticks = 0;
    ConfigData		config_data;
    uint8		raw_data[2][512];
    AnalogData		*analog_data[2] = {(AnalogData *) raw_data[0],
					   (AnalogData *) raw_data[1]};
    InfoData		*info_data      = (InfoData *) raw_data[0];
    AnalogMessage	message[2]      = {{raw_data[0], 200},
					   {raw_data[1], 200}};

    for (uint8 i = 0; i < 2; ++i)
	for (uint16 j = 0; j < sizeof(raw_data[0]); ++j)
	    raw_data[i][j] = 0;

    os_sleep(100);

    CheckCleanup(os_boot(boot_module_table, LENGTH(boot_module_table)), boot_failure);
    CheckCleanup(read_config_data("config.dat", &config_data), config_failure);
    CheckCleanup(file_open(config_data.metadata, &meta), meta_failure);
    CheckCleanup(file_open(config_data.filename, &file), open_failure);
    CheckCleanup(check_avcc(), avcc_failure);

    /*
     * Seek into the data file past any data that has already been written to it.
     */
    {
	uint32		file_position;

	CheckCleanup(check_file_position(meta, &file_position), position_failure);
	CheckCleanup(fs_file_seek(file, file_position, seek_type_beginning), seek_failure);
    }

    info_data->reset_reason = os_reset_reason();

    /*
     * Set the gain to 50x
     */
    CheckCleanup(gain_write_all(0xf3), gain_failure);
    CheckCleanup(offset_search(info_data), search_failure);
    CheckCleanup(fs_file_write(file, info_data, sizeof(*info_data), null), info_data_failure);

    CheckCleanup(analog_queue_message(&(message[0])), queue_failure);
    CheckCleanup(analog_queue_message(&(message[1])), queue_failure);

    while (1)
    {
	uint32	start_ticks = os_ticks();

	analog_wait_message(&(message[index]));

	/*
	 * Record how long we were waiting for the message.
	 */
	{
	    uint32	ticks = os_ticks() - start_ticks;

	    if (ticks > 0xff) ticks = 0xff;

	    analog_data[index]->ticks = (uint8) ticks;
	}

	/*
	 * Compute the message checksum.
	 */
	{
	    uint16	crc = 0xffff;

	    for (uint i = 0; i < 512; ++i)
		crc = _crc_ccitt_update(crc, message[index].buffer[i]);

	    analog_data[index]->checksum = crc;
	}

	/*
	 * Write the message to the SDCard.
	 */
	CheckCleanup(fs_file_write(file, raw_data[index], sizeof(raw_data[index]), null),
		     logging_failure);

	/*
	 * If the meta_interval has elapsed then we write the current file position to the meta
	 * data file.
	 */
	if (start_ticks - last_meta_ticks > meta_interval)
	{
	    uint32	position;

	    CheckCleanup(fs_file_pos(file, &position), logging_failure);
	    CheckCleanup(fs_file_seek(meta, 0, seek_type_beginning), logging_failure);
	    CheckCleanup(fs_file_write(meta, &position, sizeof(position), null), logging_failure);
	    CheckCleanup(fs_file_sync(meta), logging_failure);
	    last_meta_ticks = start_ticks;
	}

	analog_queue_message(&(message[index]));

	index ^= 1;

	/*
	 * Reset the stack if the reed switch is active.
	 */
	if (PINC & 0x10)
	    rtc_reset_stack();
    }

    return 0;

    uint8	narrow;
    uint8	wide;

  logging_failure:	narrow = 4; wide = 1; goto failure;
  queue_failure:	narrow = 3; wide = 6; goto failure;
  info_data_failure:	narrow = 3; wide = 5; goto failure;
  search_failure:	narrow = 3; wide = 4; goto failure;
  gain_failure:		narrow = 3; wide = 3; goto failure;
  seek_failure:		narrow = 3; wide = 2; goto failure;
  position_failure:	narrow = 3; wide = 1; goto failure;
  avcc_failure:		narrow = 2; wide = 4; goto failure;
  open_failure:		narrow = 2; wide = 3; goto failure;
  meta_failure:		narrow = 2; wide = 2; goto failure;
  config_failure:	narrow = 2; wide = 1; goto failure;
  boot_failure:		narrow = 1; wide = os_boot_index(); goto failure;

  failure:
    error_stack_print();
    while (1) led_signal(narrow, wide);
    return 1;
}
/*********************************************************************************************************************/
