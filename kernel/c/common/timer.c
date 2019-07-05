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



#include "timer.h"
#include "irq.h"



/**
 *
 * _kernel_timer_init
 *
 * Initialisation process for timer subsystem.
 *
 */
void _kernel_timer_init( void )
{

	/*
	// enable this particular interrupt
	*(unsigned int *)ARM_INTERRUPT_ENABLE_BASIC_IRQS_REGISTER |= 1;

	// load the timer with a counter of some sort
	*(unsigned int *)ARM_TIMER_LOAD_REGISTER = 1000000;

	// enable the timer and its interrupt
	*(unsigned int *)ARM_TIMER_CONTROL_REGISTER |= ((1<<1) + (1<<5) + (1<<7) + (3<<2)); // 32-bit counter, enable int, enable timer, pre-scale 256

	interrupt_install_handler( 0, timer_event );
	//interrupt_enable_irq();
	*/

}



/**
 *
 * _kernel_timer_event
 *
 * Event handler for when timer event occurs. (For now, nothing actually happens).
 *
 */
void _kernel_timer_event( void )
{

	// clear the pending bit
	*(unsigned int *)ARM_TIMER_CLEAR_REGISTER = 0;
	// _kernel_video_print_string( "!" );

}
