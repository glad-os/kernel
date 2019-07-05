/**
 * Copyright 2019 AbbeyCatUK
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */



#include "fat32.h"
#include "emmc.h"
#include "video.h"
#include "stdlib.h"
#include "process.h"



struct _kernel_fat32_mbr_partition_entry {
	unsigned char 		boot_flag;
	unsigned char 		chs_begin[ 3 ];
	unsigned char 		type_code;
	unsigned char 		chs_end[ 3 ];
	unsigned int 		lba_begin;
	unsigned int 		sectors;
};



/**
 * pi3 - is showing an immediate (default) dislike of unaligned accesses whereas pi2 seemed fine
 * access to the packed reserved_sectors (offset 3!) was clearly unhappy resulting in address of volume struct being returned, as if the ldrh/uxth were being 'ignored'
 * one to look into into more detail but for now, rather than blindly copy the 37 bytes, let's manually 'unpack' the FAT32 volume structure into a nicely aligned structure for ARM
 */
struct _kernel_fat32_volume {
	unsigned int		bytes_per_sector;
	unsigned char 		sectors_per_cluster;
	unsigned int 		reserved_sectors;
	unsigned char 		number_of_fats;
	unsigned int 		sectors_per_fat;
	unsigned int 		root_dir_first_cluster;
};

struct __attribute__((__packed__)) _kernel_fat32_directory_entry {
	unsigned char 		filename[ 11 ];
	unsigned char 		attribute;
	unsigned char 		_reserved_1[ 8 ];
	unsigned short int 	cluster_hi;
	unsigned int 		_reserved_2;
	unsigned short int 	cluster_lo;
	unsigned int 		size;
};

void 			_kernel_fat32_select_partition					( unsigned int n );
unsigned char	*_kernel_fat32_format_filename					( unsigned char *f );
void 			_kernel_fat32_file_load_contents				( unsigned int cluster, unsigned int size, unsigned char *dest );
unsigned int 	 _kernel_fat32_directory_entry_attribute		( struct _kernel_fat32_directory_entry d, unsigned int bits );
unsigned int 	 _kernel_fat32_directory_entry_is_volume		( struct _kernel_fat32_directory_entry d );
unsigned int 	 _kernel_fat32_directory_entry_is_directory		( struct _kernel_fat32_directory_entry d );
unsigned int 	 _kernel_fat32_directory_entry_is_file			( struct _kernel_fat32_directory_entry d );
unsigned int 	 _kernel_fat32_next_directory_entry				( struct _kernel_fat32_directory_entry *d );
void 			 _kernel_fat32_set_error						( enum _kernel_fat32_error error );
unsigned int 	 _kernel_fat32_directory_entry_is_hidden		( struct _kernel_fat32_directory_entry d );
unsigned int 	 _kernel_fat32_directory_entry_is_long_filename	( struct _kernel_fat32_directory_entry d );
void		 	 _kernel_fat32_next_cluster						( unsigned int *current_cluster, unsigned int *current_sector );

unsigned char								sector_buffer[ _KERNEL_FAT32_IDE_SECTOR_SIZE ],		/* general purpose storage for a single sector content */
											filename[ 13 ]										/* 8.3.[0x00] */;
struct _kernel_fat32_mbr_partition_entry	partition;
struct _kernel_fat32_volume 				volume;
unsigned int								first_sector_of_fat,
											first_sector_of_clusters,
											current_partition,
											directory_current_cluster,							/* relative to the start of the clusters */
											directory_current_sector,							/* relative to the start of the directory_current_cluster  */
											sector_offset,
											completed;
const char 									*_kernel_fat32_error_message[] = { "", "Invalid partition", "Invalid volume" };
enum _kernel_fat32_error					_kernel_fat32_error_state;



/**
 *
 * _kernel_fat32_set_error
 *
 * Sets error state (and echoes as such on-screen).
 *
 */
void _kernel_fat32_set_error( enum _kernel_fat32_error error ) {

	_kernel_fat32_error_state = error;
	_kernel_video_print_string( "[FAT32] ERROR\n" );

}



/**
 *
 * _kernel_fat32_init
 *
 * Initialisation process for FAT32 subsystem.
 *
 */
void _kernel_fat32_init( void ) {

	unsigned short int *signature;

	_kernel_fat32_error_state = 0;

	// by default, select partition 0
	_kernel_fat32_select_partition( 0 );

	// read and validate the partition
	_kernel_emmc_read_block( 0, sector_buffer );
	signature = (unsigned short int *) ( sector_buffer + _KERNEL_FAT32_IDE_SECTOR_SIZE - 2 );
	if ( *signature != _KERNEL_FAT32_SIGNATURE ) {
		_kernel_video_print_string( "E_ERROR : FAT32 INVALID PARTITION!!!\n" );
		_kernel_fat32_set_error( E_KERNEL_FAT32_INVALID_PARTITION );
		return;
	}
	_kernel_memcpy( &partition, sector_buffer + _KERNEL_FAT32_PARTITION_TABLE_OFFSET + current_partition * _KERNEL_FAT32_PARTITION_ENTRY_SIZE, _KERNEL_FAT32_PARTITION_ENTRY_SIZE );
	// check to ensure the partition selected is actually identified as type FAT32 (or all bets are off right now)
	if ( partition.type_code != 0x0b && partition.type_code != 0x0c ) {
		_kernel_video_print_string( "E_ERROR : PARTITION NOT OF TYPE FAT32\n" );
		_kernel_fat32_set_error( E_KERNEL_FAT32_PARTITION_NOT_FAT32 );
		return;
	}

	// read and validate the volume
	_kernel_emmc_read_block( partition.lba_begin, sector_buffer );
	signature = (unsigned short int *) ( sector_buffer + _KERNEL_FAT32_IDE_SECTOR_SIZE - 2 );
	if ( *signature != _KERNEL_FAT32_SIGNATURE ) {
		_kernel_video_print_string( "E_ERROR : FAT32 INVALID VOLUME!!!\n" );
		_kernel_fat32_set_error( E_KERNEL_FAT32_INVALID_VOLUME );
		return;
	}

	/* this looks like half-word aligned access to reserved_sectors, at offset 3, isn't happy - let's MANUALLY populate the volume struct for now */
	// memcpy( &volume, sector_buffer + 0x0b, sizeof( volume ) );
	volume.bytes_per_sector			= sector_buffer[0x0b +  0] + (sector_buffer[0x0b + 1] << 8);	// unsigned short int
	volume.sectors_per_cluster		= sector_buffer[0x0b +  2];							// unsigned char
	volume.reserved_sectors			= sector_buffer[0x0b +  3] + (sector_buffer[0x0b + 4] << 8);	// unsigned short int
	volume.number_of_fats			= sector_buffer[0x0b +  5];							// unsigned char
	volume.sectors_per_fat			= sector_buffer[0x0b + 25] + (sector_buffer[0x0b + 26] << 8) + (sector_buffer[0x0b + 27] << 16) + (sector_buffer[0x0b + 28] << 24);	// unsigned int
	volume.root_dir_first_cluster	= sector_buffer[0x0b + 33] + (sector_buffer[0x0b + 34] << 8) + (sector_buffer[0x0b + 35] << 16) + (sector_buffer[0x0b + 36] << 24);	// unsigned int

	first_sector_of_fat			= partition.lba_begin + volume.reserved_sectors;
	first_sector_of_clusters	= first_sector_of_fat + volume.number_of_fats * volume.sectors_per_fat;

}



/**
 *
 * _kernel_fat32_select_partition
 *
 * Selects the required FAT32 partition.
 *
 */
void _kernel_fat32_select_partition( unsigned int n ) {

	if ( _kernel_fat32_error_state ) {
		return;
	}

	current_partition = n;

}



/**
 *
 * _kernel_fat32_list_directory
 *
 * Echoes the content of the current directory on-screen.
 *
 */
void _kernel_fat32_list_directory( void ) {

	struct _kernel_fat32_directory_entry d;
	char decimal[20];

	if ( _kernel_fat32_error_state ) {
		return;
	}

	unsigned int count = 0;

	_kernel_fat32_select_directory( (unsigned char *)"$" );
	while ( _kernel_fat32_next_directory_entry( &d ) ) {

		count++;

		// if this isn't a long filename and it isn't something that's hidden...
		if( !_kernel_fat32_directory_entry_is_long_filename( d ) && !_kernel_fat32_directory_entry_is_hidden( d ) ) {

			if ( _kernel_fat32_directory_entry_is_volume( d ) ) {
				_kernel_video_print_string( "VOLUME : " );
				_kernel_video_print_string( (char *) _kernel_fat32_format_filename( d.filename ) );
				_kernel_video_print_string( "\n" );

			} else if ( _kernel_fat32_directory_entry_is_directory( d ) ) {
				_kernel_video_print_string( (char *) _kernel_fat32_format_filename( d.filename ) );
				_kernel_video_print_string( " [DIR]\n" );

			} else {
				_kernel_sprintf_i( decimal, d.size );
				_kernel_video_print_string( (char *) _kernel_fat32_format_filename( d.filename ) );
				_kernel_video_print_string( " " );
				_kernel_video_print_string( decimal );
				_kernel_video_print_string( " bytes\n" );
			}

		}

	}

	_kernel_video_print_string( "\n" );

}



/**
 *
 * _kernel_fat32_directory_entry_is_hidden
 *
 * Returns an indication of whether the supplied directory entry is marked as hidden.
 *
 */
unsigned int _kernel_fat32_directory_entry_is_hidden( struct _kernel_fat32_directory_entry d ) {

	return _kernel_fat32_directory_entry_attribute( d, _KERNEL_FAT32_DIRECTORY_ENTRY_HIDDEN );

}



/**
 *
 * _kernel_fat32_directory_entry_is_long_filename
 *
 * Returns an indication of whether the supplied directory entry is marked as a long filename.
 *
 */
unsigned int _kernel_fat32_directory_entry_is_long_filename( struct _kernel_fat32_directory_entry d ) {

	return _kernel_fat32_directory_entry_attribute( d, _KERNEL_FAT32_DIRECTORY_ENTRY_HIDDEN );

}



/**
 *
 * _kernel_fat32_directory_entry_is_volume
 *
 * Returns an indication of whether the supplied directory entry is marked as a volume.
 *
 */
unsigned int _kernel_fat32_directory_entry_is_volume( struct _kernel_fat32_directory_entry d ) {

	return _kernel_fat32_directory_entry_attribute( d, _KERNEL_FAT32_DIRECTORY_ENTRY_VOLUME_NAME );

}



/**
 *
 * _kernel_fat32_directory_entry_is_file
 *
 * Returns an indication of whether the supplied directory entry is marked as a file.
 *
 */
unsigned int _kernel_fat32_directory_entry_is_file( struct _kernel_fat32_directory_entry d ) {

	return !_kernel_fat32_directory_entry_is_volume( d ) && !_kernel_fat32_directory_entry_is_directory( d );

}



/**
 *
 * _kernel_fat32_directory_entry_is_directory
 *
 * Returns an indication of whether the supplied directory entry is marked as a directory.
 *
 */
unsigned int _kernel_fat32_directory_entry_is_directory( struct _kernel_fat32_directory_entry d ) {

	return _kernel_fat32_directory_entry_attribute( d, _KERNEL_FAT32_DIRECTORY_ENTRY_DIRECTORY_NAME );

}



/**
 *
 * _kernel_fat32_directory_entry_attribute
 *
 * Returns an indication of whether the supplied directory entry matches the supplied attribute bitfield.
 *
 */
unsigned int _kernel_fat32_directory_entry_attribute( struct _kernel_fat32_directory_entry d, unsigned int bits ) {

	return ( d.attribute & bits ) == bits;

}



/**
 *
 * _kernel_fat32_format_filename
 *
 * Formats a filename into the standard 8.3 format, ready for display on-screen if required.
 *
 */
unsigned char *_kernel_fat32_format_filename( unsigned char *f ) {

	unsigned int src_index, dest_index;

	src_index = dest_index = 0;

	// 8
	while ( f[ src_index ] != ' ' && src_index < 8 ) {
		filename[ dest_index++ ] = f[ src_index++ ];
	}

	// 3
	src_index = 8;
	if ( f[ src_index ] != 0 && f[ src_index ] != ' ' ) {
		filename[ dest_index++ ] = '.';
		filename[ dest_index++ ] = f[ src_index++ ];
		filename[ dest_index++ ] = f[ src_index++ ];
		filename[ dest_index++ ] = f[ src_index++ ];
	}

	filename[ dest_index++ ] = 0;
	return filename;

}



/**
 *
 * _kernel_fat32_select_directory
 *
 * Selects the appropriate directory ready for use (for now, just assumes root directory).
 *
 */
void _kernel_fat32_select_directory( unsigned char *directory ) {

	if ( _kernel_fat32_error_state ) {
		_kernel_video_print_string( "ERROR STATE - CANNOT SELECT DIRECTORY\n" );
		return;
	}

	// for now just assume it's root only
	directory_current_cluster = volume.root_dir_first_cluster;
	directory_current_sector  = 0;

	// transform the direct_current pairing to a physical sector
	unsigned int actual_sector;

	actual_sector = first_sector_of_clusters + ( ( directory_current_cluster - 2 ) * volume.sectors_per_cluster ) + directory_current_sector;
	_kernel_emmc_read_block( actual_sector, sector_buffer );

	sector_offset = 0;
	completed = 0;

}



/**
 *
 * _kernel_fat32_next_directory_entry
 *
 * Reads the next directory entry into the supplied directory entry structure.
 *
 */
unsigned int _kernel_fat32_next_directory_entry( struct _kernel_fat32_directory_entry *d ) {

	if ( completed ) {
		return 0;
	}

	_kernel_memcpy( d, sector_buffer + sector_offset, sizeof( *d ) );

	sector_offset += sizeof( *d );
	if ( sector_offset > _KERNEL_FAT32_IDE_SECTOR_SIZE ) {

		// exhausted this sector  - where next? next sector, or new cluster?
		directory_current_sector++;
		if ( directory_current_sector >= volume.sectors_per_cluster ) {
			_kernel_fat32_next_cluster( &directory_current_cluster, &directory_current_sector );
		}

		// transform the direct_current pairing to a physical sector
		unsigned int actual_sector;
		actual_sector = first_sector_of_clusters + ( ( directory_current_cluster - 2 ) * volume.sectors_per_cluster ) + directory_current_sector;
		_kernel_emmc_read_block( actual_sector, sector_buffer );

		// and now retry!
        sector_offset = 0;
        _kernel_memcpy( d, sector_buffer + sector_offset, sizeof( *d ) );
        sector_offset += sizeof( *d );

	}

	switch ( d->filename[ 0 ] ) {
		case _KERNEL_FAT32_DIRECTORY_ENTRY_TYPE_FINAL:
			completed = 1;
			return 0;
			break;
		case _KERNEL_FAT32_DIRECTORY_ENTRY_TYPE_UNUSED:
			return _kernel_fat32_next_directory_entry( d );
			break;
	}

	return 1;

}



/**
 *
 * _kernel_fat32_next_cluster
 *
 * Updates to point at the next cluster.
 *
 */
void _kernel_fat32_next_cluster( unsigned int *current_cluster, unsigned int *current_sector ) {

	unsigned int fat_sector, fat_sector_index, next_cluster;
	unsigned char temp_sector_buffer[ _KERNEL_FAT32_IDE_SECTOR_SIZE ];

	fat_sector = ( *current_cluster >> 7 ) /** volume.sectors_per_cluster*/;					// bits 7-31 of current cluster identify which FAT sector to look to
	fat_sector_index = ( (*current_cluster) & 0b1111111 );									// bits 0-6  of current cluster identify which 32-bit word in the FAT sector

	_kernel_emmc_read_block( first_sector_of_fat + fat_sector, temp_sector_buffer );

	next_cluster  = *( temp_sector_buffer + fat_sector_index * 4 + 0 );
	next_cluster += *( temp_sector_buffer + fat_sector_index * 4 + 1 ) <<  8;
	next_cluster += *( temp_sector_buffer + fat_sector_index * 4 + 2 ) << 16;
	next_cluster += *( temp_sector_buffer + fat_sector_index * 4 + 3 ) << 24;

	// update our references
	*current_cluster = next_cluster;
	*current_sector  = 0;

}



/**
 *
 * _kernel_fat32_load_file
 *
 * Loads the specified filename's content into the specified memory.
 *
 */
void _kernel_fat32_load_file( char *filename, unsigned char *dest ) {

	struct _kernel_fat32_directory_entry d;

	if ( _kernel_fat32_error_state ) {
		_kernel_video_print_string( "FAT32 ERROR STATE\n" );
		return;
	}

	// _kernel_video_print_string( "Select dir\n" );
	_kernel_fat32_select_directory( (unsigned char *)"$" );
	while ( _kernel_fat32_next_directory_entry( &d ) ) {

		// _kernel_video_print_string( "Check dir entry\n" );
		if ( _kernel_fat32_directory_entry_is_file( d ) ) {

			if ( !_kernel_strcmp( (const char *)_kernel_fat32_format_filename( d.filename ), (const char *)filename ) ) {
				// _kernel_video_print_string( "Loading file contents with " ); _kernel_video_print_hex( d.size ); _kernel_video_print_string( "\n" );
				_kernel_fat32_file_load_contents( ( d.cluster_hi << 16 ) + d.cluster_lo, d.size, dest );
				return;
			}
		}

	}

	_kernel_video_print_string( "_kernel_fat32_load_file exits with NO LOAD\n" );

}



/**
 *
 * _kernel_fat32_file_load_contents
 *
 * Loads the specified cluster content (taking into account size) into the specified memory.
 *
 */
void _kernel_fat32_file_load_contents( unsigned int cluster, unsigned int size, unsigned char *dest ) {

	unsigned int sector, available, remaining, actual_sector, i;
	unsigned char b[ _KERNEL_FAT32_IDE_SECTOR_SIZE + 1 ]; // extra 1 in case data fits in EXACTLY 512 (in which case 0x00 required to be added)

	// always starts off at the beginning of the cluster
	sector = 0;

	// transform the direct_current pairing to a physical sector
	actual_sector = first_sector_of_clusters + ( ( cluster - 2 ) * volume.sectors_per_cluster ) + sector;
	// _kernel_video_print_string( "Read Sec1 " ); _kernel_video_print_hex( actual_sector );
	// _kernel_video_print_string( " into 0x" ); _kernel_video_print_hex( (unsigned int) &b ); _kernel_video_print_string( "\n" );
	_kernel_emmc_read_block( actual_sector, b );
	available = _KERNEL_FAT32_IDE_SECTOR_SIZE;

	unsigned int read_so_far = 0;

	remaining = size;
	while ( remaining ) {

		// _kernel_video_print_string( "Remaining : 0x" ); _kernel_video_print_hex( remaining ); _kernel_video_print_string( "\n" );
		if ( remaining <= available ) {
			read_so_far += remaining;
			i = 0; while ( i < remaining ) { *dest++ = b[ i++ ]; } /* copy to dest */
			*dest = 0x00; // null terminate for now (whilst debugging so I can print easily)
			return;
		}

		read_so_far += available;
		i = 0; while ( i < available ) { *dest++ = b[ i++ ]; } /* copy to dest */
		remaining -= available;

		// exhausted this sector  - where next? next sector, or new cluster?
		sector++;
		if ( sector >= volume.sectors_per_cluster ) {
			// _kernel_video_print_string( "Next cluster required\n" );
			_kernel_fat32_next_cluster( &cluster, &sector );
		}

		// transform the direct_current pairing to a physical sector
		// _kernel_video_print_string( "Transform\n" );
		actual_sector = first_sector_of_clusters + ( ( cluster - 2 ) * volume.sectors_per_cluster ) + sector;
		// _kernel_video_print_string( "Read block\n" );
		_kernel_emmc_read_block( actual_sector, b );
		available = _KERNEL_FAT32_IDE_SECTOR_SIZE;

	}

}
