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



#include <stdint.h>



#ifndef SWI_MACRO_H
#define SWI_MACRO_H



	// macro to invoke the SWI instruction [http://www.ethernut.de/en/documents/arm-inline-asm.html] -> asm(code : output operand list : input operand list : clobber list);
	// call SWI_INVOKE which allows SWI_ string constants to be expanded prior to then being injected into the SWI_GENERATE_ASM macro.
	#define SWI( n,p_in,p_out ) SWI_GENERATE_ASM(n,p_in,p_out)
	#define SWI_GENERATE_ASM( NR, PTR_IN, PTR_OUT ) __asm__ __volatile__ ( "MOV r0, %[in]\n\tMOV r1, %[out]\n\tSWI " #NR "\n\t" : : [in] "r" ( PTR_IN ), [out] "r" ( PTR_OUT ) );



#endif /*SWI_MACRO_H*/
