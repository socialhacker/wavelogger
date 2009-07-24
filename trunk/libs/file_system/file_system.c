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
#include <string.h>
#include <avr/pgmspace.h>

#include "libs/stdio/write.h"
#include "libs/stdio/console.h"
#include "libs/stdio/debug.h"
#include "libs/error/error.h"
#include "libs/buffer/buffer.h"
#include "libs/file_system/file_system.h"
#include "libs/os/os.h"

SetupError();

#define SIGNATURE		0xaa55
#define BYTES_PER_SECTOR	0x0200	// This must match the BYTES_PER_BLOCK
#define FAT_COUNT		0x02
#define ROOT_DIRECTORY_ENTRIES	0x0200
#define BYTES_PER_ENTRY		0x20
#define ENTRIES_PER_SECTOR	(BYTES_PER_SECTOR / BYTES_PER_ENTRY)

#define	ATTRIBUTE_ARCHIVE_BIT	5
#define	ATTRIBUTE_DIRECTORY_BIT	4
#define	ATTRIBUTE_VOLUME_BIT	3
#define	ATTRIBUTE_SYSTEM_BIT	2
#define	ATTRIBUTE_HIDDEN_BIT	1
#define	ATTRIBUTE_READ_ONLY_BIT	0

#define	ATTRIBUTE_ARCHIVE	(1 << ATTRIBUTE_ARCHIVE_BIT)
#define	ATTRIBUTE_READ_ONLY	(1 << ATTRIBUTE_READ_ONLY_BIT)
#define	ATTRIBUTE_SYSTEM	(1 << ATTRIBUTE_SYSTEM_BIT)
#define	ATTRIBUTE_HIDDEN	(1 << ATTRIBUTE_HIDDEN_BIT)
#define	ATTRIBUTE_DIRECTORY	(1 << ATTRIBUTE_DIRECTORY_BIT)
#define	ATTRIBUTE_VOLUME	(1 << ATTRIBUTE_VOLUME_BIT)

#define NORMAL_MASK		(ATTRIBUTE_SYSTEM | ATTRIBUTE_HIDDEN | ATTRIBUTE_DIRECTORY | ATTRIBUTE_VOLUME)
#define NORMAL_FILE		(0)
#define NORMAL_DIRECTORY	(ATTRIBUTE_DIRECTORY)

typedef struct
{
    FSIndex		fs;
    ClusterIndex	head;
    ClusterIndex	current;
} Node;

typedef struct
{
    Node		node;

    DirEntry		entry;
    uint16		current_entry;
    bool		initialized;
} Directory;

typedef struct
{
    Node		node;

    uint32		cluster_position;
    uint32		position;
    uint32		length;
    bool		initialized;
} File;

FileSystem	_file_system[FS_MAX_FILE_SYSTEMS];
Directory	_directory[FS_MAX_OPEN_DIRS];
File		_file[FS_MAX_OPEN_FILES];

/*********************************************************************************************************************/
static bool power_of_two(uint16 value)
{
    return ((value & (value - 1)) == 0);
}
/*********************************************************************************************************************/
Error fs_init(FSIndex fs, BlockIndex block)
{
    uint16	root_directory_entries;

    CheckB(fs < FS_MAX_FILE_SYSTEMS);
    CheckB(_file_system[fs].initialized == false);

    Check(buffer_set_address(block, 0));

    CheckB(buffer_uint16(0x01fe) == SIGNATURE);
    CheckB(buffer_uint16(0x000b) == BYTES_PER_SECTOR);
    CheckB(buffer_uint8 (0x0010) == FAT_COUNT);

    _file_system[fs].sectors_per_cluster = buffer_uint8(0x000d);
    _file_system[fs].reserved            = buffer_uint16(0x000e);

    CheckB(power_of_two(_file_system[fs].sectors_per_cluster));

    /*
     * Simple detection of FAT16 vs FAT32 filesystem.
     */
    root_directory_entries = buffer_uint16(0x0011);

    if (root_directory_entries == ROOT_DIRECTORY_ENTRIES)
    {
	_file_system[fs].type                   = fs_type_fat16;
	_file_system[fs].sectors_per_fat        = buffer_uint16(0x0016);
	_file_system[fs].fat_offset[0]          = _file_system[fs].reserved;
	_file_system[fs].fat_offset[1]          = _file_system[fs].fat_offset[0] + _file_system[fs].sectors_per_fat;
	_file_system[fs].sub.fat16.root_offset  = _file_system[fs].fat_offset[1] + _file_system[fs].sectors_per_fat;
	_file_system[fs].data_offset            = _file_system[fs].sub.fat16.root_offset + (ROOT_DIRECTORY_ENTRIES / ENTRIES_PER_SECTOR);
	_file_system[fs].current_directory      = 0x0000;
    }
    else if (root_directory_entries == 0)
    {
	_file_system[fs].type                   = fs_type_fat32;
	_file_system[fs].sub.fat32.root_cluster = buffer_uint32(0x002c);
	_file_system[fs].sectors_per_fat        = buffer_uint32(0x0024);
	_file_system[fs].fat_offset[0]          = _file_system[fs].reserved;
	_file_system[fs].fat_offset[1]          = _file_system[fs].fat_offset[0] + _file_system[fs].sectors_per_fat;
	_file_system[fs].data_offset            = _file_system[fs].fat_offset[1] + _file_system[fs].sectors_per_fat;
	_file_system[fs].current_directory      = _file_system[fs].sub.fat32.root_cluster;
    }
    else
    {
	Check(failure);
    }

    _file_system[fs].block       = block;
    _file_system[fs].initialized = true;

    return success;
}
/*********************************************************************************************************************/
Error fs_get(FSIndex fs, FileSystem **file_system)
{
    CheckB(fs < FS_MAX_FILE_SYSTEMS);
    CheckB(_file_system[fs].initialized);
    CheckB(file_system != null);

    *file_system = _file_system + fs;

    return success;
}
/*********************************************************************************************************************/
static Error fs_set_cluster(FSIndex fs, ClusterIndex index)
{
    BlockIndex	block = _file_system[fs].block;
    FSType	type  = _file_system[fs].type;

    if (index == 0)
    {
	if (type == fs_type_fat16)
	    Check(buffer_set_address(block, _file_system[fs].sub.fat16.root_offset));
	else
	    Check(buffer_set_address(block, (_file_system[fs].data_offset +
					     (_file_system[fs].sub.fat32.root_cluster *
					      _file_system[fs].sectors_per_cluster))));
    }
    else if (index == 1)
    {
	Check(failure);
    }
    else if ((type == fs_type_fat16 && index < 0xfff0) ||
	     (type == fs_type_fat32 && index < 0xffffff0))
    {
	SectorIndex	sector_offset = (index - 2) * _file_system[fs].sectors_per_cluster;

	Check(buffer_set_address(block, _file_system[fs].data_offset + sector_offset));
    }
    else
    {
	Check(failure);
    }

    return success;
}
/*********************************************************************************************************************/
static ClusterIndex fs_next_cluster(FSIndex fs, ClusterIndex index)
{
    Error		check_error = success;
    BlockIndex		block       = _file_system[fs].block;
    FSType		type        = _file_system[fs].type;
    uint32		address     = _file_system[fs].fat_offset[0];
    ClusterIndex	new_index;

    switch (type)
    {
	case fs_type_fat16:
	    address += index * 2 / BYTES_PER_BLOCK;
	    CheckCleanup(buffer_set_address(block, address), failure);
	    new_index = buffer_uint16((index & 0xff) * 2);
	    break;

	case fs_type_fat32:
	    address += index * 4 / BYTES_PER_BLOCK;
	    CheckCleanup(buffer_set_address(block, address), failure);
	    new_index = buffer_uint32((index & 0x7f) * 4);
	    break;

	default:
	    Check(failure);
    }

    return new_index;

  failure:
    return 0xffffffff;
}
/*********************************************************************************************************************/
Error fs_cd(FSIndex fs, DirEntry *entry)
{
    CheckB(fs < FS_MAX_FILE_SYSTEMS);
    CheckB(_file_system[fs].initialized);
    CheckB(entry != null);
    CheckB(entry->type == dir_type_directory);

    _file_system[fs].current_directory = entry->cluster_head;

    return success;
}
/*********************************************************************************************************************/
Error fs_file_open(FSIndex fs, DirEntry *entry, FileIndex *file)
{
    CheckB(fs < FS_MAX_FILE_SYSTEMS);
    CheckB(_file_system[fs].initialized);
    CheckB(file != null);

    FileIndex	index;

    for (index = 0; index < FS_MAX_OPEN_FILES; ++index)
    {
	if (_file[index].initialized == false)
	{
	    _file[index].node.fs          = fs;
	    _file[index].node.head        = entry->cluster_head;
	    _file[index].node.current     = entry->cluster_head;
	    _file[index].cluster_position = 0;
	    _file[index].position         = 0;
	    _file[index].length           = entry->length;
	    _file[index].initialized      = true;

	    *file = index;

	    return success;
	}
    }

    Check(failure);
    return failure;
}
/*********************************************************************************************************************/
Error fs_file_sync(FileIndex file)
{
    buffer_sync();

    return success;
}
/*********************************************************************************************************************/
Error fs_file_pos(FileIndex file, uint32 *position)
{
    CheckB(file < FS_MAX_OPEN_DIRS);
    CheckB(_file[file].initialized);
    CheckB(position != null);

    *position = _file[file].position;

    return success;
}
/*********************************************************************************************************************/
Error fs_file_close(FileIndex file)
{
    CheckB(file < FS_MAX_OPEN_DIRS);
    CheckB(_file[file].initialized);

    buffer_sync();

    _file[file].initialized = false;

    return success;
}
/*********************************************************************************************************************/
Error fs_file_seek(FileIndex file, int32 offset, SeekType seek_type)
{
    CheckB(file < FS_MAX_OPEN_DIRS);
    CheckB(_file[file].initialized);

    FSIndex	fs                = _file[file].node.fs;
    uint32	bytes_per_cluster = ((uint32) _file_system[fs].sectors_per_cluster) * BYTES_PER_SECTOR;
    uint32	position          = 0;

    /*
     * Make sure offset is not the most negative number.  If it is then our test below would fail.
     */
    CheckB(offset != (int32)0x80000000);

    switch (seek_type)
    {
	case seek_type_beginning:
	    CheckB(offset >= 0);
	    position = offset;
	    break;

	case seek_type_current:
	    CheckB((offset >= 0) || ((uint32) -offset) < _file[file].position);
	    position = _file[file].position + offset;
	    break;

	case seek_type_end:
	    CheckB((offset >= 0) || ((uint32) -offset) < _file[file].length);
	    position = _file[file].length + offset;
	    break;
    }

    CheckB(position >= 0);
    CheckB(position < _file[file].length);

    {
	uint32		remainder;
	ClusterIndex	current;
	uint32		bytes;

	/*
	 * If the new file position is before the current file point then we need to start from
	 * the beginning of the file and work forward.
	 */
	if (position < _file[file].position)
	{
	    remainder = 0;
	    current   = _file[file].node.head;
	    bytes     = position;
	}
	else
	{
	    remainder = _file[file].cluster_position;
	    current   = _file[file].node.current;
	    bytes     = position - _file[file].position;
	}

	for (; bytes >= bytes_per_cluster; bytes -= bytes_per_cluster)
	    current = fs_next_cluster(fs, current);

	remainder += bytes;

	if (remainder >= bytes_per_cluster)
	{
	    remainder -= bytes_per_cluster;
	    current = fs_next_cluster(fs, current);
	}

	CheckB(remainder < bytes_per_cluster);

	_file[file].node.current     = current;
	_file[file].cluster_position = remainder;
	_file[file].position         = position;
    }

    return success;
}
/*********************************************************************************************************************/
Error fs_file_read(FileIndex file, void *data, uint length, uint *read)
{
    CheckB(file < FS_MAX_OPEN_DIRS);
    CheckB(_file[file].initialized);
    CheckB(data != null);

    FSIndex	fs                = _file[file].node.fs;
    uint32	bytes_per_cluster = ((uint32) _file_system[fs].sectors_per_cluster) * BYTES_PER_SECTOR;
    uint	_read             = 0;
    uint8	*_data		  = (uint8 *) data;

    if (read)
	*read = 0;

    while (length > 0)
    {
	/*
	 * If we are at the end of the file then report this to the user.
	 */
	if (_file[file].position == _file[file].length)
	    Check(end_of_file);

	/*
	 * If we are at the last cluster of the file then return end of file.
	 */
	if ((_file_system[fs].type == fs_type_fat16 && _file[file].node.current >= 0xfff8) ||
	    (_file_system[fs].type == fs_type_fat32 && _file[file].node.current >= 0xfffffff8))
	    Check(end_of_file);

	/*
	 * Compute the number of bytes to read from the current cluster.
	 */
	uint32	bytes = bytes_per_cluster - _file[file].cluster_position;

	if (bytes > length)
	    bytes = length;

	if (bytes > (_file[file].length - _file[file].position))
	    bytes = (_file[file].length - _file[file].position);

	/*
	 * Read up to a full clusters worth of data.
	 */
	Check(fs_set_cluster(fs, _file[file].node.current));

	for (uint i = 0; i < bytes; ++i)
	    *(_data++) = buffer_uint8(_file[file].cluster_position + i);

	/*
	 * Update the counts with the read bytes.
	 */
	length                       -= bytes;
	_read                        += bytes;
	_file[file].position         += bytes;
	_file[file].cluster_position += bytes;

	/*
	 * Move to the next cluster of the file.
	 */
	if (length > 0)
	{
	    _file[file].node.current     = fs_next_cluster(fs, _file[file].node.current);
	    _file[file].cluster_position = 0;
	}

	if (read)
	    *read = _read;
    }

    return success;
}
/*********************************************************************************************************************/
Error fs_file_write(FileIndex file, const void *data, uint length, uint *written)
{
    CheckB(file < FS_MAX_OPEN_DIRS);
    CheckB(_file[file].initialized);
    CheckB(data != null);

    FSIndex	fs                = _file[file].node.fs;
    uint32	bytes_per_cluster = ((uint32) _file_system[fs].sectors_per_cluster) * BYTES_PER_SECTOR;
    uint	_written          = 0;
    const uint8	*_data            = (const uint8 *) data;

    while (length > 0)
    {
	/*
	 * If we are at the end of the file return end_of_file.
	 */
	if (_file[file].position == _file[file].length)
	    Check(end_of_file);

	/*
	 * If we are at the last cluster of the file then return end of file.
	 */
	if ((_file_system[fs].type == fs_type_fat16 && _file[file].node.current >= 0xfff8) ||
	    (_file_system[fs].type == fs_type_fat32 && _file[file].node.current >= 0x0ffffff8))
	    Check(end_of_file);

	/*
	 * Compute the number of bytes to write to the current cluster.
	 */
	uint32	bytes = bytes_per_cluster - _file[file].cluster_position;

	if (bytes > length)
	    bytes = length;

	if (bytes > (_file[file].length - _file[file].position))
	    bytes = (_file[file].length - _file[file].position);

	/*
	 * Write up to a full clusters worth of data.
	 */
	Check(fs_set_cluster(fs, _file[file].node.current));

	for (uint i = 0; i < bytes; ++i)
	    Check(buffer_set_uint8(_file[file].cluster_position + i, *(_data++)));

	/*
	 * Update the counts with the bytes written.
	 */
	length                       -= bytes;
	_written                     += bytes;
	_file[file].position         += bytes;
	_file[file].cluster_position += bytes;

	/*
	 * Move to the next cluster of the file.
	 */
	if (length > 0)
	{
	    _file[file].node.current     = fs_next_cluster(fs, _file[file].node.current);
	    _file[file].cluster_position = 0;
	}
    }

    if (written)
	*written = _written;

    return success;
}
/*********************************************************************************************************************/
Error fs_dir_open(FSIndex fs, DirIndex *dir)
{
    CheckB(fs < FS_MAX_FILE_SYSTEMS);
    CheckB(_file_system[fs].initialized);
    CheckB(dir != null);

    DirIndex	index;

    for (index = 0; index < FS_MAX_OPEN_DIRS; ++index)
    {
	if (_directory[index].initialized == false)
	{
	    _directory[index].node.fs         = fs;
	    _directory[index].node.head       = _file_system[fs].current_directory;
	    _directory[index].node.current    = _file_system[fs].current_directory;
	    _directory[index].current_entry   = 0;
	    _directory[index].initialized     = true;

	    *dir = index;

	    return success;
	}
    }

    return failure;
}
/*********************************************************************************************************************/
Error fs_dir_close(DirIndex dir)
{
    CheckB(dir < FS_MAX_OPEN_DIRS);
    CheckB(_directory[dir].initialized);

    buffer_sync();

    _directory[dir].initialized = false;

    return success;
}
/*********************************************************************************************************************/
Error fs_dir_reset(DirIndex dir)
{
    CheckB(dir < FS_MAX_OPEN_DIRS);
    CheckB(_directory[dir].initialized);

    _directory[dir].node.current  = _directory[dir].node.head;
    _directory[dir].current_entry = 0;

    return success;
}
/*********************************************************************************************************************/
static void fs_dir_read_name(char *name, uint16 offset)
{
    uint	write_index = 0;
    uint8	first_byte  = buffer_uint8(offset);

    if (first_byte == 0x05)
	first_byte = 0xe5;

    name[write_index++] = first_byte;

    for (uint read = 1; read < 8; ++read)
    {
	name[write_index] = buffer_uint8(offset + read);
	if (name[write_index] != ' ') ++write_index;
    }

    name[write_index++] = '.';

    for (uint8 read = 9; read < 12; ++read)
    {
	name[write_index] = buffer_uint8(offset + read - 1);
	if (name[write_index] != ' ') ++write_index;
    }

    name[write_index] = '\0';

    if (name[write_index - 1] == '.') name[write_index - 1] = '\0';
}
/*********************************************************************************************************************/
DirEntry *fs_dir_read(DirIndex dir)
{
    Error	check_error = success;

    CheckCleanupB(dir < FS_MAX_OPEN_DIRS, failure);
    CheckCleanupB(_directory[dir].initialized, failure);

    FSIndex	fs = _directory[dir].node.fs;

    CheckCleanupB(fs < FS_MAX_FILE_SYSTEMS, failure);
    CheckCleanupB(_file_system[fs].initialized, failure);
    CheckCleanup(fs_set_cluster(fs, _directory[dir].node.current), failure);

    /*
     * FIX FIX FIX: obviously this should read the cluster linked list.  For now just read the
     * first few entries.
     */
    uint16	max_entries = ROOT_DIRECTORY_ENTRIES;

    if (max_entries > (_file_system[fs].sectors_per_cluster * ENTRIES_PER_SECTOR))
	max_entries = (_file_system[fs].sectors_per_cluster * ENTRIES_PER_SECTOR);

    for (; _directory[dir].current_entry < max_entries; _directory[dir].current_entry++)
    {
	uint16	offset     = _directory[dir].current_entry * BYTES_PER_ENTRY;
	uint8	first_byte = buffer_uint8(offset);
	uint8	attributes = buffer_uint8(offset + 0x0b);
	bool	file       = ((attributes & NORMAL_MASK) == NORMAL_FILE);
	bool	directory  = ((attributes & NORMAL_MASK) == NORMAL_DIRECTORY);

	if (first_byte == 0x00) return null;
	if (first_byte == 0xe5) continue;

	if (file || directory)
	{
	    _directory[dir].entry.type         = file ? dir_type_regular : dir_type_directory;
	    _directory[dir].entry.length       = buffer_uint32(offset + 0x1c);
	    _directory[dir].entry.cluster_head = buffer_uint16(offset + 0x1a);

	    if (_file_system[fs].type == fs_type_fat32)
		_directory[dir].entry.cluster_head |= ((uint32) buffer_uint16(offset + 0x14)) << 16;

	    /*
	     * A cluster of zero in FAT32 land needs to be translated to the actual root directories
	     * first cluster.
	     */
	    if ((_file_system[fs].type == fs_type_fat32) &&
		(_directory[dir].entry.cluster_head == 0))
		_directory[dir].entry.cluster_head = _file_system[fs].sub.fat32.root_cluster;

	    /*
	     * Read the filename.
	     */
	    fs_dir_read_name(_directory[dir].entry.name, offset);

	    /*
	     * Move to the next entry in the directory and return.
	     */
	    _directory[dir].current_entry++;

	    return &(_directory[dir].entry);
	}
    }

  failure:
    return null;
}
/*********************************************************************************************************************/
DirEntry *fs_dir_lookup(DirIndex dir, const char *name)
{
    Error	check_error = success;

    CheckCleanup(fs_dir_reset(dir), failure);

    for (DirEntry *current = fs_dir_read(dir); current != null; current = fs_dir_read(dir))
	if (strncasecmp(name, current->name, FS_MAX_NAME_LENGTH) == 0)
	    return current;

  failure:
    return null;
}
/*********************************************************************************************************************/
static Error fs_module_init()
{
    return fs_init(0, 1);
}
/*********************************************************************************************************************/
const BootModule boot_module_file_system PROGMEM =
{
    fs_module_init
};
/*********************************************************************************************************************/
