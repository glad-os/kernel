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
						// @todo




						.balign 8
saved_svc_sp_entrypoint:			.word		0

						.balign 8
swi_entrypoint_registers:			.word		0, 0, 0, 0

						.balign 8
swi_params_ptr_in:				.word		0

						.balign 8
swi_params_ptr_out:				.word		0

						.balign 8

