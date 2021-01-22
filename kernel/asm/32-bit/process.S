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

.global                         _kernel_process_push_cpu_state
.global                         _kernel_process_pop_cpu_state
.global                         _kernel_process_continue


#include                        "macros.S"


                                //
                                // _kernel_process_push_cpu_state
                                //
                                // Takes the currently pushed CPU state (at swi_temp_cpu_state) and copies it into the 
                                // CPU state struct pointed to by r0.
                                //
                                // On entry:
                                // r0 = pointer to a process's CPU state struct
                                //
                                // On exit:
                                // The CPU state has had the register set preserved (USER: r1-r12,sp,lr,cpsr,pc [note: r0 is never preserved])
                                //
_kernel_process_push_cpu_state:
                                LDR             r1      , temp_state_ptr
                                MOV             r2      , #17                                       // 17 registers are in play (r0-r12,sp,lr,cpsr,pc = 17)
          
_kernel_process_push_cpu_state_loop:
                                LDR             r3      , [r1], #4
                                STR             r3      , [r0], #4
                                SUBS            r2      , r2, #1
                                BNE             _kernel_process_push_cpu_state_loop
                                MOV             pc      , lr


temp_state_ptr:
                                .word           swi_temp_cpu_state


                                //
                                // _kernel_process_pop_cpu_state
                                //
                                // ?
                                //
                                // On entry:
                                // r0 = pointer to a process's CPU state struct
                                //
                                // On exit:
                                // ?
                                //
_kernel_process_pop_cpu_state:
                                ADD             r0      , r0, #4
                                _POP_CPU_STATE
                                MOVS            pc      , lr


                                //
                                // _kernel_process_continue
                                //
                                // This code should, based on the value of "current", take the relevant CPU state and 
                                // restore it, allowing the process to start/continue from "where it left off"
                                //
                                // To do this, in theory we need USER mode to restore, from the r0 pointer, r0-12, sp, lr, pc and cpsr
                                //
                                // On entry:
                                // r0 = pointer to CPU state to reinstate
                                //
                                // On exit:
                                // ?
                                //
_kernel_process_continue:
                                STR             r0      , _cpu_state_ptr

                                // in USER mode, get r0-12,sp,lr reinstated
                                _SET_MODE       SYS
                                MOV             lr      , r0                                        // unstack r0-14 of CPU state
                                LDMIA           lr      , {r0-r12, sp, lr}

                                // in SVC mode, get LR and SPSR *ready* to reinstate
                                _SET_MODE       SVC
                                STMFD           sp!     , {r0}
                                LDR             r0      , _cpu_state_ptr
                                LDR             lr      , [r0, #15*4]                               // load LR   with preserved PC
                                LDR             r0      , [r0, #16*4]                               // load SPSR with preserved CPSR
                                MSR             spsr    , r0
                                LDMFD           sp!     , {r0}

                                // launch process...
                                MOVS            pc      , lr


_cpu_state_ptr:                             
                                .word           0
