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
                                 * _PUSH_CPU_STATE
                                 *
                                 * Preserves the CPU's state (register set) in the current process's CPU state struct.
                                 * "state" is defined as USR mode's x0-x15, sp, lr, pstate, and pc registers.
                                 *
                                 * Parameters:
                                 * scratch       = address of 8 bytes (8-byte aligned) in memory (scratch space)
                                 * cpu_state_ptr = pointer to a cpu_state struct in memory, where state is to be stored
                                 *
                                 * When an SVC is taken in AArch64:
                                 * - PSTATE is copied into SPSR_ELn
                                 * - return address is stored in ELR_ELn
                                 *
                                 * To return from the SVC:
                                 * "ERET", which:
                                 * - copies SPSR_ELn into PSTATE
                                 * - copies ELR_ELn into PC
                                 *
                                 * See here for interesting details on using user mode stack to stack everything:
                                 * https://www.sciencedirect.com/topics/engineering/privileged-mode
                                 *
                                 * AArch64 essentially doesn't use banked registers (aside from SP, ELR and CPSR0/SPSRn)
                                 * So apparently it's easier to write this handler? (Can I use PSTATE to select SP_EL0, stack all the registers there,
                                 * switch back to use SP_EL1, do my handler, and then at the end switch into SP_EL0, reinstate all the registers from the 
                                 * userland stack, switch back to SP_EL1 and then ERET? Is that what they're thinking of
                                 *
                                 */
.macro                          _PUSH_CPU_STATE scratch, cpu_state_ptr

                                // let's just tempoarily wreck X24/X25 for a mo, instead...
                                MOV             x24     , sp                                        // preserve SP

                                ADR             x25     , \cpu_state_ptr                            // use SP to point at the cpu_state struct
                                LDR             x25     , [x25]
                                MOV             sp      , x25

                                STP             x0      , x1 , [sp], #16                            // PUSH: x0-x15
                                STP             x2      , x3 , [sp], #16
                                STP             x4      , x5 , [sp], #16
                                STP             x6      , x7 , [sp], #16
                                STP             x8      , x9 , [sp], #16
                                STP             x10     , x11, [sp], #16
                                STP             x12     , x13, [sp], #16
                                STP             x14     , x15, [sp], #16

                                MSR             SPSel   , #0                                        // update PSTATE to use SP_EL0
                                MOV             x25     , sp
                                MOV             x26     , lr
                                STP             x25     , x26, [sp], #16                            // PUSH: sp, lr
                                MSR             SPSel   , #1                                        // update PSTATE to use SP_ELn

                                MRS             x25     , SPSR_EL1
                                MRS             x26     , ELR_EL1
                                STP             x25     , x26, [sp], #16                            // PUSH: pstate, pc

                                MOV             sp      , x24                                       // reinstate SP

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

                                MOV             x24     , x0

                                LDP             x0      , x1 , [x24], #16                            // POP: x0-x15
                                LDP             x2      , x3 , [x24], #16
                                LDP             x4      , x5 , [x24], #16
                                LDP             x6      , x7 , [x24], #16
                                LDP             x8      , x9 , [x24], #16
                                LDP             x10     , x11, [x24], #16
                                LDP             x12     , x13, [x24], #16
                                LDP             x14     , x15, [x24], #16

                                MSR             SPSel   , #0                                        // update PSTATE to use SP_EL0
                                LDP             x25     , x26, [x24], #16                           // POP: sp, lr
                                MOV             sp      , x25
                                MOV             lr      , x26
                                MSR             SPSel   , #1                                        // update PSTATE to use SP_ELn

                                LDP             x25     , x26, [x24], #16                           // POP: pstate, pc
                                MSR             SPSR_EL1, x25
                                MSR             ELR_EL1 , x26

.endm
