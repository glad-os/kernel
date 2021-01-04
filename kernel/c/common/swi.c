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
#include <stdint.h>



/**
 *
 * _kernel_swi_handler
 *
 * Dispatches the relevant internal SWI function, based on the supplied SWI number.
 *
 */
uintptr_t _kernel_swi_handler( uintptr_t swi )
{

        /*
	// notify the screen of the SWI number invoked, as well as the value of in.r[0]
	uintptr_t *regs_ptr;
	regs_ptr = (uintptr_t *) swi_params_ptr_in;
	//_kernel_video_print_string( "_kernel_swi_handler : " ); _kernel_video_print_hex( swi         ); _kernel_video_print_string( "\n" );
	//_kernel_video_print_string( "in.r[0] = 0x"           ); _kernel_video_print_hex( regs_ptr[0] ); _kernel_video_print_string( "\n" );	

	// @todo the handler should return the result
	handler[ swi ]();
	return 0;
        */

}
