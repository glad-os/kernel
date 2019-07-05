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



.global 						irq_handler
.global							_kernel_interrupt_enable_interrupts



							//
							// irq_handler
							//
							// The IRQ handler code, called to service any individual IRQ that arrives at the ARM core.
							//
							// On entry:
							// -
							//
							// On exit:
							// -
							//
							// References:
							// [1] http://www.ic.unicamp.br/~celio/mc404-2013/arm-manuals/ARM_exception_slides.pdf
							// [2] F7.1.64 LDM (exception return) [ARM Architecture Reference Manual]
							//
irq_handler:
							SUB 			lr		,	lr		, #4					// [1] with an IRQ, the PC is 1 instruction beyond the instruction causing the exception
							STMFD			sp!		,	{r0-r12,lr}
							BL			_kernel_interrupt_handler
							LDMFD			sp!		,	{r0-r12,pc}^						// [2]



_kernel_interrupt_enable_interrupts:
							CPSIE			i
							MOV			pc		,	lr



_kernel_interrupt_disable_interrupts:
							CPSID			if
							MOV			pc		,	lr

