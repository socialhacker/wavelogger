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

#include "record.h"

#include "libs/analog_disc/analog_disc.h"
#include "libs/file_system/file_system.h"
#include "libs/serial/serial.h"
#include "libs/dac/dac.h"
#include "libs/led/led.h"
#include "libs/gain/gain.h"
#include "libs/rtc/rtc.h"
#include "libs/stdio/write.h"

#include <util/crc16.h>

SetupError();

typedef struct
{
    char	filename[13];
    char	metadata[13];
    uint16	sample_per_second;
    uint8	gain;
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
static bool wait_on_switch()
{
    uint8	count = 0;

    while (count < 10)
    {
	if (uart_character_ready())
	    return true;

	if (PINC & 0x10)
	    ++count;
	else
	    count = 0;
    }

    return false;
}
/*********************************************************************************************************************/
static Error offset_search(InfoData * info)
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
static Error check_file_position(FileIndex meta, uint32 *file_position, bool *quit)
{
    Check(fs_file_read(meta, file_position, sizeof(*file_position), null));

    if (*file_position == (uint32) 0xffffffff)
    {
	/*
	 * If the file position is invalid then we wait for the user to activate the magnetic
	 * switch before we start writing to the data file at its beginning.
	 */
	led_flash(2, true);
	*quit = wait_on_switch();
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
static uint8		data[2][512];
static AnalogMessage	message[2] =
{
    {{message_state_new, null}, data[0], 200},
    {{message_state_new, null}, data[1], 200}
};

Error record_command(uint argc, const char **argv)
{
    Error	check_error = success;
    uint8	count       = 0;
    uint8	narrow[10]  = {4, 3, 3, 3, 3, 3, 3, 2, 2, 2};
    uint8	wide[10]    = {1, 6, 5, 4, 3, 2, 1, 3, 2, 1};

    for (uint8 i = 0; i < 2; ++i)
	for (uint16 j = 0; j < sizeof(data[0]); ++j)
	    data[i][j] = 0;

    /*
     * Read configuration data and use it to open files and set gain.
     */
    FileIndex	file;
    FileIndex	meta;

    {
	ConfigData	config_data;

	CheckCleanup(read_config_data("config.dat", &config_data), config_failure);
	CheckCleanup(file_open(config_data.metadata, &meta), meta_failure);
	CheckCleanup(file_open(config_data.filename, &file), open_failure);
	CheckCleanup(gain_write_all(config_data.gain), gain_failure);
    }

    /*
     * Seek into the data file past any data that has already been written to it.
     */
    {
	uint32		file_position;
	bool		quit = false;

	CheckCleanup(check_file_position(meta, &file_position, &quit), position_failure);
	CheckCleanup(fs_file_seek(file, file_position, seek_type_beginning), seek_failure);

	if (quit)
	{
	    fs_file_close(file);
	    fs_file_close(meta);
	    return success;
	}
    }

    /*
     * Populate the info_data and write it to the data file.
     */
    {
	InfoData	*info_data = (InfoData *) data[0];

	info_data->reset_reason = os_reset_reason();

	CheckCleanup(offset_search(info_data), search_failure);
	CheckCleanup(fs_file_write(file, info_data, sizeof(*info_data), null), info_data_failure);
    }

    /*
     * Queue up both analog messages and start recording.
     */
    CheckCleanup(analog_queue_message(&(message[0])), queue_failure);
    CheckCleanup(analog_queue_message(&(message[1])), queue_failure);

    const uint32	meta_interval   = 100 * 100; // 100 sample blocks
    uint32		last_meta_ticks = 0;
    uint8		index           = 0;

    while (!uart_character_ready())
    {
	AnalogData	*analog_data = (AnalogData *) data[index];
	uint32		start_ticks  = os_ticks();

	analog_wait_message(&(message[index]));

	/*
	 * Record how long we were waiting for the message.
	 */
	{
	    uint32	ticks = os_ticks() - start_ticks;

	    if (ticks > 0xff) ticks = 0xff;

	    analog_data->ticks = (uint8) ticks;
	}

	/*
	 * Compute the message checksum.
	 */
	{
	    uint16	crc = 0xffff;

	    for (uint i = 0; i < 512; ++i)
		crc = _crc_ccitt_update(crc, message[index].buffer[i]);

	    analog_data->checksum = crc;
	}

	/*
	 * Write the message to the SDCard.
	 */
	CheckCleanup(fs_file_write(file, data[index], sizeof(data[index]), null), logging_failure);

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

    fs_file_close(file);
    fs_file_close(meta);
    return success;

  logging_failure:	++count;
  queue_failure:	++count;
  info_data_failure:	++count;
  search_failure:	++count;
  seek_failure:		++count;
  position_failure:	++count;
  gain_failure:		++count; fs_file_close(file);
  open_failure:		++count; fs_file_close(meta);
  meta_failure:		++count;
  config_failure:	++count;
    if (count > 10)
	count = 10;

    while (!uart_character_ready())
	led_signal(narrow[count - 1], wide[count - 1]);

    return check_error;
}
/*********************************************************************************************************************/
