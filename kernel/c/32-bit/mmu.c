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
#include <stdint.h>



// translation table (bottom 14 bits not used, must be 2^14 (16Kb) aligned for this)
uint32_t translation_table[ 4096 ] __attribute__ ((aligned(16384)));

// flag indicating whether MMU is currently enabled
int mmu_enabled = 0;



/*
 *
 * _kernel_mmu_init
 *
 * Initialises the MMU hardware.
 *
 * Uses the ARM MMU short-descriptor format translation tables, which offers a simpler approach
 * to virtual memory management; using Sections, memory is divided into 1MB blocks. this
 * requires only one level of translation table to achieve.
 *
 */
void _kernel_mmu_init( void )
{

	int i, cache, buffer;
	uint32_t ttbcr, dacr;
	uintptr_t ttbr0;

	/*
	 *
	 * configure TTBCR (Translation Table Base Control Register)
	 * (accessible via CP15 c2 registers)
	 *
	 * - set N=0 (only use TTBR0 for table walks (no need for TTBR1))
	 * - disable (just as a matter of course) table walks using TTBR1
	 *
	 * [ARM DDI 0487A.f / G6-4686] [ARM DDI 0500G / 4-212]
	 *
	 */
	// ARM DDI 0487A.f / G4-4803
	ttbcr = ARM_MMU_TTB_DISABLE_TTBR1_WALKS;
	_kernel_mmu_configure_ttbcr( ttbcr );


	/*
	 *
	 * configure TTBR0 (Translation Table Base Register 0)
	 * (accessible via CP15 c2 registers)
	 *
	 * [ARM DDI 0487A.f / G6-4691] [ARM DDI 0500G / 4-207]
	 *
	 */
	ttbr0 = (uintptr_t) &translation_table | ARM_MMU_INNER_CACHEABLE_WRITE_BACK_WRITE_ALLOCATE | ARM_MMU_OUTER_CACHEABLE_WRITE_THROUGH;
	_kernel_mmu_configure_ttbr0( ttbr0 );


	/*
	 *
	 * configure DACR (Domain Access Control Register)
	 *
	 * for each of the 16 domains, identify the access permission as "Manager"
	 * (implies "Accesses are not checked against the permission bits in the translation tables.")
	 *
	 * [ARM DDI 0500G / 4-219 ]
	 *
	 */
	dacr = 0;
	for ( i = 0; i <= 15; i++ ) {
		dacr += ARM_MMU_DOMAIN_ACCESS_CONTROL_MANAGER << ( i*2 );
	}
	_kernel_mmu_configure_dacr( dacr );

	/*
	 *
	 * populate the translation table
	 * (one entry per 1Mb Section - for now, map VAs to PAs directly)
	 *
	 * [ARM DDI 0487A.f / G4-4080]
	 *
	 */
	for ( i = 0; i < (1 * 1024); i++ ) {

		cache = buffer = 1;

		// memory coherent in page 2 (2Mb), and also from 946Mb+ (where kernel data area resides)
		if ( i ==   2 ) { cache = 0; buffer = 0; }
		if ( i >= 946 ) { cache = 0; buffer = 0; }

		// [ARM DDI 0487A.f / G4-4113]
		_kernel_mmu_map_va_section( i, i, cache, buffer );

	}

	// MMU updated - clear tlb/btc
	// by default, initialisation of the MMU will request that it be enabled
	_kernel_mmu_clear_tlb_and_btc();
	_kernel_mmu_enable();

	// echo state of MMU at point of exit here
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

	uint32_t flags;

	_kernel_mmu_clean_l1_d_cache();
	_kernel_mmu_clean_l2_d_cache();

	// adding branch prediction... and barrier enable...
	flags = ARM_CP15_SCTLR_MMU_ENABLE | ARM_CP15_SCTLR_CACHE_ENABLE | ARM_CP15_SCTLR_INSTRUCTION_CACHE_ENABLE | (1<<11) | (1<<5);
	_kernel_mmu_configure_enable( flags );

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

	uint32_t flags;

	flags = ARM_CP15_SCTLR_MMU_ENABLE | ARM_CP15_SCTLR_CACHE_ENABLE | ARM_CP15_SCTLR_INSTRUCTION_CACHE_ENABLE;
	_kernel_mmu_configure_disable( flags );

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

	// ARM DDI 0487A.f - G4-4079
	// map the section in *each* of the 4 x 1Gb areas
	unsigned int ns = (1<<19);

	translation_table[ (0*1024) + va_section ] = ( ((0*1024)+pa_section ) << 20) | ARM_MMU_TTB_SHORT_ACCESS_FULL_ACCESS | (cache << 3) | (buffer << 2) | ARM_MMU_TTB_ENTRY_TYPE_SECTION|ns;
	translation_table[ (1*1024) + va_section ] = ( ((1*1024)+pa_section ) << 20) | ARM_MMU_TTB_SHORT_ACCESS_FULL_ACCESS | (cache << 3) | (buffer << 2) | ARM_MMU_TTB_ENTRY_TYPE_SECTION|ns;
	translation_table[ (2*1024) + va_section ] = ( ((2*1024)+pa_section ) << 20) | ARM_MMU_TTB_SHORT_ACCESS_FULL_ACCESS | (cache << 3) | (buffer << 2) | ARM_MMU_TTB_ENTRY_TYPE_SECTION|ns;
	translation_table[ (3*1024) + va_section ] = ( ((3*1024)+pa_section ) << 20) | ARM_MMU_TTB_SHORT_ACCESS_FULL_ACCESS | (cache << 3) | (buffer << 2) | ARM_MMU_TTB_ENTRY_TYPE_SECTION|ns;

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
