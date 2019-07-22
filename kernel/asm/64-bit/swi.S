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



.global						swi_handler
.global						swi_params_ptr_in, swi_params_ptr_out, swi_entrypoint_registers
.global						saved_svc_sp_entrypoint



swi_handler:

						STR lr, [sp, #-8]!

						MSR DAIFClr, #0x2 // re-enable interrupts as OS_ReadC blocks otherwise (this is what was missing yesterday - systimer can now continue!)

						LDR			x4			, =swi_params_ptr_in						// stack r0 as this points to the _kernel_regs struct (in)
						STR			x0			, [x4]
						LDR			x4			, =swi_params_ptr_out						// stack r1 as this points to the _kernel_regs struct (out)
						STR			x1			, [x4]

						// take the SVC code from the ESR (Exception Syndrome Register) and pass it in to _kernel_swi_handler
						MRS			x0			, ESR_EL1
						AND			x0			, x0		, #0xffff
						BL			_kernel_swi_handler

finish_swi_handler:
						LDR lr, [sp], #8
						RET

						

						.balign 8
swi_params_ptr_in:				.dword		0

						.balign 8
swi_params_ptr_out:				.dword		0

						.balign 8

