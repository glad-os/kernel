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



.global                                     _kernel_process_push_cpu_state
.global                                     _kernel_process_pop_cpu_state
.global                                     _kernel_process_continue



#define                                     SYS                 0b00011111
#define                                     SVC                 0b00010011



.macro                                      _SET_MODE           mode
                                            MRS         	r1          	, cpsr
                                            BIC         	r1          	, r1        	, #0b00011111
                                            ORR         	r1          	, r1        	, #\mode
                                            MSR         	cpsr            , r1
.endm



.macro                                      _KERNEL_PROCESS_PUSH_CPU_STATE

                                            STMFD               sp!             , {r0-r12}                              @ STACK: r0  ... r12
                                            
                                            _SET_MODE           SYS                                                     @ STACK: r13 ... r14
                                            MOV                 r4              , r13
                                            MOV                 r5              , r14
                                            _SET_MODE           SVC
                                            STMFD               sp!             , {r4,r5}
                                            
                                            MRS                 r4              , spsr                                  @ STACK: spsr, pc
                                            STMFD               sp!             , {r4,lr}                               
.endm



                                            @
                                            @ _kernel_process_push_cpu_state
                                            @
                                            @ ?
                                            @
                                            @ On entry:
                                            @ -
                                            @
                                            @ On exit:
                                            @ The stack has had (User Mode) r0..r14, CPSR and PC pushed onto it
                                            @
_kernel_process_push_cpu_state:
                                            _KERNEL_PROCESS_PUSH_CPU_STATE



                                            @
                                            @ _kernel_process_pop_cpu_state
                                            @
                                            @ ?
                                            @
                                            @ On entry:
                                            @ r0 = pointer to kernel memory where registers were preserved
                                            @
                                            @ On exit:
                                            @ ?
                                            @
_kernel_process_pop_cpu_state:
                                            MRS         	r1          	, cpsr                                  @ switch to SYS mode (gain access to USR mode register bank)
                                            BIC         	r1          	, r1        	, #0b00011111
                                            ORR         	r1          	, r1        	, #0b00011111
                                            MSR         	cpsr        	, r1

                                            @LDR                r11             , [r0, #0x0]
                                            @LDR                r12             , [r0, #0x4]
                                            @LDR                sp              , [r0, #0x8]
                                            @LDR                r3              , [r0, #0xc]

                                            MRS         	r1          	, cpsr                                  @ switch to SVC mode
                                            BIC         	r1          	, r1        	, #0b00011111
                                            ORR         	r1          	, r1        	, #0b00010011
                                            MSR                 cpsr            , r1

                                            // ERET will return from the SWI Exception and should now safely return to userland
                                            // need to consider that we jump out of the kernel without 'undoing' unwanted SVC stack work at this point
                                            @LDR                r0              , svc_sp_value
                                            @LDR                sp              , [r0]
                                            @MOV                lr              , r3
                                            ERET



svc_sp_value:                               @.word              saved_svc_sp_entrypoint



                                            @
                                            @ _kernel_process_continue
                                            @
                                            @ This code should, based on the value of "current", take the relevant CPU state and 
                                            @ restore it, allowing the process to start/continue from "where it left off"
                                            @
                                            @ To do this, in theory we need USER mode to restore, from the r0 pointer, r0-12, sp, lr, pc and cpsr
                                            @
                                            @ On entry:
                                            @ r0 = pointer to CPU state to reinstate
                                            @
                                            @ On exit:
                                            @ ?
                                            @
_kernel_process_continue:
                                            STR                 r0              , _cpu_state_ptr

                                            // in USER mode, get r0-12,sp,lr reinstated
                                            _SET_MODE           SYS
                                            MOV                 lr              , r0                                    @ unstack r0-14 of CPU state
                                            LDMIA               lr              , {r0-r12,sp,lr}

                                            // in SVC mode, get LR and SPSR *ready* to reinstate
                                            _SET_MODE           SVC
                                            STMFD               sp!             , {r0}
                                            LDR                 r0              , _cpu_state_ptr
                                            LDR                 lr              , [r0, #15*4]                           @ load LR   with preserved PC
                                            LDR                 r0              , [r0, #16*4]                           @ load SPSR with preserved CPSR
                                            MSR                 spsr            , r0
                                            LDMFD               sp!             , {r0}

                                            // launch process...
                                            MOVS                pc              , lr



_cpu_state_ptr:                             .word               0
