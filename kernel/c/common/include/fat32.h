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


#include <stdint.h>

#ifndef FAT32_H
#define FAT32_H



	enum _kernel_fat32_error { E_KERNEL_FAT32_INVALID_PARTITION = 1, E_KERNEL_FAT32_INVALID_VOLUME, E_KERNEL_FAT32_PARTITION_NOT_FAT32 };


	void _kernel_fat32_init( void );
	void _kernel_fat32_select_directory( unsigned char *directory );
	void _kernel_fat32_list_directory( void );
	void _kernel_fat32_load_file( char *filename, unsigned char *dest );

	#define		_KERNEL_FAT32_IDE_SECTOR_SIZE					512
	#define		_KERNEL_FAT32_PARTITION_TABLE_OFFSET			446
	#define		_KERNEL_FAT32_PARTITION_ENTRY_SIZE				16
	#define		_KERNEL_FAT32_DIRECTORY_ENTRY_TYPE_FINAL		0x00
	#define		_KERNEL_FAT32_DIRECTORY_ENTRY_TYPE_UNUSED		0xe5
	#define		_KERNEL_FAT32_DIRECTORY_ENTRY_LONG_FILENAME		0b01111
	#define		_KERNEL_FAT32_DIRECTORY_ENTRY_HIDDEN			0b00010
	#define		_KERNEL_FAT32_DIRECTORY_ENTRY_VOLUME_NAME		0b01000
	#define		_KERNEL_FAT32_DIRECTORY_ENTRY_DIRECTORY_NAME	0b10000
	#define		_KERNEL_FAT32_SIGNATURE							0xaa55



#endif /*FAT32_H*/
