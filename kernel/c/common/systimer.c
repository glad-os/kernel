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

#include "define.h"
#include "systimer.h"
#include "irq.h"
#include "start.h"
#include "video.h"



/**
 *
 * _kernel_systimer_init
 *
 * Initialisation process for systimer subsystem.
 *
 */
void _kernel_systimer_init( void )
{

	// @todo describe here, and document officially, the difference between Timer and SysTimer facilities
	uint32_t val;

	// set System Timer #1 using free running counter value
	val = get_word( ARM_SYSTIMER_COUNTER_LOW_REGISTER );

	put_word( ARM_SYSTIMER_COMPARE_1_REGISTER, val + 1000000 );

	_kernel_interrupt_register_irq_handler( INTERRUPT_IRQ_SYSTIMER, &_kernel_systimer_event, (void *) 0 );
	_kernel_interrupt_enable_irq( INTERRUPT_IRQ_SYSTIMER );

}


/**
 *
 * _kernel_systimer_init
 *
 * Initialisation process for systimer subsystem.
 *
 */
void _kernel_systimer_event( void *argument )
{

	unsigned int val;

	// check: is this the droid you're looking for? (System Timer #1)
	if ( get_word( ARM_SYSTIMER_CONTROL_STATUS_REGISTER ) & (1<<1) )
	{

		// clear by writing to CS register (http://xinu.mscs.mu.edu/BCM2835_System_Timer)
		*(unsigned int *)ARM_SYSTIMER_CONTROL_STATUS_REGISTER = (1<<1); // note web says write 0x80 but this contradicts BCM document they link to! *sigh*

		// output something so we can some day get this working on 64-bit build
		_kernel_video_print_string( "." );

		// systimer is 1MHz
		val = get_word( ARM_SYSTIMER_COUNTER_LOW_REGISTER );
		put_word( ARM_SYSTIMER_COMPARE_1_REGISTER, val + 1000000 );

	}

}



/**
 *
 * _kernel_systimer_wait_msec
 *
 * Waits for the specified number of msec (milli-seconds, 1/1,000th of a second)
 *
 */
void _kernel_systimer_wait_msec( unsigned int msec )
{

	unsigned int now, finish;

	now		= get_word( ARM_SYSTIMER_COUNTER_LOW_REGISTER );
	finish	= now + msec * 1000;

	do { now = get_word( ARM_SYSTIMER_COUNTER_LOW_REGISTER ); } while ( now > finish );
	do { now = get_word( ARM_SYSTIMER_COUNTER_LOW_REGISTER ); } while ( now < finish );

}


/**
 *
 * _kernel_systimer_wait_usec
 *
 * Waits for the specified number of usec (micro-seconds, 1/1,000,000th of a second)
 *
 */
void _kernel_systimer_wait_usec( unsigned int usec )
{

	unsigned int now, finish;

	now		= get_word( ARM_SYSTIMER_COUNTER_LOW_REGISTER );
	finish	= now + usec;

	do { now = get_word( ARM_SYSTIMER_COUNTER_LOW_REGISTER ); } while ( now > finish );
	do { now = get_word( ARM_SYSTIMER_COUNTER_LOW_REGISTER ); } while ( now < finish );

}
