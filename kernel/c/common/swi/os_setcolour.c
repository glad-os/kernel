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
#include "swi.h"
#include "video.h"

unsigned int _kernel_swi_os_setcolour( void )
{

	uintptr_t *regs_ptr;
	uintptr_t f, r, g, b;

	regs_ptr = (uintptr_t *) swi_params_ptr_in;

	f = *(regs_ptr+0);
	r = *(regs_ptr+1);
	g = *(regs_ptr+2);
	b = *(regs_ptr+3);
	_kernel_video_set_colour( f, (r<<16) + (g<<8) + b );

	return 0;

}

