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
							// https://developer.arm.com/docs/100933/latest/aarch64-exception-and-interrupt-handling
							//
irq_handler:
							STP			x0			, x1		, [sp, #-16]!
							STP			x2			, x3		, [sp, #-16]!
							STP			x4			, x5		, [sp, #-16]!
							STP			x6			, x7		, [sp, #-16]!
							STP			x8			, x9		, [sp, #-16]!
							STP			x10			, x11		, [sp, #-16]!
							STP			x12			, x13		, [sp, #-16]!
							STP			x14			, x15		, [sp, #-16]!

							BL			_kernel_interrupt_handler

							LDP			x14			, x15		, [sp]	, #16
							LDP			x12			, x13		, [sp]	, #16
							LDP			x10			, x11		, [sp]	, #16
							LDP			x8			, x9		, [sp]	, #16
							LDP			x6			, x7		, [sp]	, #16
							LDP			x4			, x5		, [sp]	, #16
							LDP			x2			, x3		, [sp]	, #16
							LDP			x0			, x1		, [sp]	, #16

							ERET



_kernel_interrupt_enable_interrupts:
							MSR			DAIFClr			, #0x7
							RET



_kernel_interrupt_disable_interrupts:
							MSR			DAIFClr			, #0x0
							RET

