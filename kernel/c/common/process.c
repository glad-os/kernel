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
#include "process.h"
#include "fat32.h"
#include "mmu.h"
#include "start.h"
#include "stdlib.h"
#include "video.h"
#include "cpu-state.h"


// IMP OS manages up to 16 processes for now
process		proc[ MAX_PROCESSES ];
int 		current;
char        filename_copy[ 16 ];


int _kernel_process_find_free_slot( void );


/**
 *
 * _kernel_process_init
 *
 * Initialisation process for process subsystem.
 *
 */
void _kernel_process_init( void )
{

	int i;

	for ( i = 0; i < MAX_PROCESSES; i++ ) {
		proc[ i ].free = 1;
	}

	// @todo consider the appropriate initial values here
	current = 0;
    current_process_state = &proc[ current ].state;

}


/**
 *
 * _kernel_process_find_free_slot
 *
 * Finds the id of a process slot that is currently unused (available)
 * If no process slot is available, returns ERROR_PROCESS_SLOT_UNAVAILABLE
 *
 */
int _kernel_process_find_free_slot( void ) {

	int slot;

	for ( slot = 0; slot < MAX_PROCESSES; slot++ ) {
		if ( proc[ slot ].free ) {
			return slot;
		}
	}

	return ERROR_PROCESS_SLOT_UNAVAILABLE;

}


/**
 *
 * _kernel_process_begin
 *
 * Attempts to install and begin the required process.
 * If successful, the call never formally returns; the process is installed and immediately started.
 * If unsuccessful, the call returns ERROR_PROCESS_SLOT_UNAVAILABLE.
 *
 */
int _kernel_process_begin( char *filename ) {

    int slot;

    // make a copy of the filename before the process is mapped out
    _kernel_strcpy( filename_copy, filename );

    // find process slot
    slot = _kernel_process_find_free_slot();
    if ( slot == ERROR_PROCESS_SLOT_UNAVAILABLE ) {
        return slot;
    }

    // claim, record parent id, initialise cpu state
    proc[ slot ].free   = 0;
    proc[ slot ].parent = current;
    current_process_state = &proc[ slot ].state;
    _kernel_process_init_cpu_state( current_process_state );

    // update current, map memory in, install binary, and officially "continue" the process at this point
    current = slot;

    // currently testing cache/buffer being switched on (invalidate I&D caches completely for now)
    _kernel_mmu_map_process_in( slot, 1,1 );
    _kernel_fat32_load_file( filename_copy, (unsigned char *) ( 4 * MBYTE ) );
    _kernel_mmu_invalidate_i_cache();
	_kernel_mmu_clean_l1_d_cache();
	//_kernel_mmu_clean_l2_d_cache();
    _kernel_process_pop_cpu_state( current_process_state );

    return slot;

}


/**
 *
 * _kernel_process_exit
 *
 * Frees the current process, returning control to the parent process that started it.
 *
 */
void _kernel_process_exit( void ) {

	unsigned int parent;

        // special case - if this is the first process, then the kernel has "finished"
        if ( current == 0 ) {
            _kernel_video_print_string( "[IMP OS] finished" );
            while ( 1 ) {}
        }

	// free the process slot, map the parent back in, update "current", and "continue" the parent process
	proc[ current ].free = 1;
	parent = proc[ current ].parent;

	_kernel_mmu_map_process_in( parent, 1,1 );
	current = parent;
    current_process_state = &proc[ parent ].state;
    _kernel_mmu_invalidate_i_cache();
	_kernel_mmu_clean_l1_d_cache();
	_kernel_process_pop_cpu_state( current_process_state );

}
