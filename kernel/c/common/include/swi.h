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
#include <swi_macro.h>


#ifndef SWI_H
#define SWI_H



	// up to a maximum of 4 register values can be passed in/out of an SWI using this structure
	struct _kernel_regs {
		uintptr_t r[ 4 ];
	};

	// internal SWI handlers
	unsigned int _kernel_swi_handler( uintptr_t swi );

	unsigned int _kernel_swi_os_setmode( void );
	unsigned int _kernel_swi_os_setcolour( void );
	unsigned int _kernel_swi_os_clearscreen( void );
	unsigned int _kernel_swi_os_putpixel( void );
	unsigned int _kernel_swi_os_getpixel( void );
	unsigned int _kernel_swi_os_setcursorposition( void );
	unsigned int _kernel_swi_os_printchar( void );
	unsigned int _kernel_swi_os_printstring( void );
	unsigned int _kernel_swi_os_readvideovariables( void );
	unsigned int _kernel_swi_os_readc( void );
    	unsigned int _kernel_swi_os_processbegin( void );
    	unsigned int _kernel_swi_os_processexit( void );

	extern unsigned int *swi_params_ptr_in;
	extern unsigned int *swi_params_ptr_out;



#endif /*SWI_H*/
