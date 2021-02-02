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


.global                         put_word, get_word, delay_loop, move1k, blank1k


put_word:
                                // todo
                                STR 		w1		, [x0]
                                RET


get_word:
                                // todo
                                LDR 		w0		, [x0]
                                RET


                                //
                                // delay_loop
                                //
                                // Simple delay loop.
                                //
                                // On entry:
                                // x0 = delay counter
                                //
                                // On exit:
                                // x0 = 0
                                //
delay_loop:
                                STR			lr			, [sp, #-16]!
delay_loop2:
                                subs x0,x0,#1
                                b.ne			delay_loop2 // not bhi
                                LDR			lr			, [sp]		, #16

                                RET

							
                                //
                                // move1k
                                //
                                // Moves 1Kb of memory from the source to the target memory locations.
                                // No accounting for overlapping memory locations is taken into account.
                                //
                                // On entry:
                                // x0 = pointer to source memory location
                                // x1 = pointer to target memory location
                                //
                                // On exit:
                                // x0-x1 = pointer to next locations (both source and target)
                                //
move1k:
                                STP             x2      , x3, [sp, #-1 * 16]
                                STP             x4      , x5, [sp, #-2 * 16]
                                STP             x6      , x7, [sp, #-3 * 16]
                                STP             x8      , x9, [sp, #-4 * 16]

                                LDP             x2      , x3, [x0], #16; STP x2, x3, [x1], #16
                                    LDP             x4      , x5, [x0], #16; STP x4, x5, [x1], #16
                                    LDP             x6      , x7, [x0], #16; STP x6, x7, [x1], #16
                                    LDP             x8      , x9, [x0], #16; STP x8, x9, [x1], #16
                                LDP             x2      , x3, [x0], #16; STP x2, x3, [x1], #16
                                    LDP             x4      , x5, [x0], #16; STP x4, x5, [x1], #16
                                    LDP             x6      , x7, [x0], #16; STP x6, x7, [x1], #16
                                    LDP             x8      , x9, [x0], #16; STP x8, x9, [x1], #16
                                LDP             x2      , x3, [x0], #16; STP x2, x3, [x1], #16
                                    LDP             x4      , x5, [x0], #16; STP x4, x5, [x1], #16
                                    LDP             x6      , x7, [x0], #16; STP x6, x7, [x1], #16
                                    LDP             x8      , x9, [x0], #16; STP x8, x9, [x1], #16
                                LDP             x2      , x3, [x0], #16; STP x2, x3, [x1], #16
                                    LDP             x4      , x5, [x0], #16; STP x4, x5, [x1], #16
                                    LDP             x6      , x7, [x0], #16; STP x6, x7, [x1], #16
                                    LDP             x8      , x9, [x0], #16; STP x8, x9, [x1], #16
                                LDP             x2      , x3, [x0], #16; STP x2, x3, [x1], #16
                                    LDP             x4      , x5, [x0], #16; STP x4, x5, [x1], #16
                                    LDP             x6      , x7, [x0], #16; STP x6, x7, [x1], #16
                                    LDP             x8      , x9, [x0], #16; STP x8, x9, [x1], #16
                                LDP             x2      , x3, [x0], #16; STP x2, x3, [x1], #16
                                    LDP             x4      , x5, [x0], #16; STP x4, x5, [x1], #16
                                    LDP             x6      , x7, [x0], #16; STP x6, x7, [x1], #16
                                    LDP             x8      , x9, [x0], #16; STP x8, x9, [x1], #16
                                LDP             x2      , x3, [x0], #16; STP x2, x3, [x1], #16
                                    LDP             x4      , x5, [x0], #16; STP x4, x5, [x1], #16
                                    LDP             x6      , x7, [x0], #16; STP x6, x7, [x1], #16
                                    LDP             x8      , x9, [x0], #16; STP x8, x9, [x1], #16
                                LDP             x2      , x3, [x0], #16; STP x2, x3, [x1], #16
                                    LDP             x4      , x5, [x0], #16; STP x4, x5, [x1], #16
                                    LDP             x6      , x7, [x0], #16; STP x6, x7, [x1], #16
                                    LDP             x8      , x9, [x0], #16; STP x8, x9, [x1], #16
                                LDP             x2      , x3, [x0], #16; STP x2, x3, [x1], #16
                                    LDP             x4      , x5, [x0], #16; STP x4, x5, [x1], #16
                                    LDP             x6      , x7, [x0], #16; STP x6, x7, [x1], #16
                                    LDP             x8      , x9, [x0], #16; STP x8, x9, [x1], #16
                                LDP             x2      , x3, [x0], #16; STP x2, x3, [x1], #16
                                    LDP             x4      , x5, [x0], #16; STP x4, x5, [x1], #16
                                    LDP             x6      , x7, [x0], #16; STP x6, x7, [x1], #16
                                    LDP             x8      , x9, [x0], #16; STP x8, x9, [x1], #16
                                LDP             x2      , x3, [x0], #16; STP x2, x3, [x1], #16
                                    LDP             x4      , x5, [x0], #16; STP x4, x5, [x1], #16
                                    LDP             x6      , x7, [x0], #16; STP x6, x7, [x1], #16
                                    LDP             x8      , x9, [x0], #16; STP x8, x9, [x1], #16
                                LDP             x2      , x3, [x0], #16; STP x2, x3, [x1], #16
                                    LDP             x4      , x5, [x0], #16; STP x4, x5, [x1], #16
                                    LDP             x6      , x7, [x0], #16; STP x6, x7, [x1], #16
                                    LDP             x8      , x9, [x0], #16; STP x8, x9, [x1], #16
                                LDP             x2      , x3, [x0], #16; STP x2, x3, [x1], #16
                                    LDP             x4      , x5, [x0], #16; STP x4, x5, [x1], #16
                                    LDP             x6      , x7, [x0], #16; STP x6, x7, [x1], #16
                                    LDP             x8      , x9, [x0], #16; STP x8, x9, [x1], #16
                                LDP             x2      , x3, [x0], #16; STP x2, x3, [x1], #16
                                    LDP             x4      , x5, [x0], #16; STP x4, x5, [x1], #16
                                    LDP             x6      , x7, [x0], #16; STP x6, x7, [x1], #16
                                    LDP             x8      , x9, [x0], #16; STP x8, x9, [x1], #16
                                LDP             x2      , x3, [x0], #16; STP x2, x3, [x1], #16
                                    LDP             x4      , x5, [x0], #16; STP x4, x5, [x1], #16
                                    LDP             x6      , x7, [x0], #16; STP x6, x7, [x1], #16
                                    LDP             x8      , x9, [x0], #16; STP x8, x9, [x1], #16
                                LDP             x2      , x3, [x0], #16; STP x2, x3, [x1], #16
                                    LDP             x4      , x5, [x0], #16; STP x4, x5, [x1], #16
                                    LDP             x6      , x7, [x0], #16; STP x6, x7, [x1], #16
                                    LDP             x8      , x9, [x0], #16; STP x8, x9, [x1], #16

                                LDP             x2      , x3, [sp, #-1 * 16]
                                LDP             x4      , x5, [sp, #-2 * 16]
                                LDP             x6      , x7, [sp, #-3 * 16]
                                LDP             x8      , x9, [sp, #-4 * 16]
                                RET


                                //
                                // blank1k
                                //
                                // Blanks (ie. sets to zero) the memory range identified by r0 (lower) and r1 (upper).
                                //
                                // On entry:
                                // x0 = pointer to memory to blank
                                // x1 = number of 1kb blocks to blank
                                //
                                // On exit:
                                // x0-x1 = corrupted
                                //
blank1k:
                                STP             x2      , x3, [sp, #-1 * 16]
                                STP             x4      , x5, [sp, #-2 * 16]
                                STP             x6      , x7, [sp, #-3 * 16]
                                STP             x8      , x9, [sp, #-4 * 16]

                                MOV             x2      , #0
                                MOV             x3      , #0
                                MOV             x4      , #0
                                MOV             x5      , #0
                                MOV             x6      , #0
                                MOV             x7      , #0
                                MOV             x8      , #0
                                MOV             x9      , #0
blank1k_loop:

                                STP x2, x3, [x0], #16
                                    STP x4, x5, [x0], #16
                                    STP x6, x7, [x0], #16
                                    STP x8, x9, [x0], #16
                                STP x2, x3, [x0], #16
                                    STP x4, x5, [x0], #16
                                    STP x6, x7, [x0], #16
                                    STP x8, x9, [x0], #16
                                STP x2, x3, [x0], #16
                                    STP x4, x5, [x0], #16
                                    STP x6, x7, [x0], #16
                                    STP x8, x9, [x0], #16
                                STP x2, x3, [x0], #16
                                    STP x4, x5, [x0], #16
                                    STP x6, x7, [x0], #16
                                    STP x8, x9, [x0], #16
                                STP x2, x3, [x0], #16
                                    STP x4, x5, [x0], #16
                                    STP x6, x7, [x0], #16
                                    STP x8, x9, [x0], #16
                                STP x2, x3, [x0], #16
                                    STP x4, x5, [x0], #16
                                    STP x6, x7, [x0], #16
                                    STP x8, x9, [x0], #16
                                STP x2, x3, [x0], #16
                                    STP x4, x5, [x0], #16
                                    STP x6, x7, [x0], #16
                                    STP x8, x9, [x0], #16
                                STP x2, x3, [x0], #16
                                    STP x4, x5, [x0], #16
                                    STP x6, x7, [x0], #16
                                    STP x8, x9, [x0], #16
                                STP x2, x3, [x0], #16
                                    STP x4, x5, [x0], #16
                                    STP x6, x7, [x0], #16
                                    STP x8, x9, [x0], #16
                                STP x2, x3, [x0], #16
                                    STP x4, x5, [x0], #16
                                    STP x6, x7, [x0], #16
                                    STP x8, x9, [x0], #16
                                STP x2, x3, [x0], #16
                                    STP x4, x5, [x0], #16
                                    STP x6, x7, [x0], #16
                                    STP x8, x9, [x0], #16
                                STP x2, x3, [x0], #16
                                    STP x4, x5, [x0], #16
                                    STP x6, x7, [x0], #16
                                    STP x8, x9, [x0], #16
                                STP x2, x3, [x0], #16
                                    STP x4, x5, [x0], #16
                                    STP x6, x7, [x0], #16
                                    STP x8, x9, [x0], #16
                                STP x2, x3, [x0], #16
                                    STP x4, x5, [x0], #16
                                    STP x6, x7, [x0], #16
                                    STP x8, x9, [x0], #16
                                STP x2, x3, [x0], #16
                                    STP x4, x5, [x0], #16
                                    STP x6, x7, [x0], #16
                                    STP x8, x9, [x0], #16
                                STP x2, x3, [x0], #16
                                    STP x4, x5, [x0], #16
                                    STP x6, x7, [x0], #16
                                    STP x8, x9, [x0], #16

                                SUBS            x1      , x1 , #1
                                BNE             blank1k_loop

                                LDP             x2      , x3, [sp, #-1 * 16]
                                LDP             x4      , x5, [sp, #-2 * 16]
                                LDP             x6      , x7, [sp, #-3 * 16]
                                LDP             x8      , x9, [sp, #-4 * 16]
                                RET
