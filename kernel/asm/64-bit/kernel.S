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


.global 						_kernel_entry, _kernel_peek_value
.global                         _kernel_debug_sp
.global                         swi_entrypoint_registers
.global                         saved_svc_sp_entrypoint


#include                        "macros.S"


                                //
                                // _kernel_entry
                                //
                                // First code executed by ARM core after reset.
                                // On startup, cores 1-3 are supposedly "parked"; AArch64 is in force, EL3 is in force.
                                //
                                // On entry:
                                // -
                                //
                                // On exit:
                                // -
                                //
_kernel_entry:

park_cores_1_to_3:
                                MRS			x0			, MPIDR_EL1						// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0500j/BABHBJCI.html
					        	MOV     		x1			, #0xff
					        	ANDS    		x0			, x0		, x1
					        	BEQ     		first_core_instruction
parked_loop:
					        	WFI
       							B			parked_loop

first_core_instruction:
                                //
                                // if required, the MMU code will need to write to the CPUECTLR (CPU Extended Control Register) register (in order to enable 
                                // the SMPEN bit); write access to this register must first be enabled explicitly from the higher EL. Here, both EL2 and EL1 
                                // must be explicitly allowed write access to ensure this is possible.
                                //
                                // ACTLR_EL3 (Cortex-A53) characteristics: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.100403_0200_00_en/lau1443434859896.html
                                // ACTLR_EL2 (Cortex-A53) characteristics: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.100403_0200_00_en/lau1443434859896.html
                                //
enable_cpuectlr_write_access:
                                MRS			x0			, ACTLR_EL3
                                ORR			x0			, x0		, #(1<<1)				// CPUECTLR_EL2 write accessible from EL2
                                MSR			ACTLR_EL3		, x0
                                MRS			x0			, ACTLR_EL2
                                ORR			x0			, x0		, #(1<<1)				// CPUECTLR_EL1 write accessible from EL1
                                MSR			ACTLR_EL2		, x0

drop_from_el3_to_el2:
                                MSR			SCTLR_EL2		, xzr
                                MOV			x0			, #0b01001 						// DDI0487A, C5-300 (0 = AArch64, 1001 = EL2)
                                MSR			SPSR_EL3		, x0
                                ADR			x0			, el2_entry
                                MSR			ELR_EL3			, x0
                                ERET

el2_entry:
                                //
                                // the following changes to HCR_EL2 ensure that various operations are *not* elevated up to EL2 (Hypervisor) but instead remain
                                // routed to EL1 (Supervisor). Of particular interest here is the bit which prevent trapping of TLB maintenance instructions, 
                                // and also the bit which mean EL1&0 Stage 2 address translations are DISABLED (which is MMU-related).
                                //
dont_trap_vm_reg_writes_to_el2:
                                MRS			x0			, HCR_EL2
                                BIC			x0			, x0		, #(1<<30)				// clear bit 26 (TRVM) which means DON'T trap non-secure EL1 reads to the VM registers to EL2
                                BIC			x0			, x0		, #(1<<27)				// clear bit 27 (TGE)  which means DON'T trap general exceptions to EL2
                                BIC			x0			, x0		, #(1<<26)				// clear bit 26 (TVM)  which means DON'T trap non-secure EL1 writes to the VM registers to EL2
                                BIC			x0			, x0		, #(1<<25)				// clear bit 25 (TTLB) which means DON'T trap TLB maintenance instructions to EL2
                                BIC			x0			, x0		, #(1<<21) 				// clear bit 21 (TACR) which means DON'T trap access to the Auxiliary Control Registers to EL2
                                BIC			x0			, x0		, #(1<< 0)				// clear bit  0 (VM)   which means EL1&0 stage 2 address translations are DISABLED (only stage 1 for EL1&0 please)
                                MSR			HCR_EL2			, x0

                                //
                                // HCR_EL2 determines whether various (non-secure) operations are trapped to EL2.
                                // Currently this is of no interest - only EL1 is relevant, so ensure relevant operations are not trapped.
                                //
prevent_exception_routing_to_el2:
                                MRS			x0			, HCR_EL2
                                BIC			x0			, x0		, #(1<<5)				// AMO (Async external abort, and SError interrupt routing)
                                BIC			x0			, x0		, #(1<<4)				// physical IRQ routing
                                BIC			x0			, x0		, #(1<<3)				// physical FIQ routing
                                MSR			HCR_EL2			, x0

set_el1_as_aarch64:
                                MRS			x0			, HCR_EL2						// HCR = "Hypervisor Control Register"
                                ORR			x0			, x0, #(1<<31)						// EL1 is AArch64
                                MSR			HCR_EL2			, x0
					
drop_from_el2_to_el1:
                                MSR			SCTLR_EL1		, xzr
                                MOV			x0			, #0b00101 						// DDI0487A, C5-300 (0 = AArch64, 0101 = EL1)
                                MSR			SPSR_EL2		, x0
                                ADR			x0			, el1_entry
                                MSR			ELR_EL2			, x0
                                ERET

el1_entry:
                                MSR SPSel,#1 // ACU - in EL1, please us SP_EL1 and don't start sharing SP_EL0
                                BL			_kernel_start_enable_fpu
                                BL			_kernel_start_clear_bss
                                BL			_kernel_start_setup_stacks
                                BL 			_kernel_start_install_vector_table
                                BL			_kernel_init

switch_to_el0:
                                //MOV			x0			, #0b00000						// DDI0487A, C5-300 (0 = AArch64, 0000 = EL0t)
                                //MSR			SPSR_EL1		, x0
                                //ADR			x0			, el0_entry
                                //MSR			ELR_EL1			, x0
                                //ERET
//el0_entry:
                                BL              _kernel_start_cli
infinite_loop:					B               infinite_loop


                                // ------------------------------------------------------------------------------------------------------------------------------------------------
                                //
                                //
                                // sync handler - exceptions (SVC, etc.) are synchronous exceptions!
                                //
                                // if EL2 or EL3, set them up and return, simple as that for now
                                //
                                // CurrentEL : ARM Architecture Reference Manual ARMv8 (DDI0487A) [C5-265]
                                //
                                //
                                // ------------------------------------------------------------------------------------------------------------------------------------------------

sync_handler:
                                _PUSH_CPU_STATE swi_sp_backup, current_process_state                // PUSH cpu state

                                MRS             x24      , esr_el1
                                LSR             x24      , x24, #26                                 // exception class (top 6-bits identify the class of sync exception that occurred - quite a few to choose from)
                                CMP             x24      , #0b010101                                // SVC in 64-bit state
                                B.EQ            el0_svc
stick_around:                   B               stick_around

el0_svc:
                                BL              swi_handler
                                STR             x0      , [sp, #16*0]                               // stack the return value at this point

                                ADR             x0      , current_process_state                     // POP cpu state
                                LDR             x0      , [x0]
                                _POP_CPU_STATE
                                
                                LDR             x0      , [sp, #16*0]                               // grab the value to return
                                ERET


_kernel_debug_sp:
                                STR             lr      , [sp, #-8]!
                                MOV             x0      , sp
                                BL              _kernel_video_print_hex
                                LDR             lr      , [sp], #8
                                RET


							// ------------------------------------------------------------------------------------------------------------------------------------------------
							//
							//
							// END OF SYNC HANDLER CODE
							//
							//
							// ------------------------------------------------------------------------------------------------------------------------------------------------


the_irq_handler:
							sub	sp, sp, #256
							stp	x0, x1, [sp, #16 * 0]
							stp	x2, x3, [sp, #16 * 1]
							stp	x4, x5, [sp, #16 * 2]
							stp	x6, x7, [sp, #16 * 3]
							stp	x8, x9, [sp, #16 * 4]
							stp	x10, x11, [sp, #16 * 5]
							stp	x12, x13, [sp, #16 * 6]
							stp	x14, x15, [sp, #16 * 7]
							stp	x16, x17, [sp, #16 * 8]
							stp	x18, x19, [sp, #16 * 9]
							stp	x20, x21, [sp, #16 * 10]
							stp	x22, x23, [sp, #16 * 11]
							stp	x24, x25, [sp, #16 * 12]
							stp	x26, x27, [sp, #16 * 13]
							stp	x28, x29, [sp, #16 * 14]
							str	x30, [sp, #16 * 15] 

							BL _kernel_interrupt_handler

							ldp	x0, x1, [sp, #16 * 0]
							ldp	x2, x3, [sp, #16 * 1]
							ldp	x4, x5, [sp, #16 * 2]
							ldp	x6, x7, [sp, #16 * 3]
							ldp	x8, x9, [sp, #16 * 4]
							ldp	x10, x11, [sp, #16 * 5]
							ldp	x12, x13, [sp, #16 * 6]
							ldp	x14, x15, [sp, #16 * 7]
							ldp	x16, x17, [sp, #16 * 8]
							ldp	x18, x19, [sp, #16 * 9]
							ldp	x20, x21, [sp, #16 * 10]
							ldp	x22, x23, [sp, #16 * 11]
							ldp	x24, x25, [sp, #16 * 12]
							ldp	x26, x27, [sp, #16 * 13]
							ldp	x28, x29, [sp, #16 * 14]
							ldr	x30, [sp, #16 * 15] 
							add	sp, sp, #256		

							ERET	// is this a problem? I'm in the middle of an existing SVC Exception, an IRQ is allowed, triggered, and I end up here - does this cause issue?!
							

sync_handler_message:
							.ascii			"<SYNC HANDLER>"
							.byte			0
							.balign			8


_kernel_echo_currentel:
			str lr, [sp,#-8]!
			mrs x0,ELR_EL1
			bl _kernel_video_print_hex
			ldr lr,[sp],#8
			ret

fiq_handler:						MOV			x0			, #66
							BL			_kernel_video_print_char
							B			fiq_handler



serror_handler:						MOV			x0			, #67
							BL			_kernel_video_print_char
							B			serror_handler



							//
							// _kernel_start_clear_bss
							//
							// Clears the BSS area of memory as defined by the values held in bss_start and bss_end.
							// Both bss_start and bss_end must be word aligned.
							//
_kernel_start_clear_bss:
							LDR			x0			, _kernel_bss_start
							LDR			x1			, _kernel_bss_end
							MOV			w2			, #0
_kernel_start_clear_bss_loop:
							STR			w2			, [x0]		, #4
							CMP			x0			, x1
							BLT			_kernel_start_clear_bss_loop
							RET



							.balign			8
_kernel_bss_start:					.dword			0x3b200000
_kernel_bss_end:					.dword			0x3b400000



							//
							// _kernel_start_enable_fpu
							//
							// Enables the FPU unit in the ARM core (nothing to do in AArch64 - access already available
							// but ensure that trapping is disabled for FP-related references in EL1/EL0)
							//
_kernel_start_enable_fpu:
							MOV			x1			, #(0x3 << 20)
							MSR			CPACR_EL1		, x1
							ISB
							RET



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
_kernel_start_setup_el1_stack:
							LDR			x0			, _kernel_el1_stack_pointer
							MOV			sp			, x0
						    	RET


							.align			8
_kernel_el1_stack_pointer:				.dword			_kernel_el1_stack + (4096*1)

							.balign			16
_kernel_el1_stack:					.skip			8192*1



							//
							// _kernel_start_install_vector_table
							//
							// Installs the exception vectors from the kernel into the ARM memory space starting at 0.
							//
_kernel_start_install_vector_table:
							LDR 			x1			, = vector_table
							MSR			VBAR_EL1		, x1
							RET



							.balign			2048
vector_table:							
							B			sync_handler;	.balign 128		// Current EL with SP0
							B			the_irq_handler;	.balign 128
							B			fiq_handler;	.balign 128
							B			serror_handler;	.balign 128

							B			sync_handler;	.balign 128		// Current EL with SPx
							B			the_irq_handler;	.balign 128
							B			fiq_handler;	.balign 128
							B			serror_handler;	.balign 128

							B			sync_handler;	.balign 128		// From a lower EL (Aarch64)
							B			the_irq_handler;	.balign 128
							B			fiq_handler;	.balign 128
							B			serror_handler;	.balign 128

							B			sync_handler;	.balign 128		// From a lower EL (Aarch32)
							B			the_irq_handler;	.balign 128
							B			fiq_handler;	.balign 128
							B			serror_handler;	.balign 128

