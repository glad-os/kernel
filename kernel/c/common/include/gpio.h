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



#ifndef GPIO_H
#define GPIO_H



	#define		ARM_GPIO_REGISTERS								( ARM_PERIPHERAL_BASE_ADDRESS + 0x00200000 )
	#define		ARM_GPIO_FUNCTION_SELECT_REGISTERS						( ARM_GPIO_REGISTERS + 0x00 )
	#define		ARM_GPIO_FUNCTION_SELECT_0							ARM_GPIO_FUNCTION_SELECT_REGISTERS + 0x00
	#define		ARM_GPIO_FUNCTION_SELECT_1							ARM_GPIO_FUNCTION_SELECT_REGISTERS + 0x04
	#define		ARM_GPIO_FUNCTION_SELECT_2							ARM_GPIO_FUNCTION_SELECT_REGISTERS + 0x08
	#define		ARM_GPIO_FUNCTION_SELECT_3							ARM_GPIO_FUNCTION_SELECT_REGISTERS + 0x0c
	#define		ARM_GPIO_FUNCTION_SELECT_4							ARM_GPIO_FUNCTION_SELECT_REGISTERS + 0x10
	#define		ARM_GPIO_FUNCTION_SELECT_5							ARM_GPIO_FUNCTION_SELECT_REGISTERS + 0x14
	#define		ARM_GPIO_PULL_UP_DOWN_ENABLE							ARM_GPIO_FUNCTION_SELECT_REGISTERS + 0x94
	#define		ARM_GPIO_PULL_UP_DOWN_ENABLE_CLOCK_0						ARM_GPIO_FUNCTION_SELECT_REGISTERS + 0x98
	#define		ARM_GPIO_PULL_UP_DOWN_ENABLE_CLOCK_1						ARM_GPIO_FUNCTION_SELECT_REGISTERS + 0x9c

	#define		ARM_GPIO_PIN_DISABLE_PULL_UP_PULL_DOWN						0b00
	#define		ARM_GPIO_PIN_ENABLE_PULL_UP							0b01
	#define		ARM_GPIO_PIN_ENABLE_PULL_DOWN							0b10

	#define		GPIO_SELECT_INPUT								0b000
	#define		GPIO_SELECT_OUTPUT								0b001
	#define		GPIO_SELECT_ALTERNATIVE_FUNCTION_0						0b100
	#define		GPIO_SELECT_ALTERNATIVE_FUNCTION_1						0b101
	#define		GPIO_SELECT_ALTERNATIVE_FUNCTION_2						0b110
	#define		GPIO_SELECT_ALTERNATIVE_FUNCTION_3						0b111
	#define		GPIO_SELECT_ALTERNATIVE_FUNCTION_4						0b011
	#define		GPIO_SELECT_ALTERNATIVE_FUNCTION_5						0b010



	void			_kernel_gpio_function_select		( unsigned int pin, unsigned int fn );
	void			_kernel_gpio_set_pull_up_or_down	( unsigned int pin, unsigned int state );



#endif /*GPIO_H*/
