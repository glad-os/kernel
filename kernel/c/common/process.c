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



// IMP OS manages up to 16 processes for now
process		proc[ MAX_PROCESSES ];
int 		current;
char 		filename_copy[ 16 ];

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

	// @todo consider the appropriate initial value here
	current = 0;

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
	if ( ( slot = _kernel_process_find_free_slot() ) == ERROR_PROCESS_SLOT_UNAVAILABLE ) {
		return slot;
	}

	// claim, record parent id, snapshot parent state
	proc[ slot ].free = 0;
	proc[ slot ].parent = current;
	_kernel_process_push_cpu_state( (unsigned int *) &proc[ slot ].state );

	// update current, map memory in, install binary, start process
	current = slot;
	_kernel_mmu_map_process_in( slot, 0,0 );
	_kernel_fat32_load_file( filename_copy, (unsigned char *) ( 4 * MBYTE ) );
	_kernel_process_start();

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

	unsigned int parent, tmp_current;

	// free slot, map parent in, update current, reinstate process
	proc[ current ].free = 1;

	parent = proc[ current ].parent;

	_kernel_mmu_map_process_in( parent, 1,1 );

	tmp_current = current;
	current = parent;

	unsigned int i;
	i = 0;
	while ( i < 4 ) {
	_kernel_video_print_hex( proc[tmp_current].state.r[i] );
	_kernel_video_print_string( " , " );
	i++;
	}
	_kernel_video_print_string( "\n" );
	_kernel_video_print_string( "Will set ELR_EL1 to 0x" ); _kernel_video_print_hex( proc[tmp_current].state.r[3] ); _kernel_video_print_string( "\n" );

	_kernel_process_pop_cpu_state( (unsigned int *) &proc[ tmp_current ].state );

}
