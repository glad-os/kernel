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


.global                         _kernel_entry


_kernel_entry:
_kernel_exception_vectors_start:
                                LDR             pc      , reset_vector                              // 0x00000000 - reset
	    						LDR             pc      , undefined_vector                          // 0x00000004 - undefined
                                LDR             pc      , swi_vector                                // 0x00000008 - swi
	    						LDR             pc      , prefetch_vector                           // 0x0000000c - prefetch
	    						LDR             pc      , abort_vector                              // 0x00000010 - abort
	    						NOP                                                                 // 0x00000014 - reserved
	    						LDR             pc      , irq_vector                                // 0x00000018 - irq
                                LDR             pc      , fiq_vector                                // 0x0000001c - fiq


reset_vector:                   .word           reset_handler
undefined_vector:               .word           0x00000000
swi_vector:                     .word           swi_handler
prefetch_vector:                .word           0x00000000
abort_vector:                   .word           0x00000000
irq_vector:                     .word           irq_handler
fiq_vector:                     .word           0x00000000
_kernel_exception_vectors_end:


                                //
                                // reset_handler
                                //
                                // First code executed by ARM core after reset.
                                // On startup, cores 1-3 are supposedly "parked"; AArch32 is in force, Hypervisor (EL2) is in force.
                                //
                                // On entry:
                                // -
                                //
                                // On exit:
                                // -
                                //
reset_handler:
park_cores_1_to_3:
                                MRC             p15     , 0, r0, c0, c0, 5
                                MOV             r1      , #0xff
                                ANDS            r1      , r0, r1
                                BEQ             switch_to_svc_mode
parked_loop:
                                WFI
                                B               parked_loop

switch_to_svc_mode:
                                ADR             r0      , first_svc_mode_instruction                // p. 3428 discussed ELR_hyp purpose

                                MRS             r1      , cpsr_all
                                BIC             r1      , r1, #0b00011111                           // mask out existing mode
                                ORR             r1      , r1, #0b11010011                           // mask I/F, enable SVC_MODE [11]01 00[11] [SVC_MODE = EL1]

                                MSR             elr_hyp , r0                                        // prep LR and CPSR; ERET does the rest
                                MSR             spsr_hyp, r1
                                ERET

first_svc_mode_instruction:
                                BL              _kernel_start_enable_fpu
                                BL              _kernel_start_clear_bss
                                BL              _kernel_start_install_vector_table
                                BL              _kernel_start_setup_stacks
                                BL              _kernel_init

switch_to_usr_mode:
                                //MRS           r1      , cpsr
                                //BIC           r1      , r1, #0b00001111                           // mask out existing mode -> transform SVC_MODE (3) to USR_MODE (0) [EL1->EL0]
                                //MSR           spsr    , r1
                                //LDR           lr      , usr_mode_ptr
                                //ERET                                                              // ERET: return from EL1->EL0; SPSR: USR_MODE; LR: first usr mode instruction

first_usr_mode_instruction:
                                //LDR           sp      , _kernel_usr_stack_pointer
                                BL              _kernel_start_cli
infinite_loop:                  B               infinite_loop


usr_mode_ptr:                   .word           first_usr_mode_instruction


                                //
                                // _kernel_start_clear_bss
                                //
                                // Clears the BSS area of memory as defined by the values held in bss_start and bss_end.
                                // Both bss_start and bss_end must be word aligned.
                                //
                                // On entry:
                                // r0 = pointer to start of BSS area (word aligned)
                                // r1 = pointer to end of BSS area (word aligned)
                                //
                                // On exit:
                                // r0-r2 = corrupted
                                //
_kernel_start_clear_bss:
                                LDR             r0      , _kernel_bss_start
                                LDR             r1      , _kernel_bss_end
                                MOV             r2      , #0
_kernel_start_clear_bss_loop:
                                STR             r2      , [r0], #4
                                CMP             r0      , r1
                                BLT             _kernel_start_clear_bss_loop
                                MOV             pc      , lr


_kernel_bss_start:              .word           0x3b200000
_kernel_bss_end:                .word           0x3b400000


                                //
                                // _kernel_start_enable_fpu
                                //
                                // Enables the FPU unit in the ARM core
                                //
                                // On entry:
                                // -
                                //
                                // On exit:
                                // r0 = corrupted
                                //
_kernel_start_enable_fpu:
                                MRC             p15     , 0, r0, c1, c0, 2                          // Read CPACR into Rt
                                ORR             r0      , r0, #(0x0f<<20)
                                MCR             p15     , 0, r0, c1, c0, 2                          // Write Rt to CPACR

                                MRC             p15     , 0, r0, c1, c0, #2                         // R0 = Access Control Register
                                ORR             r0      , #(0x00300000 + 0x00C00000)                // Enable Single & Double Precision
                                MCR             p15     , 0, r0, c1, c0, #2                         // Access Control Register = R0
                                MOV             r0      , #0x40000000                               // R0 = Enable VFP
                                VMSR            fpexc   , r0                                        // FPEXC = R0
                                MOV             pc      , lr


                                //
                                // _kernel_start_setup_stacks
                                //
                                // Initialises stack pointers for IRQ, FIQ and SVC modes.
                                // Stack pointers are expected to be full and descending.
                                //
                                // On entry:
                                // -
                                //
                                // On exit:
                                // SP registers for IRQ, FIQ and SVC modes point to pre-defined stack areas within kernel space
                                //
_kernel_start_setup_stacks:
_kernel_start_setup_irq_stack:
                                MSR             cpsr_c  , #0b11000000 | 0b10010                     // switch to IRQ Mode, disabling I/F
                                LDR             sp      , _kernel_irq_stack_pointer
_kernel_start_setup_fiq_stack:
                                MSR             cpsr_c  , #0b11000000 | 0b10001                     // switch to FIQ Mode, disabling I/F
                                LDR             sp      , _kernel_fiq_stack_pointer
_kernel_start_setup_svc_stack:
                                MSR             cpsr_c  , #0b11000000 | 0b10011                     // switch to SVC Mode, disabling I/F
                                LDR             sp      , _kernel_svc_stack_pointer
                                MOV             pc      , lr


_kernel_irq_stack_pointer:      .word           _kernel_irq_stack + 2048 - 4
_kernel_fiq_stack_pointer:      .word           _kernel_fiq_stack + 2048 - 4
_kernel_svc_stack_pointer:      .word           _kernel_svc_stack + 2048 - 4
_kernel_usr_stack_pointer:      .word           _kernel_usr_stack + 2048 - 4
_kernel_irq_stack:              .skip           2048
_kernel_fiq_stack:              .skip           2048
_kernel_svc_stack:              .skip           2048
_kernel_usr_stack:              .skip           2048


                                //
                                // _kernel_start_install_vector_table
                                //
                                // Installs the exception vectors from the kernel into the ARM memory space starting at 0.
                                //
                                // On entry:
                                // -
                                //
                                // On exit:
                                // r0-r3 = corrupted
                                //
_kernel_start_install_vector_table:
                                MOV             r0      , #0x3B000000
                                MOV             r1      , #0x00000000
                                MOV             r2      , #( _kernel_exception_vectors_end - _kernel_exception_vectors_start )
_kernel_start_install_vectors_loop:
                                LDR             r3      , [r0], #4
                                STR             r3      , [r1], #4
                                SUBS            r2      , r2, #4
                                BNE             _kernel_start_install_vectors_loop
                                MOV             pc      , lr
