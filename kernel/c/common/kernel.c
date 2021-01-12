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
#include "swi.h"

#include "irq.h"
#include "video.h"
#include "mmu.h"
#include "keyboard.h"
#include "systimer.h"
#include "emmc.h"
#include "fat32.h"
#include "process.h"



void _kernel_init( void )
{

	// initialise the various subsystems
	_kernel_interrupt_init();
	_kernel_video_init();
	_kernel_mmu_init();
	_kernel_keyboard_init();
	_kernel_emmc_init();
	_kernel_systimer_init();
	_kernel_fat32_init();
	_kernel_process_init();

}



/**
 *
 * _kernel_panic
 *
 * Displays the panic information, and suspends any further processing.
 *
 */
void _kernel_panic( char *subsystem, char *msg )
{

	_kernel_video_print_string( subsystem );
	_kernel_video_print_string( ": " );
	_kernel_video_print_string( msg );

	// kernel will now panic
	while( 1 );

}



/**
 *
 * install_app
 *
 * Installs the CLI.BIN file by use of OS_ProcessBegin, as the first userspace program to run.
 *
 */
void _kernel_start_cli( void ) {

	char *appname = "CLI.BIN";

        // after this first process is begun, we never return here - only when this first 
        // process calls OS_ProcessExit, will the kernel recognise that the first process 
        // has finished, and will just say "bye" and spin at that point forever: kernel is done!
	_kernel_video_print_string( "[IMP OS] CLI starting\n" );
	_kernel_process_begin( appname );

}

