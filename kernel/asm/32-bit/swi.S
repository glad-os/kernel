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



							@
							@ swi_handler
							@
							@ The SWI handler code - invoked when an SWI instruction has been executed.
							@
							@ On entry:
							@ r0 - r3 carry up to 4 parameters for the individual SWI to use
							@
							@ On exit:
							@ All registers reinstated *except* R0 which carries a status (success/failure) value appropriate to the individual SWI
							@
swi_handler:
							STMFD			sp!			, {r1-r12,lr}

							CPSIE 			i                                                   					@ OS_ReadC in busy waiting stops interrupts! (consider re-entrant handler in future)

                                                         @ use r4-r5 to calculate the relevant SWI handler function
                                                         @ make sure LR is ready to return the handler back here afterwards
                                                         ADR                      r4                      , swi_function_pointers
							LDR			r5			, [lr, #-4]							@ get SWI instruction and mask to determine SWI number
							BIC			r5			, r5		, #0xff << 24
                                                         ADR                      lr                      , swi_handler_exit
                                                         LDR                      pc                      , [r4]  , r5 , LSL #2

swi_handler_exit:
						    	LDMFD			sp!			, {r1-r12,pc}^							@ get SPSR and LR ready, then ERET (EL1->EL0)


swi_function_pointers:
                                                         .word                 _kernel_swi_os_setmode
                                                         .word                 _kernel_swi_os_setcolour
                                                         .word                 _kernel_swi_os_clearscreen
                                                         .word                 _kernel_swi_os_putpixel
                                                         .word                 _kernel_swi_os_getpixel
                                                         .word                 _kernel_swi_os_setcursorposition
                                                         .word                 _kernel_swi_os_printchar
                                                         .word                 _kernel_swi_os_printstring
                                                         .word                 _kernel_swi_os_readvideovariables
                                                         .word                 _kernel_swi_os_readc
                                                         .word                 _kernel_swi_os_processbegin
                                                         .word                 _kernel_swi_os_processexit
