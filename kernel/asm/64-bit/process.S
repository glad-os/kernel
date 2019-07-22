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



.global                     				_kernel_process_push_cpu_state
.global							_kernel_process_pop_cpu_state
.global							_kernel_process_start



							//
							// _kernel_process_push_cpu_state
							//
							// ?
							//
							// On entry:
							// x0 = pointer to kernel memory where registers are to be preserved
							//
							// On exit:
							// ?
							//
_kernel_process_push_cpu_state:

							// take key registers for now (just a subset)
							// these will be pushed into the cpu_state for the process so it can become possible to return to it via OS_ProcessExit
							LDR 		x1		, =swi_entrypoint_registers			// PUSH r11,r12,SP,LR
							
							ldr x2, [x1,#0*8]
							str x2, [x0,#0*8]

							ldr x2, [x1,#1*8]
							str x2, [x0,#1*8]

							ldr x2, [x1,#2*8]
							str x2, [x0,#2*8]

							ldr x2, [x1,#3*8]
							str x2, [x0,#3*8]

							ldr x2, [x1,#4*8]
							str x2, [x0,#4*8]

							RET



							//
							// _kernel_process_pop_cpu_state
							//
							// ?
							//
							// On entry:
							// r0 = pointer to kernel memory where registers were preserved
							//
							// On exit:
							// ?
							//
_kernel_process_pop_cpu_state:
							//MRS         		x1          		, cpsr                                  		// switch to SYS mode (gain access to USR mode register bank)
                            				//BIC         		x1          		, x1        	, #0b00011111
                            				//ORR         		x1          		, x1        	, #0b00011111
                            				//MSR         		cpsr        		, x1

							//LDR			x11			, [x0, #0x0]
							//LDR			x12			, [x0, #0x4]
							//LDR			sp			, [x0, #0x8]
							//LDR			x3			, [x0, #0xc]

                            				//MRS         		x1          		, cpsr                                  		// switch to SVC mode
                            				//BIC         		x1          		, x1        	, #0b00011111
                            				//ORR         		x1          		, x1        	, #0b00010011
							//MSR			cpsr			, x1

							// ERET will return from the SWI Exception and should now safely return to userland
							// need to consider that we jump out of the kernel without 'undoing' unwanted SVC stack work at this point
							//LDR 			x0			, =saved_svc_sp_entrypoint
							//LDR 			sp			, [x0]
							//MOV			lr			, x3
						
							ldr x29, [x0, #0*8]

							ldr x30, [x0, #1*8]

							ldr x1, [x0, #2*8]
							msr SP_EL0, x1

							ldr x1, [x0, #3*8]
							msr ELR_EL1, x1

							ldr x1, [x0, #4*8]
							msr SPSR_EL1, x1

							// counteract the fact we are leaving early
							ldr x1, =saved_svc_sp_entrypoint
							ldr x2, [x1]
							mov sp, x2

                            				ERET



							//
							// _kernel_process_start
							//
							// ?
							//
							// On entry:
							// ?
							//
							// On exit:
							// ?
							//
_kernel_process_start:

							// DDI0487A, C5-300 (0 = AArch64, 0000 = EL0t)
							MOV			x0			, #0b00000	// C5-295
							MSR			SPSR_EL1		, x0

							// prepare stack pointer and link register
							MOV			x0			, #8*1024*1024				
							SUB			x0			, x0		, #16
							MSR			SP_EL0			, x0
							MOV			x0			, #4*1024*1024
							MSR			ELR_EL1			, x0
							
							// return from Exception
							ERET



