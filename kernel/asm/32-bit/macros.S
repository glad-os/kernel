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
                                 * On entry:
                                 * mode = required mode
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
                                 * Preserves the CPU's state (register set) in the given memory area.
                                 * "state" is defined as r0-r12,sp,lr,psr,pc
                                 *
                                 * On entry:
                                 * r0 = pointer to area of memory where state is to be stored + 4
                                 *      (r0 has already been preserved, prior to entry)
                                 *
                                 * On exit:
                                 * r0, r4-r6 = corrupted
                                 *
                                 */
.macro                          _PUSH_CPU_STATE
                                STMIA           r0!     , {r1-r12}                                  // PUSH: r1..r12

                                _SET_MODE       SYS                                                 // PUSH: USR_sp, USR_lr
                                MOV             r5      , sp
                                MOV             r6      , lr
                                _SET_MODE       SVC
                                STMIA           r0!     , {r5, r6}

                                MRS             r4      , spsr                                      // PUSH: USR_cpsr, USR_pc
                                STMIA           r0!     , {r4, lr}                               
.endm


                                /*
                                 *
                                 * _POP_CPU_STATE
                                 *
                                 * Reinstates the CPU's state (register set) from the given memory area
                                 * "state" is defined as r0-r12,sp,lr,psr,pc
                                 *
                                 * On entry:
                                 * r0 = pointer to area of memory where state is to be reinstated from + 4
                                 *      (r0 will be reinstated separately, after exit)
                                 *
                                 * On exit:
                                 * r0       = corrupted
                                 * r1-r12   = restored
                                 * USR_sp   = restored
                                 * USR_lr   = restored
                                 * SVC_spsr = restored (USR_cpsr)
                                 * SVC_lr   = restored (USR_pc)
                                 *
                                 */
.macro                          _POP_CPU_STATE
                                LDMIA           r0!     , {r1-r12}                                  // POP: r1..r12

                                STMFD           sp!     , {r4}                                      // stack r4 (about to be used for scratch-space)

                                _SET_MODE       SYS                                                 // POP: USR_sp, USR_lr
                                LDMIA           r0!     , {sp, lr}

                                _SET_MODE       SVC                                                 // POP: SVC_spsr, SVC_lr
                                LDMIA           r0!     , {r4, lr}
                                MSR             spsr    , r4

                                LDMFD           sp!     , {r4}                                      // restore r4
.endm
