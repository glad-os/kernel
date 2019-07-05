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



#include "gpio.h"
#include "start.h"



/**
 *
 * _kernel_gpio_function_select
 *
 * Assigns the given function to the given pin.
 *
 */
void _kernel_gpio_function_select( unsigned int pin, unsigned int fn ) {

    unsigned int reg, shift, value;

    switch ( pin / 10 ) {
        case 0 : reg = ARM_GPIO_FUNCTION_SELECT_0; break;
        case 1 : reg = ARM_GPIO_FUNCTION_SELECT_1; break;
        case 2 : reg = ARM_GPIO_FUNCTION_SELECT_2; break;
        case 3 : reg = ARM_GPIO_FUNCTION_SELECT_3; break;
        case 4 : reg = ARM_GPIO_FUNCTION_SELECT_4; break;
        case 5 : reg = ARM_GPIO_FUNCTION_SELECT_5; break;
    }
    shift = (pin % 10) * 3;

    value = get_word( reg );
    value &= ~(   0b111        << shift );
    value |=  ( ( 0b100 + fn ) << shift );
    put_word( reg, value );

}



/**
 *
 * _kernel_gpio_set_pull_up_or_down
 *
 * Sets the given pin as pull up or pull down.
 *
 */
void _kernel_gpio_set_pull_up_or_down( unsigned int pin, unsigned int state ) {

	unsigned int bit, reg;

	if ( pin < 32 ) {
		reg = ARM_GPIO_PULL_UP_DOWN_ENABLE_CLOCK_0;
		bit = 1 << pin;
	} else {
		reg = ARM_GPIO_PULL_UP_DOWN_ENABLE_CLOCK_1;
		bit = 1 << ( pin - 32 );
	}

	// BCM2835 (p101/235)
	put_word( ARM_GPIO_PULL_UP_DOWN_ENABLE, state );
	delay_loop( 150 );
	put_word( reg, bit );
	delay_loop( 150 );
	put_word( ARM_GPIO_PULL_UP_DOWN_ENABLE, 0 );
	put_word( reg, 0 );

}
