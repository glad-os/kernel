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



#ifndef START_H_
#define START_H_



	extern unsigned int 		*swi_registers_in;

	extern void 			put_word( unsigned int, unsigned int );
	extern unsigned int		get_word( unsigned int );

	extern void 			move1k( void );
	extern void 			blank1k( void );

	extern void 			delay_loop( unsigned int );

	extern void 			_kernel_process_push_cpu_state( unsigned int * );	
	extern void 			_kernel_process_pop_cpu_state( unsigned int * );
	extern void 			_kernel_process_start( void );

	extern void 			_kernel_mmu_invalidate_tlb( void );
	extern void 			_kernel_mmu_invalidate_btc( void );
	extern void 			_kernel_mmu_invalidate_i_cache( void );
	extern void 			_kernel_mmu_clean_l1_d_cache( void );
	extern void 			_kernel_mmu_clean_l2_d_cache( void );


	extern void			_kernel_start_enable_fpu( void );
	extern void			_kernel_start_clear_bss( void );
	extern void			_kernel_start_install_vector_table( void );
	extern void			_kernel_start_setup_stacks( void );

	extern void			_kernel_interrupt_enable_interrupts( void );
	extern void			_kernel_interrupt_disable_interrupts( void );



#endif /*START_H_*/
