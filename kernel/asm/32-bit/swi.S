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



.global                                                 swi_handler
.global                                                 swi_temp_cpu_state



#define                                                 SYS                     0b00011111
#define                                                 SVC                     0b00010011



.macro                                                  _SET_MODE               mode
                                                        MRS                     r1          	, cpsr
                                                        BIC                     r1          	, r1        	, #0b00011111
                                                        ORR                     r1          	, r1        	, #\mode
                                                        MSR                     cpsr            , r1
.endm



.macro                                                  _KPUSH_CPU_STATE
                                                        STMIA                   r0!             , {r1-r12}                              @ PUSH: r1..r12

                                                        _SET_MODE               SYS                                                     @ PUSH: sp, lr
                                                        MOV                     r4              , sp
                                                        MOV                     r5              , lr
                                                        _SET_MODE               SVC
                                                        STMIA                   r0!             , {r4,r5}

                                                        MRS                     r4              , spsr                                  @ PUSH: cpsr, pc
                                                        STMIA                   r0!             , {r4,lr}                               
.endm



.macro                                                  _KPOP_CPU_STATE
                                                        LDMIA                   r0!             , {r1-r12}                              @ POP: r1..r12

                                                        _SET_MODE               SYS                                                     @ POP: sp, lr
                                                        LDMIA                   r0!             , {sp, lr}
                                                        _SET_MODE               SVC

                                                        LDMIA                   r0!             , {r4,lr}                               @ POP: cpsr, pc
                                                        MSR                     spsr            , r4

.endm



							@
							@ swi_handler
							@
							@ The SWI handler code - invoked when an SWI instruction has been executed.
							@
							@ On entry:
							@ r0 - r3 carry up to 4 parameters for the individual SWI to use
							@
							@ On exit:
							@ All registers reinstated *except* R0 which carries a status (success/failure) value appropriate to the individual SWI
							@
swi_handler:
                                                        STR                     r0                      , swi_temp_cpu_state + 0
                                                        ADR                     r0                      , swi_temp_cpu_state + 4
                                                        _KPUSH_CPU_STATE
                                                        LDR                     r0                      , swi_temp_cpu_state + 0
							
                                                        CPSIE 			i                                                   					@ OS_ReadC in busy waiting stops interrupts! (consider re-entrant handler in future)

swi_bl_to_handler:
                                                        ADR                     r4                      , swi_function_pointers
							LDR			r5			, [lr, #-4]							@ get SWI instruction and mask to determine SWI number
							BIC			r5			, r5		, #0xff << 24
                                                        ADR                     lr                      , swi_handler_return
                                                        LDR                     pc                      , [r4, r5, LSL #2]


swi_handler_return:
                                                        STR                     r0                      , swi_temp_cpu_state + 0
                                                        ADR                     r0                      , swi_temp_cpu_state + 4
                                                        _KPOP_CPU_STATE
                                                        LDR                     r0                      , swi_temp_cpu_state + 0

						    	MOVS                    pc                      , lr    							@ get SPSR and LR ready, then ERET (EL1->EL0)



swi_temp_sp:                                            .word                   0
swi_temp_cpu_state:                                     .space                  16 * 4                                                                                  @ r1-12, sp, lr, pc, cpsr (17 registers)



swi_function_pointers:
                                                        .word                   _kernel_swi_os_setmode
                                                        .word                   _kernel_swi_os_setcolour
                                                        .word                   _kernel_swi_os_clearscreen
                                                        .word                   _kernel_swi_os_putpixel
                                                        .word                   _kernel_swi_os_getpixel
                                                        .word                   _kernel_swi_os_setcursorposition
                                                        .word                   _kernel_swi_os_printchar
                                                        .word                   _kernel_swi_os_printstring
                                                        .word                   _kernel_swi_os_readvideovariables
                                                        .word                   _kernel_swi_os_readc
                                                        .word                   _kernel_swi_os_processbegin
                                                        .word                   _kernel_swi_os_processexit
