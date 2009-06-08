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
#ifndef __file_system_h__
#define __file_system_h__

#include "libs/os/os.h"
#include "libs/types/types.h"
#include "libs/error/error.h"
#include "libs/sdcard/sdcard.h"
#include "libs/block/block.h"

#define FS_MAX_FILE_SYSTEMS	1
#define FS_MAX_NAME_LENGTH	13
#define FS_MAX_OPEN_DIRS	2
#define FS_MAX_OPEN_FILES	2

typedef	uint8	FSIndex;
typedef uint8	DirIndex;
typedef uint8	FileIndex;
typedef uint32	SectorIndex;
typedef uint32	ClusterIndex;

typedef enum 
{
    fs_type_fat16,
    fs_type_fat32
} FSType;

typedef struct
{
    SectorIndex		root_offset;
} FAT16;

typedef struct
{
    ClusterIndex	root_cluster;
} FAT32;

typedef struct
{
    FSType		type;
    uint16		sectors_per_cluster;
    uint16		reserved;
    uint32		sectors_per_fat;
    SectorIndex		fat_offset[2];
    SectorIndex		data_offset;

    union
    {
	FAT16		fat16;
	FAT32		fat32;
    } sub;

    ClusterIndex	current_directory;
    BlockIndex		block;
    bool		initialized;
} FileSystem;

typedef enum
{
    dir_type_unknown   = 0,
    dir_type_directory = 1,
    dir_type_regular   = 2
} DirType;

typedef struct
{
    ClusterIndex	cluster_head;
    DirType		type;
    uint32		length;
    char		name[FS_MAX_NAME_LENGTH];
} DirEntry;

typedef enum
{
    seek_type_beginning,
    seek_type_current,
    seek_type_end
} SeekType;

Error fs_init    (FSIndex fs, BlockIndex block);
Error fs_shutdown(FSIndex fs);
Error fs_get     (FSIndex fs, FileSystem **file_system);
Error fs_cd      (FSIndex fs, DirEntry *entry);

Error fs_file_open (FSIndex fs, DirEntry *entry, FileIndex *file);
Error fs_file_close(FileIndex file);
Error fs_file_sync (FileIndex file);
Error fs_file_pos  (FileIndex file, uint32 *position);
Error fs_file_seek (FileIndex file, int32 offset, SeekType seek_type);
Error fs_file_read (FileIndex file, void *data, uint length, uint *read);
Error fs_file_write(FileIndex file, const void *data, uint length, uint *written);

Error fs_dir_open      (FSIndex fs, DirIndex *dir);
Error fs_dir_close     (DirIndex dir);
Error fs_dir_reset     (DirIndex dir);
DirEntry *fs_dir_read  (DirIndex dir);
DirEntry *fs_dir_lookup(DirIndex dir, const char *name);

extern const BootModule boot_module_file_system PROGMEM;

#endif //__file_system_h__
