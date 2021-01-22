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


.global                         swi_handler
.global                         current_process_state


#include                        "macros.S"


                                //
                                // swi_handler
                                //
                                // The SWI handler code - invoked when an SWI instruction has been executed.
                                //
                                // On entry:
                                // r0 - r3 carry up to 4 parameters for the individual SWI to use
                                //
                                // On exit:
                                // All registers reinstated *except* R0 which carries a status (success/failure) value appropriate to the individual SWI
                                //
swi_handler:
                                _PUSH_CPU_STATE swi_sp_backup, current_process_state                // PUSH cpu state

                                CPSIE           i

swi_dispatch_to_handler:
                                ADR             r4      , swi_function_pointers
                                LDR             r5      , [lr, #-4]                                 // get SWI instruction and mask to determine SWI number
                                BIC             r5      , r5, #0xff << 24
                                ADR             lr      , swi_handler_return
                                LDR             pc      , [r4, r5, LSL #2]

swi_handler_return:
                                STR             r0      , swi_return_value                          // preserve r0 (this is the return value)

                                LDR             r0      , current_process_state                     // POP cpu state
                                _POP_CPU_STATE

                                LDR             r0      , swi_return_value                          // reinstate the return value, and return...
                                MOVS            pc      , lr


swi_return_value:               .word           0
swi_sp_backup:                  .word           0
current_process_state:          .word           0


swi_function_pointers:
                                .word           _kernel_swi_os_printstring
                                .word           _kernel_swi_os_printchar
                                .word           _kernel_swi_os_clearscreen
                                .word           _kernel_swi_os_setcolour
                                .word           _kernel_swi_os_readc
                                .word           _kernel_swi_os_processbegin
                                .word           _kernel_swi_os_processexit
