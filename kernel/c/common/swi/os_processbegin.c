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

#include "swi.h"
#include "process.h"
#include <stdint.h>

unsigned int _kernel_swi_os_processbegin( void )
{

	// [64-bit] exclude process-related code from the build for the moment
	// _kernel_video_print_string( "_kernel_swi_os_processbegin\n" );

	char *c;
	uintptr_t *regs_ptr;

	regs_ptr = (uintptr_t *) swi_params_ptr_in;
	c = (char *) *(regs_ptr+0);

	_kernel_process_begin( c );

	return 0;

}
