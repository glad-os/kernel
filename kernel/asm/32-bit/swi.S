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



							@
							@ swi_handler
							@
							@ The SWI handler code - invoked when an SWI instruction has been executed.
							@
							@ On entry:
							@ -
							@
							@ On exit:
							@ Ordinarily, all registers are preserved. For SWIs that are process related, all registers may be corrupted.
							@
swi_handler:
							STR			r11			, swi_entrypoint_registers + 0x0
							STR			r12			, swi_entrypoint_registers + 0x4
							STR			sp			, saved_svc_sp_entrypoint
							STR			lr			, swi_entrypoint_registers + 0xc

							STMFD			sp!			, {r0-r12,lr}

							CPSIE 			i                                                   					@ OS_ReadC in busy waiting stops interrupts! (consider re-entrant handler in future)

							ADR			r4			, swi_params_ptr_in						@ stack r0 as this points to the _kernel_regs struct (in)
							STR			r0			, [r4]
							ADR			r4			, swi_params_ptr_out						@ stack r1 as this points to the _kernel_regs struct (out)
							STR			r1			, [r4]

							LDR			r0			, [lr, #-4]							@ get SWI instruction and mask to determine SWI number
							BIC			r0			, r0		, #0xff<<24
                            				BL			_kernel_swi_handler

						    	LDMFD			sp!			, {r0-r12,pc}^							@ get SPSR and LR ready, then ERET (EL1->EL0)



saved_svc_sp_entrypoint:				.word			0
swi_entrypoint_registers:				.word			0, 0, 0, 0
swi_params_ptr_in:					.word			0
swi_params_ptr_out:					.word			0

