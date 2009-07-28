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
#include "libs/buffer/buffer.h"
#include "libs/stdio/console.h"
#include "libs/stdio/debug.h"
#include "libs/stdio/write.h"
#include "libs/shell/shell.h"
#include "libs/file_system/file_system.h"
#include "libs/file_system/commands.h"

SetupError();

/*********************************************************************************************************************/
static Error fs_init_command(uint argc, const char **argv)
{
    CheckB(argc == 2);

    BlockIndex	block = parse_number(argv[1]);

    return fs_init(0, block);
}
/*********************************************************************************************************************/
static Error ls_command(uint argc, const char **argv)
{
    DirIndex	index;
    DirEntry	*entry;

    Check(fs_dir_open(0, &index));

    while ((entry = fs_dir_read(index)))
    {
	switch (entry->type)
	{
	    case dir_type_directory:	write(PSTR("%S <dir>\r\n"), entry->name); break;
	    default:			write(PSTR("%S       %ld\r\n"), entry->name, entry->length); break;
	}
    }

    Check(fs_dir_close(index));

    return success;
}
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
static Error cd_command(uint argc, const char **argv)
{
    Error	check_error;

    CheckB(argc == 2);

    DirIndex	dir;

    Check(fs_dir_open(0, &dir));

    DirEntry	*entry = fs_dir_lookup(dir, argv[1]);

    CheckCleanupB(entry != null, lookup_failure);
    CheckCleanup(fs_cd(0, entry), cd_failure);
    CheckCleanup(fs_dir_close(dir), dir_close_failure);

    return success;

  lookup_failure:
  cd_failure:
    fs_dir_close(dir);

  dir_close_failure:
    return check_error;
}
/*********************************************************************************************************************/
static void show_char_line(uint32 address, uint8 *data, uint length)
{
    for (uint i = 0; i < length; ++i)
	putc(data[i]);
}
/*********************************************************************************************************************/
static void show_hex_line(uint32 address, uint8 *data, uint length)
{
    uint8	j;

    write(PSTR("%lx: "), address);

    for (j = 0; j < length; ++j)
    {
	write(PSTR("%hx"), data[j]);
	putc(' ');
    }

    for (; j < 16; ++j)
    {
	putc(' ');
	putc(' ');
	putc(' ');
    }

    putc(' ');

    for (j = 0; j < length; ++j)
    {
	uint8	value = data[j];

	if (value >= 0x7f || value <= 0x1f)
	    value = '.';

	putc(value);
    }

    write(PSTR("\r\n"));
}
/*********************************************************************************************************************/
static Error show_file(const char *filename, void (*show)(uint32 address, uint8 *data, uint length))
{
    bool	done    = false;
    uint32	address = 0;
    FileIndex	file;

    Check(file_open(filename, &file));

    while (!done)
    {
	uint8	data[16] = {0};
	uint	read     = 0;
	Error	error    = fs_file_read(file, data, 16, &read);

	switch (error)
	{
	    case end_of_file:
		error_stack_clear();
		done = true;

	    case success:
		show(address, data, read);
		break;

	    default:
		write(PSTR("Error while reading file (%d).\r\n"), error);
		done = true;
		break;
	}

	address += read;
    }

    write(PSTR("\r\n"));
    Check(fs_file_close(file));

    return success;
}
/*********************************************************************************************************************/
static Error cat_command(uint argc, const char **argv)
{
    CheckB(argc == 2);
    Check(show_file(argv[1], show_char_line));
    return success;
}
/*********************************************************************************************************************/
static Error hexdump_command(uint argc, const char **argv)
{
    CheckB(argc == 2);
    Check(show_file(argv[1], show_hex_line));

    return success;
}
/*********************************************************************************************************************/
static Error write_command(uint argc, const char **argv)
{
    bool	done  = false;
    uint32	total = 0;
    FileIndex	file;

    CheckB(argc == 2);
    Check(file_open(argv[1], &file));

    while (!done)
    {
	uint8	data[17] = "0123456789abcdef";
	uint	written  = 0;
	Error	error    = fs_file_write(file, data, 16, &written);

	switch (error)
	{
	    case end_of_file:
		error_stack_clear();
		done = true;

	    case success:
		total += written;
		break;

	    default:
		write(PSTR("Error while writting file (%d).\r\n"), error);
		done = true;
		break;
	}
    }

    write(PSTR("Wrote %d bytes\r\n"), total);

    Check(fs_file_close(file));

    return success;
}
/*********************************************************************************************************************/
static Error debug_command(uint argc, const char **argv)
{
    CheckB(argc == 2);

    FSIndex	fs = parse_number(argv[1]);
    FileSystem	*file_system;

    Check(fs_get(fs, &file_system));

    write(PSTR("FAT0......: %lx\r\n"), file_system->fat_offset[0]);
    write(PSTR("FAT1......: %lx\r\n"), file_system->fat_offset[1]);
    write(PSTR("Data......: %lx\r\n"), file_system->data_offset);
    write(PSTR("Sec/Clu...: %hx\r\n"), file_system->sectors_per_cluster);
    write(PSTR("Reserved..: %x\r\n"), file_system->reserved);
    write(PSTR("Sec/Fat...: %x\r\n"), file_system->sectors_per_fat);
    write(PSTR("type......: %s\r\n"), file_system->type == fs_type_fat16 ? PSTR("FAT16") : PSTR("FAT32"));

    if (file_system->type == fs_type_fat16)
	write(PSTR("Root......: %lx\r\n"), file_system->sub.fat16.root_offset);
    else
	write(PSTR("Root......: %lx\r\n"), file_system->sub.fat32.root_cluster);

    return success;
}
/*********************************************************************************************************************/
static void show_partition(uint8 index, uint16 offset)
{
    uint8	byte    = buffer_uint8(offset + 0x00);
    uint8	type    = buffer_uint8(offset + 0x04);
    char	*status = null;

    if (byte == 0x00 &&
	type == 0x00)
	return;

    if      (byte == 0x80) status = PSTR("Active\r\n");
    else if (byte == 0x00) status = PSTR("Inactive\r\n");
    else		   status = PSTR("Invalid\r\n");

    write(PSTR("Partition   %d\r\n"), 4 - index);
    write(PSTR("    status  %s\r\n"), status);
    write(PSTR("    type    %hx\r\n"), type);
    write(PSTR("    start   %lx\r\n"), buffer_uint32(offset + 0x08));
    write(PSTR("    length  %lx\r\n"), buffer_uint32(offset + 0x0c));
}
/*********************************************************************************************************************/
static Error partition_command(uint argc, const char **argv)
{
    Check(buffer_set_address(0, 0));
    CheckB(buffer_uint16(0x1fe) == 0xaa55);

    show_partition(0, 0x1ee);
    show_partition(1, 0x1de);
    show_partition(2, 0x1ce);
    show_partition(3, 0x1be);

    return success;
}
/*********************************************************************************************************************/
const ShellCommand shell_command_fs_init   PROGMEM = {"fs_init",   fs_init_command};
const ShellCommand shell_command_ls        PROGMEM = {"ls",        ls_command};
const ShellCommand shell_command_cd        PROGMEM = {"cd",        cd_command};
const ShellCommand shell_command_cat       PROGMEM = {"cat",       cat_command};
const ShellCommand shell_command_hexdump   PROGMEM = {"hexdump",   hexdump_command};
const ShellCommand shell_command_write     PROGMEM = {"write",     write_command};
const ShellCommand shell_command_debug     PROGMEM = {"debug",     debug_command};
const ShellCommand shell_command_partition PROGMEM = {"partition", partition_command};
/*********************************************************************************************************************/
