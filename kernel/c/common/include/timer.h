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



#ifndef TIMER_H
#define TIMER_H



	#define		ARM_TIMER_REGISTERS								( ARM_PERIPHERAL_BASE_ADDRESS + 0xb000 )
	#define		ARM_TIMER_LOAD_REGISTER							( ARM_TIMER_REGISTERS + 0x400 )
	#define		ARM_TIMER_VALUE_REGISTER						( ARM_TIMER_REGISTERS + 0x404 )
	#define		ARM_TIMER_CONTROL_REGISTER						( ARM_TIMER_REGISTERS + 0x408 )
	#define		ARM_TIMER_CLEAR_REGISTER						( ARM_TIMER_REGISTERS + 0x40C )
	#define		ARM_TIMER_RAW_IRQ_REGISTER						( ARM_TIMER_REGISTERS + 0x410 )
	#define		ARM_TIMER_MASKED_IRQ_REGISTER					( ARM_TIMER_REGISTERS + 0x414 )
	#define		ARM_TIMER_RELOAD_REGISTER						( ARM_TIMER_REGISTERS + 0x418 )



	void timer_init( void );
	void timer_event ( void );
	void timer_display( void );



#endif /*TIMER_H*/
