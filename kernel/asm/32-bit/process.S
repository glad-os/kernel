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



.global                     _kernel_process_push_cpu_state, _kernel_process_pop_cpu_state, _kernel_process_start



							@
							@ _kernel_process_push_cpu_state
							@
							@ ?
							@
							@ On entry:
							@ r0 = pointer to kernel memory where registers are to be preserved
							@
							@ On exit:
							@ ?
							@
_kernel_process_push_cpu_state:
                            MRS         r1          , cpsr                                  @ switch to SYS mode (gain access to USR mode register bank)
                            BIC         r1          , r1        , #0b00011111
                            ORR         r1          , r1        , #0b00011111
                            MSR         cpsr        , r1

							// here we will take r11/r12/lr (automatically preserved at the point of SWI entry)
							// and we will - whilst in SYS mode - take the SP as well
							LDR 		r1			, temp_lr_address						@ PUSH r11,r12,SP,LR
							LDMIA		r1			, {r2-r5}
							MOV			r4			, sp
							STMIA		r0			, {r2-r5}

                            MRS         r1          , cpsr                                  @ switch to SVC mode
                            BIC         r1          , r1        , #0b00011111
                            ORR         r1          , r1        , #0b00010011
                            MSR         cpsr        , r1

							MOV			pc			, lr



temp_lr_address:            .word       swi_entrypoint_registers



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
							MRS         r1          , cpsr                                  @ switch to SYS mode (gain access to USR mode register bank)
                            BIC         r1          , r1        , #0b00011111
                            ORR         r1          , r1        , #0b00011111
                            MSR         cpsr        , r1

							LDR			r11			, [r0, #0x0]
							LDR			r12			, [r0, #0x4]
							LDR			sp			, [r0, #0x8]
							LDR			r3			, [r0, #0xc]

                            MRS         r1          , cpsr                                  @ switch to SVC mode
                            BIC         r1          , r1        , #0b00011111
                            ORR         r1          , r1        , #0b00010011
							MSR			cpsr		, r1

							// ERET will return from the SWI Exception and should now safely return to userland
							// need to consider that we jump out of the kernel without 'undoing' unwanted SVC stack work at this point
							LDR 		r0			, svc_sp_value
							LDR 		sp			, [r0]
							MOV			lr			, r3
                            ERET



svc_sp_value:				.word		saved_svc_sp_entrypoint



							@
							@ _kernel_process_start
							@
							@ ?
							@
							@ On entry:
							@ ?
							@
							@ On exit:
							@ ?
							@
_kernel_process_start:
							MRS         r1          , cpsr                                  @ switch to SYS mode (gain access to USR mode register bank)
                            BIC         r1          , r1        , #0b00011111
                            ORR         r1          , r1        , #0b00011111
                            MSR         cpsr        , r1

							MOV         sp          , #8*1024*1024							@ prepare SP
							SUB			sp			, sp		, #4

							MRS         r1          , cpsr                                  @ switch to SVC mode
                            BIC         r1          , r1        , #0b00011111
                            ORR         r1          , r1        , #0b00010011
                            MSR         cpsr        , r1

                            // ERET will return from the SWI Exception and should now safely return to userland
                            // need to consider that we jump out of the kernel without 'undoing' unwanted stack work at this point
							LDR r0, svc_sp_value
                            LDR sp, [r0]
							MOV			lr			, #4*1024*1024
                            ERET
