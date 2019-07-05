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



.global							put_word, get_word, delay_loop, move1k, blank1k



							//
							// put_word
							//
							// Stores a single word into a specified location in memory.
							//
							// On entry:
							// r0 = target memory location
							// r1 = word to be stored
							//
							// On exit:
							// -
							//
put_word:
    							STR 			r1			, [r0]
    							BX 			lr



							//
							// get_word
							//
							// Retrieves a single word from a specified location in memory.
							//
							// On entry:
							// r0 = memory location to be retrieved
							//
							// On exit:
							// r0 = contents of memory location
							//
get_word:
    							LDR 			r0			, [r0]
    							BX 			lr



							//
							// delay_loop
							//
							// Simple delay loop.
							//
							// On entry:
							// r0 = delay counter
							//
							// On exit:
							// r0 = 0
							//
delay_loop:
							SUBS			r0			, r0		, #1
							BHI			delay_loop
							MOV			pc			, lr



							//
							// move1k
							//
							// Moves 1Kb of memory from the source to the target memory locations.
							// No accounting for overlapping memory locations is taken into account.
							//
							// On entry:
							// r0 = pointer to source memory location
							// r1 = pointer to target memory location
							//
							// On exit:
							// r0-r1 = pointer to next locations (both source and target)
							//
move1k:
							STMFD			sp!			, {r2-r11}
							LDMIA			r0!			, {r2-r11}; STMIA r1! , {r2-r11}
							LDMIA			r0!			, {r2-r11}; STMIA r1! , {r2-r11}
							LDMIA			r0!			, {r2-r11}; STMIA r1! , {r2-r11}
							LDMIA			r0!			, {r2-r11}; STMIA r1! , {r2-r11}
							LDMIA			r0!			, {r2-r11}; STMIA r1! , {r2-r11}
							LDMIA			r0!			, {r2-r11}; STMIA r1! , {r2-r11}
							LDMIA			r0!			, {r2-r11}; STMIA r1! , {r2-r11}
							LDMIA			r0!			, {r2-r11}; STMIA r1! , {r2-r11}
							LDMIA			r0!			, {r2-r11}; STMIA r1! , {r2-r11}
							LDMIA			r0!			, {r2-r11}; STMIA r1! , {r2-r11}
							LDMIA			r0!			, {r2-r11}; STMIA r1! , {r2-r11}
							LDMIA			r0!			, {r2-r11}; STMIA r1! , {r2-r11}
							LDMIA			r0!			, {r2-r11}; STMIA r1! , {r2-r11}
							LDMIA			r0!			, {r2-r11}; STMIA r1! , {r2-r11}
							LDMIA			r0!			, {r2-r11}; STMIA r1! , {r2-r11}
							LDMIA			r0!			, {r2-r11}; STMIA r1! , {r2-r11}
							LDMIA			r0!			, {r2-r11}; STMIA r1! , {r2-r11}
							LDMIA			r0!			, {r2-r11}; STMIA r1! , {r2-r11}
							LDMIA			r0!			, {r2-r11}; STMIA r1! , {r2-r11}
							LDMIA			r0!			, {r2-r11}; STMIA r1! , {r2-r11}
							LDMIA			r0!			, {r2-r11}; STMIA r1! , {r2-r11}
							LDMIA			r0!			, {r2-r11}; STMIA r1! , {r2-r11}
							LDMIA			r0!			, {r2-r11}; STMIA r1! , {r2-r11}
							LDMIA			r0!			, {r2-r11}; STMIA r1! , {r2-r11}
							LDMIA			r0!			, {r2-r11}; STMIA r1! , {r2-r11}
							LDMIA			r0!			, {r2-r7};  STMIA r1! , {r2-r7}
							LDMFD 			sp!			, {r2-r11}
							MOV			pc			, lr



							//
							// blank1k
							//
							// Blanks (ie. sets to zero) the memory range identified by r0 (lower) and r1 (upper).
							//
							// On entry:
							// r1 = pointer to first memory location
							//
							// On exit:
							// r0-r1 = corrupted
							//
blank1k:
							STMFD 			sp!		, {r3-r12}
							MOV			r2		, #0
							MOV 			r3		, r2;		MOV 		r4		, r2
							MOV 			r5		, r2;		MOV 		r6		, r2
							MOV 			r7		, r2;		MOV 		r8		, r2
							MOV 			r9		, r2;		MOV 		r10		, r2
							MOV 			r11		, r2;		MOV 		r12		, r2
							STMIA			r1!		, {r3-r12};	STMIA		r1!		, {r3-r12}
							STMIA			r1!		, {r3-r12};	STMIA		r1!		, {r3-r12}
							STMIA			r1!		, {r3-r12};	STMIA		r1!		, {r3-r12}
							STMIA			r1!		, {r3-r12};	STMIA		r1!		, {r3-r12}
							STMIA			r1!		, {r3-r12};	STMIA		r1!		, {r3-r12}
							STMIA			r1!		, {r3-r12};	STMIA		r1!		, {r3-r12}
							STMIA			r1!		, {r3-r12};	STMIA		r1!		, {r3-r12}
							STMIA			r1!		, {r3-r12};	STMIA		r1!		, {r3-r12}
							STMIA			r1!		, {r3-r12};	STMIA		r1!		, {r3-r12}
							STMIA			r1!		, {r3-r12};	STMIA		r1!		, {r3-r12}
							STMIA			r1!		, {r3-r12};	STMIA		r1!		, {r3-r12}
							STMIA			r1!		, {r3-r12};	STMIA		r1!		, {r3-r12}
							STMIA			r1!		, {r3-r12};	STMIA		r1!		, {r3-r8}
							LDMFD 			sp!		, {r3-r12}
							MOV			pc		, lr

