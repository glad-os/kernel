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



#include "irq.h"
#include "systimer.h"
#include "start.h"



irq_entry irq_entries[96] = {
	/* INTERRUPT_IRQ_SYSTIMER */ {
			.handler = 0, .argument = 0,
			.enabled = 0,
			.pending_register = ARM_INTERRUPT_PENDING_1_REGISTER,      .pending_register_bit = 1,
			.enable_register  = ARM_INTERRUPT_ENABLE_IRQS_1_REGISTER,  .enable_register_bit  = 1,
			.disable_register = ARM_INTERRUPT_DISABLE_IRQS_1_REGISTER, .disable_register_bit = 1
	},
	/* INTERRUPT_IRQ_USB */ {
			.handler = 0, .argument = 0,
			.enabled = 0,
			.pending_register = ARM_INTERRUPT_PENDING_1_REGISTER,      .pending_register_bit = 9,
			.enable_register  = ARM_INTERRUPT_ENABLE_IRQS_1_REGISTER,  .enable_register_bit  = 9,
			.disable_register = ARM_INTERRUPT_DISABLE_IRQS_1_REGISTER, .disable_register_bit = 9
	},
	/* INTERRUPT_IRQ_TIMER3 */ {
			.handler = 0, .argument = 0,
			.enabled = 0,
			.pending_register = ARM_INTERRUPT_PENDING_1_REGISTER,      .pending_register_bit = 3,
			.enable_register  = ARM_INTERRUPT_ENABLE_IRQS_1_REGISTER,  .enable_register_bit  = 3,
			.disable_register = ARM_INTERRUPT_DISABLE_IRQS_1_REGISTER, .disable_register_bit = 3
	}
};



/**
 *
 * _kernel_interrupt_init
 *
 * Initialisation process for interrupt subsystem.
 *
 */
void _kernel_interrupt_init( void )
{

	unsigned int i;

	for ( i = 0; i < 96; i++ ) {
		irq_entries[ i ].enabled  = 0;
		irq_entries[ i ].handler  = 0;
		irq_entries[ i ].argument = 0;
	}

	put_word( ARM_INTERRUPT_DISABLE_IRQS_1_REGISTER, 0xffffffff );
	put_word( ARM_INTERRUPT_DISABLE_IRQS_2_REGISTER, 0xffffffff );
	put_word( ARM_INTERRUPT_DISABLE_BASIC_IRQS_REGISTER, 0xffffffff );

	put_word( ARM_INTERRUPT_BASIC_PENDING_REGISTER, get_word( ARM_INTERRUPT_BASIC_PENDING_REGISTER ) );
	put_word( ARM_INTERRUPT_PENDING_1_REGISTER, get_word( ARM_INTERRUPT_PENDING_1_REGISTER ) );
	put_word( ARM_INTERRUPT_PENDING_2_REGISTER, get_word( ARM_INTERRUPT_PENDING_2_REGISTER ) );

	// can now enable interrupts, officially
	_kernel_interrupt_enable_interrupts();

}



/**
 *
 * _kernel_interrupt_register_irq_handler
 *
 * Registers a handler routine against a given IRQ.
 *
 */
 void _kernel_interrupt_register_irq_handler( int irq, irq_routine handler, void *argument )
{

	irq_entries[ irq ].handler  = handler;
	irq_entries[ irq ].argument = argument;

}



/**
 *
 * _kernel_interrupt_enable_irq
 *
 * Enables an individual IRQ.
 *
 */
 void _kernel_interrupt_enable_irq( int irq )
{

	put_word( irq_entries[ irq ].enable_register, 1 << irq_entries[ irq ].enable_register_bit );
	irq_entries[ irq ].enabled = 1;

}



/**
 *
 * _kernel_interrupt_disable_irq
 *
 * Disables an individual IRQ.
 *
 */
 void _kernel_interrupt_disable_irq( int irq )
{

	put_word( irq_entries[ irq ].disable_register, 1 << irq_entries[ irq ].disable_register_bit );
	irq_entries[ irq ].enabled = 0;

}



/**
 *
 * _kernel_interrupt_handler
 *
 * Invokes individual IRQ handler(s).
 *
 */
 void _kernel_interrupt_handler( void )
{

	unsigned int i, pending;

	for ( i = 0; i < 96; i++ ) {
		if ( irq_entries[i].enabled && irq_entries[i].handler ) {

			pending = get_word( irq_entries[i].pending_register );
			if ( pending & (1 << irq_entries[i].pending_register_bit ) ) {
				irq_entries[i].handler( irq_entries[i].argument );
			}

		}
	}

}
