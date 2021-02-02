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
                                // x0 - x3 carry up to 4 parameters for the individual SWI to use
                                //
                                // On exit:
                                // All registers reinstated *except* x0 which carries a status (success/failure) value appropriate to the individual SWI
                                //
swi_handler:
                                adr x29, mylr
                                str   x30, [x29]

                                MSR             DAIFClr , #0x2                                      // re-enable interrupts as OS_ReadC blocks otherwise

swi_dispatch_to_handler:
                                ADR             x4      , swi_function_pointers

                                MRS             x5		, ESR_EL1                                   // get SVC instruction and mask to determine SVC number
                                AND             x5		, x5, #0xffff

                                ADR             lr      , swi_handler_return
                                LDR             x4      , [x4, x5, LSL #3]
                                BR              x4

swi_handler_return:
                                ldr   x30, mylr
                                RET x30


                                .balign         8
mylr:                           .dword          0
swi_return_value:               .dword          0
swi_sp_backup:                  .dword          0
current_process_state:          .dword          0


swi_function_pointers:
                                .dword          _kernel_swi_os_printstring
                                .dword          _kernel_swi_os_printchar
                                .dword          _kernel_swi_os_clearscreen
                                .dword          _kernel_swi_os_setcolour
                                .dword          _kernel_swi_os_readc
                                .dword          _kernel_swi_os_processbegin
                                .dword          _kernel_swi_os_processexit
