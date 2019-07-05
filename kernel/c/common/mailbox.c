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
#include "mailbox.h"
#include "start.h"



/**
 *
 * _kernel_mailbox_write
 *
 * Writes the value to the Mailbox channel.
 *
 */
unsigned int _kernel_mailbox_write( unsigned int channel, uintptr_t data )
{

	volatile uint32_t value;

	while ( 1 )
	{
		value = get_word( ARM_MAILBOX_STATUS_REGISTER );
		if ( ( value & ARM_MAILBOX_STATUS_FULL ) == 0 )
			break;
	}

	// how can I take my 64-bit pointer "data" here, and extract the lower 32-bits without GCC spotting this and complaining at me?
	value = (uint32_t) (data);
	// the above works, but doesn't this begin to 'pollute' the code for 32-bit viewers? is there a way I can specifically avoid using
	// uint64_t in the above in favour of a type that will be 32/64 bit depending on the environment here? is there a size_t or pointer_t that will help us?

	put_word( ARM_MAILBOX_WRITE_REGISTER, value | channel );

	return 0;

}



/**
 *
 * _kernel_mailbox_read
 *
 * Returns the value read from the Mailbox channel.
 *
 */
unsigned int _kernel_mailbox_read( unsigned int channel )
{

	volatile unsigned int result;

	while ( 1 )
	{

		while ( 1 )
		{
			result = get_word( ARM_MAILBOX_STATUS_REGISTER );
			if ( ( result & ARM_MAILBOX_STATUS_EMPTY ) == 0 )
			{
				break;
			}
		}

		result = get_word( ARM_MAILBOX_READ_REGISTER );
		if ( ( result & 0x0f ) == channel )
		{
			break;
		}

	}

	return result;

}
