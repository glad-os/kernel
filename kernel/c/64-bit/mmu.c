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



#include "const.h"
#include "mmu.h"
#include "start.h"
#include "video.h"
#include "stdlib.h"
#include <stdint.h>



extern uint64_t		_kernel_mmu_tcr_read( void );
extern void		_kernel_mmu_tcr_write( uint64_t value );
extern void		_kernel_mmu_configure_mair( uint64_t value );
extern void		_kernel_mmu_clear_tlb_and_btc( void );
extern void		_kernel_mmu_configure_ttbr0( uint64_t ttbr0 );
extern void		_kernel_mmu_configure_enable( void );
extern void		_kernel_mmu_configure_disable( void );
extern void		_kernel_mmu_invalidate_tlb( void );
extern void		_kernel_mmu_invalidate_btc( void );




// TCR field definitions
bitfield TCR_EL1_IPS	= { 34, 32 };
bitfield TCR_EL1_TG0	= { 15, 14 };
bitfield TCR_EL1_SH0	= { 13, 12 };
bitfield TCR_EL1_ORGN0	= { 11, 10 };
bitfield TCR_EL1_IRGN0	= {  9,  8 };
bitfield TCR_EL1_EPD0	= {  7,  7 };
bitfield TCR_EL1_T0SZ	= {  5,  0 };



/*
 * Level 2 and Level 3 lookups are required for VA->PA translation. 64Kb granule size is used.
 * 
 * Level 2 : resolves [31:29], requiring 2^3  8-byte entries; so a 64-byte table (64-byte aligned) is required
 * Level 3 : resolves [28:16], requiring 2^13 8-byte entries; so a 64Kb    table (64Kb    aligned) is required
 *           8 x Level 3 tables are required, one for each entry identified in the Level 2 table
 */
uint64_t level_2_table [ 8 ]		__attribute__ ( ( aligned( 64    ) ) );
uint64_t level_3_tables[ 8 * 8192 ]	__attribute__ ( ( aligned( 65536 ) ) );

// flag indicating whether MMU is currently enabled
int mmu_enabled = 0;



/*
 *
 * _kernel_mmu_init
 *
 * Initialises the MMU hardware.
 *
 */
void _kernel_mmu_init( void )
{

	uint64_t tcr, page, flags, i, j;

	// populate the translation tables
	for ( i = 0; i < 8; i++ ) {
		// each level 2 entry is a table descriptor, pointing to the relevant level 3 table [D4-1792]
		level_2_table[ i ] = ( (uint64_t) &level_3_tables[ i*8192 ] ) | MMU_TRANS_ENTRY_LEVEL_012_TABLE;
	}
	page = 0;
	for ( i = 0; i < 8; i++ ) {
		for ( j = 0; j < 8192; j++ ) {
			// level 3 entries are page descriptors, pointing to the relevant 64Kb page (944Mb+ = kernel/Pi device)
			flags = MMU_PAGE_DESCRIPTOR_ACCESS_FLAG | MMU_PAGE_DESCRIPTOR_NON_SECURE | MMU_TRANS_ENTRY_LEVEL_3_PAGE;
			if ( page >= ( (944 * MBYTE / 64) ) ) {
				flags |= MMU_PAGE_DESCRIPTOR_OUTER_SHAREABLE | 
					 MMU_PAGE_DESCRIPTOR_ACCESS_EL1_RW_EL0_NONE |
					 MMU_PAGE_DESCRIPTOR_MAIR_3;
			} else {
				flags |= MMU_PAGE_DESCRIPTOR_INNER_SHAREABLE |
					 MMU_PAGE_DESCRIPTOR_ACCESS_EL1_RW_EL0_NONE	| 
					 MMU_PAGE_DESCRIPTOR_MAIR_3;
			}
			level_3_tables[ i * 8192 + j ] = ( page++ << 16 ) | flags ;
		}
	}

	// point the MMU at the top-most translation table
	_kernel_mmu_configure_ttbr0( (uint64_t) level_2_table );

	// configure the MMU via its TCR
	tcr = _kernel_mmu_tcr_read();
	tcr = _kernel_set_flags( tcr, TCR_EL1_T0SZ,	32                                          );
	tcr = _kernel_set_flags( tcr, TCR_EL1_IPS,      MMU_TCR_IPS_40BITS                      );
	tcr = _kernel_set_flags( tcr, TCR_EL1_TG0,      MMU_TCR_TG0_64KB                        );
	tcr = _kernel_set_flags( tcr, TCR_EL1_SH0,      MMU_TCR_SH0_NON_SHAREABLE               );
	tcr = _kernel_set_flags( tcr, TCR_EL1_EPD0,     MMU_TCR_EPD0_ENABLE                     );
	tcr = _kernel_set_flags( tcr, TCR_EL1_ORGN0,	MMU_TCR_ORGN0_NON_CACHEABLE             ); // @todo investigate/understand this stuff better
	tcr = _kernel_set_flags( tcr, TCR_EL1_IRGN0,	MMU_TCR_IRGN0_WRITE_BACK_WRITE_ALLOCATE );
	_kernel_mmu_tcr_write( tcr );
	
	// configure the access permissions for the MMU
	flags = ( MMU_MAIR_DEVICE_NGNRNE                << 24       ) |
            ( MMU_MAIR_NORMAL_OUTER_INNER_NOCACHE   << 16       ) |
            ( MMU_MAIR_DEVICE_NGNRE                 <<  8       ) |
            ( MMU_MAIR_NORMAL_OUTER_INNER_WRITEBACK_RW_ALLOC    );
	_kernel_mmu_configure_mair( flags );

	// enable the MMU
	_kernel_mmu_enable();
	if ( _kernel_mmu_enabled() ) {
		_kernel_video_print_string( "[MMU : ENABLED]\n\n" );
	} else {
		_kernel_video_print_string( "[MMU : NOT ENABLED]\n\n" );
	}

}



/*
 *
 * _kernel_mmu_enable
 *
 * Enables the MMU.
 *
 */
void _kernel_mmu_enable( void )
{

	_kernel_mmu_configure_enable();
	mmu_enabled = 1;

}



/*
 *
 * _kernel_mmu_disable
 *
 * Disables the MMU.
 *
 */
void _kernel_mmu_disable( void )
{

	_kernel_mmu_configure_disable();
	mmu_enabled = 0;

}



/*
 *
 * _kernel_mmu_enabled
 *
 * Identifies whether or not the MMU is currently enabled.
 * Returns an unsigned int (boolean) indicating whether MMU is enabled.
 *
 */
unsigned int _kernel_mmu_enabled( void )
{

	return mmu_enabled;

}



/**
 *
 * _kernel_mmu_map_va_section
 *
 * Maps an individal 1Mb section of virtual memory, to point to the required physical 1Mb of memory.
 * Allows the cache and buffer bits to be recorded so the MMU can know whether this chunk can be cached and/or buffered.
 *
 */
void _kernel_mmu_map_va_section( unsigned int va_section, unsigned int pa_section, int cache, int buffer ) {

	uint64_t i, flags, va_index, pa_index;

	va_index = ( va_section * MBYTE ) / ( 64 * KBYTE );
	pa_index = ( pa_section * MBYTE ) / ( 64 * KBYTE );

	for ( i = 0; i < 16; i++ ) {
		// _kernel_video_print_string( "Mapping " ); _kernel_video_print_hex( va_index ); _kernel_video_print_string( " to " ); _kernel_video_print_hex( pa_index ); _kernel_video_print_string( "\n" );
		// level 3 entries are page descriptors, pointing to the relevant 64Kb page
		flags = MMU_PAGE_DESCRIPTOR_ACCESS_FLAG | MMU_PAGE_DESCRIPTOR_NON_SECURE | MMU_TRANS_ENTRY_LEVEL_3_PAGE;
		flags |= MMU_PAGE_DESCRIPTOR_OUTER_SHAREABLE | 
			 MMU_PAGE_DESCRIPTOR_ACCESS_EL1_RW_EL0_RW |
			 MMU_PAGE_DESCRIPTOR_MAIR_3;
		level_3_tables[ va_index ] = ( pa_index << 16 ) | flags ;
		va_index++;
		pa_index++;
	}

}



/**
 *
 * _kernel_mmu_map_process_in
 *
 * Given a process #n, maps the 4MB-8MB region to point to the physical 4MB 'slot' that the process will have been installed into
 * (e.g. process 0 => 4MB, 1 => 8MB, 2 => 12MB etc., so to map in process #1, VA 4MB-8MB => PA 8MB-12MB).
 * Ensures caches are suitably invalidated as a result
 *
 */
void _kernel_mmu_map_process_in( unsigned int n, unsigned int cache, unsigned int buffer ) {

	unsigned int i, va_section, pa_section;

	// set up memory
	va_section = 4;
	pa_section = 4 + (n * 4);
	for ( i = 0; i < 4; i++ ) {
		_kernel_mmu_map_va_section( va_section + i, pa_section + i, cache, buffer );
	}

	// MMU updated - clear tlb/btc
	_kernel_mmu_clear_tlb_and_btc();

}


