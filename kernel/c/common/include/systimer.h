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



#ifndef SYSTIMER_H
#define SYSTIMER_H



	// BCM2835-ARM.pdf (172/205)
	#define		ARM_SYSTIMER_REGISTERS							( ARM_PERIPHERAL_BASE_ADDRESS + 0x3000 )
	#define 	ARM_SYSTIMER_CONTROL_STATUS_REGISTER			( ARM_SYSTIMER_REGISTERS + 0x00 )
	#define 	ARM_SYSTIMER_COUNTER_LOW_REGISTER				( ARM_SYSTIMER_REGISTERS + 0x04 )
	#define 	ARM_SYSTIMER_COUNTER_HIGH_REGISTER				( ARM_SYSTIMER_REGISTERS + 0x08 )
	#define 	ARM_SYSTIMER_COMPARE_0_REGISTER					( ARM_SYSTIMER_REGISTERS + 0x0C )
	#define 	ARM_SYSTIMER_COMPARE_1_REGISTER					( ARM_SYSTIMER_REGISTERS + 0x10 )
	#define 	ARM_SYSTIMER_COMPARE_2_REGISTER					( ARM_SYSTIMER_REGISTERS + 0x14 )
	#define 	ARM_SYSTIMER_COMPARE_3_REGISTER					( ARM_SYSTIMER_REGISTERS + 0x18 )



	void _kernel_systimer_init( void );
	void _kernel_systimer_event( void *argument );
	void _kernel_systimer_wait_msec( unsigned int msec );
	void _kernel_systimer_wait_usec( unsigned int usec );



#endif /*SYSTIMER_H*/
