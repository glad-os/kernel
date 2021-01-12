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


#ifndef PROCESS_H
#define PROCESS_H



    #define		MAX_PROCESSES		16



    // can record up to a maximum of 32 registers (AArch64 = x0-x30, SP_EL0, ELR_EL1)
    typedef struct cpu_state {
        uintptr_t r[ 32 ];
    } cpu_state;

    typedef struct process {
        int         free;       /* whether or not this process is currently free to be used */
        cpu_state   state;      /* the CPU register state */
	int         parent;	/* the id of the parent process */
    } process;



    void 	_kernel_process_init	( void );
    int 	_kernel_process_begin	( char *filename );
    void 	_kernel_process_exit	( void );
    int         _kernel_process_init_cpu_state( cpu_state *state );
    int         _kernel_process_continue( cpu_state *state );


#endif /*PROCESS_H*/
