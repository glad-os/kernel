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
							// r0 = pointer to kernel memory where registers are to be preserved
							//
							// On exit:
							// ?
							//
_kernel_process_push_cpu_state:
                            				//MRS         		x1          		, cpsr                                  		// switch to SYS mode (gain access to USR mode register bank)
                            				//BIC         		x1          		, x1        	, #0b00011111
                            				//ORR         		x1          		, x1        	, #0b00011111
                            				//MSR         		cpsr        		, x1

							// here we will take r11/r12/lr (automatically preserved at the point of SWI entry)
							// and we will - whilst in SYS mode - take the SP as well
							LDR 			x1			, =swi_entrypoint_registers				// PUSH r11,r12,SP,LR
							LDMIA			x1			, {x2-x5}
							MOV			x4			, sp
							STMIA			x0			, {x2-x5}

                            				//MRS         		x1          		, cpsr                                  		// switch to SVC mode
                            				//BIC         		x1          		, x1        	, #0b00011111
                            				//ORR         		x1          		, x1        	, #0b00010011
                            				//MSR         		cpsr        		, x1

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

							LDR			x11			, [x0, #0x0]
							LDR			x12			, [x0, #0x4]
							LDR			sp			, [x0, #0x8]
							LDR			x3			, [x0, #0xc]

                            				//MRS         		x1          		, cpsr                                  		// switch to SVC mode
                            				//BIC         		x1          		, x1        	, #0b00011111
                            				//ORR         		x1          		, x1        	, #0b00010011
							//MSR			cpsr			, x1

							// ERET will return from the SWI Exception and should now safely return to userland
							// need to consider that we jump out of the kernel without 'undoing' unwanted SVC stack work at this point
							LDR 			x0			, =saved_svc_sp_entrypoint
							LDR 			sp			, [x0]
							MOV			lr			, x3
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
							//MRS         		x1          		, cpsr                                  		// switch to SYS mode (gain access to USR mode register bank)
                            				//BIC         		x1          		, x1        	, #0b00011111
                            				//ORR         		x1          		, x1        	, #0b00011111
                            				//MSR         		cpsr        		, x1

							MOV         		sp          		, #8*1024*1024						// prepare SP
							SUB			sp			, sp		, #4

							//MRS         		x1          		, cpsr                                  		// switch to SVC mode
                            				//BIC         		x1          		, x1        	, #0b00011111
                            				//ORR         		x1          		, x1        	, #0b00010011
                            				//MSR         		cpsr        		, x1

							// ERET will return from the SWI Exception and should now safely return to userland
							// need to consider that we jump out of the kernel without 'undoing' unwanted stack work at this point
							LDR			x0			, =saved_svc_sp_entrypoint
							LDR			sp			, [x0]
							MOV			lr			, #4*1024*1024
							ERET

