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



#include "define.h"



#ifndef INTERRUPT_H
#define INTERRUPT_H



	// BCM2835-ARM.pdf (112/205)
	// ARM_INTERRUPT_ENABLE_IRQS_1_REGISTER bits 0-3 represent systimer 0-3
	// (but install handler under which irq_routine index, to complement this flag when it's been set???)
	// given it suggests BASIC/1/2 are "0, 1, and 2" presumably BASIC does 0..31, 1 does 32..63, 2 does 64..95?
	// i.e. to enable SYSTIMER 0, I enable bit 0 of IRQ1, and install handler at irq_routine.32??
	#define		ARM_INTERRUPT_REGISTERS					( ARM_PERIPHERAL_BASE_ADDRESS + 0xb200 )
	#define		ARM_INTERRUPT_BASIC_PENDING_REGISTER			( ARM_INTERRUPT_REGISTERS + 0x00 )
	#define		ARM_INTERRUPT_PENDING_1_REGISTER			( ARM_INTERRUPT_REGISTERS + 0x04 )
	#define		ARM_INTERRUPT_PENDING_2_REGISTER			( ARM_INTERRUPT_REGISTERS + 0x08 )
	#define		ARM_INTERRUPT_FIQ_CONTROL_REGISTER			( ARM_INTERRUPT_REGISTERS + 0x0C )
	#define		ARM_INTERRUPT_ENABLE_IRQS_1_REGISTER			( ARM_INTERRUPT_REGISTERS + 0x10 )
	#define		ARM_INTERRUPT_ENABLE_IRQS_2_REGISTER			( ARM_INTERRUPT_REGISTERS + 0x14 )
	#define		ARM_INTERRUPT_ENABLE_BASIC_IRQS_REGISTER		( ARM_INTERRUPT_REGISTERS + 0x18 )
	#define		ARM_INTERRUPT_DISABLE_IRQS_1_REGISTER			( ARM_INTERRUPT_REGISTERS + 0x1C )
	#define		ARM_INTERRUPT_DISABLE_IRQS_2_REGISTER			( ARM_INTERRUPT_REGISTERS + 0x20 )
	#define		ARM_INTERRUPT_DISABLE_BASIC_IRQS_REGISTER		( ARM_INTERRUPT_REGISTERS + 0x24 )

	// list of (logical) IRQs that the interrupt subsystem knows about, and can deal with
	#define INTERRUPT_IRQ_SYSTIMER		0
	#define INTERRUPT_IRQ_USB		1
	#define INTERRUPT_IRQ_TIMER3		2



	typedef void (*irq_routine) ( void *argument );

	typedef struct {
		irq_routine	 handler;				/* address of the IRQ handler routine 			*/
		void		*argument;				/* pointer to whatever argument(s) the handler may need */
		unsigned int enabled;					/* whether currently enabled or not			*/
		unsigned int pending_register;				/* which pending register/bit identifies a pending IRQ 	*/
		unsigned int pending_register_bit;
		unsigned int enable_register;				/* which register/bit for enabling this IRQ 		*/
		unsigned int enable_register_bit;
		unsigned int disable_register;				/* which register/bit for disabling this IRQ 		*/
		unsigned int disable_register_bit;
	} irq_entry;

	void _kernel_interrupt_init( void );
	void _kernel_interrupt_handler( void );
	void _kernel_interrupt_register_irq_handler( int irq, irq_routine handler, void *argument );
	void _kernel_interrupt_enable_irq( int irq );
	void _kernel_interrupt_disable_irq( int irq );



#endif /*INTERRUPT_H*/
