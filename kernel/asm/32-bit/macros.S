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


#define                         SYS             0b00011111
#define                         SVC             0b00010011


                                /*
                                 *
                                 * _SET_MODE
                                 *
                                 * Changes the current CPSR to the required mode (USER, SYS, etc.)
                                 *
                                 * Parameters:
                                 * mode = required mode
                                 *
                                 * On entry:
                                 * -
                                 *
                                 * On exit:
                                 * r4 = corrupted
                                 *
                                 */
.macro                          _SET_MODE       mode

                                MRS             r4      , cpsr
                                BIC             r4      , r4, #0b00011111
                                ORR             r4      , r4, #\mode
                                MSR             cpsr    , r4

.endm


                                /*
                                 *
                                 * _PUSH_CPU_STATE
                                 *
                                 * Preserves the CPU's state (register set) in the current process's CPU state struct.
                                 * "state" is defined as USR mode's r0-r12, sp, lr, cpsr, and pc registers.
                                 *
                                 * Parameters:
                                 * scratch       = address of 4 bytes (word-aligned) in memory (scratch space)
                                 * cpu_state_ptr = pointer to a cpu_state struct in memory, where state is to be stored
                                 *
                                 */
.macro                          _PUSH_CPU_STATE scratch, cpu_state_ptr

                                STR             sp      , \scratch + 0                              // preserve SP

                                ADR             sp      , \cpu_state_ptr                            // use SP to point at the cpu_state struct
                                LDR             sp      , [sp]

                                STMIA           sp      , {r0-r12}                                  // PUSH: r0-r12

                                _SET_MODE       SYS                                                 // PUSH: USR_sp, USR_lr
                                MOV             r5      , sp    
                                MOV             r6      , lr
                                _SET_MODE       SVC
                                STR             r5      , [sp, #13*4]
                                STR             r6      , [sp, #14*4]

                                MRS             r5      , spsr                                      // PUSH: USR_cpsr, USR_pc                                   
                                STR             r5      , [sp, #15*4]                               
                                STR             lr      , [sp, #16*4]

                                LDR             sp      , \scratch + 0                              // reinstate SP

.endm


                                /*
                                 *
                                 * _POP_CPU_STATE
                                 *
                                 * Reinstates the CPU's state (register set) in the current process's CPU state struct
                                 * "state" is defined as r0-r12,sp,lr,psr,pc
                                 *
                                 * On entry:
                                 * r0 = pointer to state to be popped
                                 *
                                 * On exit:
                                 * -
                                 *
                                 */
.macro                          _POP_CPU_STATE

                                _SET_MODE       SYS                                                 // POP: USR_sp, USR_lr
                                LDR             sp      , [r0, #13*4]
                                LDR             lr      , [r0, #14*4]

                                _SET_MODE       SVC                                                 // POP: SVC_spsr, SVC_lr (will become USR_cpsr, USR_pc)
                                LDR             r4      , [r0, #15*4]
                                LDR             lr      , [r0, #16*4]
                                MSR             spsr    , r4

                                LDMIA           r0      , {r0-r12}                                  // POP: r0-r12


.endm
