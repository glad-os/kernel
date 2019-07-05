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



typedef unsigned int (*swi_function)( void );

swi_function handler[] =
{
	_kernel_swi_os_setmode,
	_kernel_swi_os_setcolour,
	_kernel_swi_os_clearscreen,
	_kernel_swi_os_putpixel,
	_kernel_swi_os_getpixel,
	_kernel_swi_os_setcursorposition,
	_kernel_swi_os_printchar,
	_kernel_swi_os_printstring,
	_kernel_swi_os_readvideovariables,
	_kernel_swi_os_readc,
    _kernel_swi_os_processbegin,
    _kernel_swi_os_processexit
};



/**
 *
 * _kernel_swi_handler
 *
 * Dispatches the relevant internal SWI function, based on the supplied SWI number.
 *
 */
unsigned int _kernel_swi_handler( unsigned int swi )
{

	// @todo the handler should return the result
	handler[ swi ]();
	return 0;

}
