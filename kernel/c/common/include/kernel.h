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



#ifndef KERNEL_H
#define KERNEL_H



	void _kernel_init( void );
	void _kernel_panic( char *subsystem, char *msg );

	void _kernel_start_enable_fpu( void );
	void _kernel_start_clear_bss( void );
	void _kernel_start_install_vector_table( void );
	void _kernel_start_setup_stacks( void );

	void _kernel_start_cli( void );



#endif /*KERNEL_H*/
