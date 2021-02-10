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
.global                         _fastcharplot

                                /*
                                 * _fastcharplot
                                 *
                                 * On entry:
                                 * x0 = ASCII value of char to plot
                                 *
                                 * Plots 8x8 ASCII char on-screen at VRAM position
                                 * currently identified by value held in cursor_vram_address
                                 */

_fastcharplot:
                                LDR             x1      , _fontdata_8x8_ptr                         // x0 contains *entire* 8 bytes of ASCII "bit" data
                                LDR             x0      , [x1, x0, LSL #3]                          

                                LDR             x1      , _fontdata_64bit_ptr                       // x1 points to 64-bit font LUT

                                LDR             x2      , _cursor_vram_address_ptr                  // x2 points to VRAM
                                LDR             x2      , [x2]

                                MOV             x8      , #8
                                LDR             x9      , _colour_f_ptr                             // x9 is the foreground colour
                                LDR             x9      , [x9]
                                LDR             x10     , _colour_b_ptr                             // x10 is the background colour
                                LDR             x10     , [x10]

_fastcharplot_loop:
                                AND             x3      , x0, #0xff                                 // determine the 8-bit "row" pixels
                                ADD             x3      , x1, x3, LSL #5                            // obtain offset into LUT

                                LDP             x4      , x5, [x3, #0*16]                           // LUT->VRAM (4 x 64-bit words = 8 x 32-bit pixels)
                                LDP             x6      , x7, [x3, #1*16]

                                AND             x4      , x4, x9                                    // apply foreground/background colours
                                AND             x5      , x5, x9
                                AND             x6      , x6, x9
                                AND             x7      , x7, x9
                                ORR             x4      , x4, x10
                                ORR             x5      , x5, x10
                                ORR             x6      , x6, x10
                                ORR             x7      , x7, x10

                                STP             x4      , x5, [x2, #0*16]
                                STP             x6      , x7, [x2, #1*16]

                                ADD             x2      , x2, #640*4                                // move VRAM down one row
                                MOV             x0      , x0, LSR #8                                // shift ASCII "bit" data

                                SUBS            x8      , x8, #1
                                BNE             _fastcharplot_loop
                                RET


                                .balign     8
_fontdata_8x8_ptr:              .dword      fontdata_8x8
_fontdata_64bit_ptr:            .dword      fontdata_64bit
_cursor_vram_address_ptr:       .dword      cursor_vram_address
_colour_f_ptr:                  .dword      colour_f
_colour_b_ptr:                  .dword      colour_b


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
                                // x2 = number of Kb
                                //
                                // On exit:
                                // x0-x1 = pointer to next locations (both source and target)
                                //
move1k:

                                STP             x3      , x4, [sp, #-1 * 16]
                                STP             x5      , x6, [sp, #-2 * 16]
                                STP             x7      , x8, [sp, #-3 * 16]
                                STP             x9      , x10,[sp, #-4 * 16]

move1k_loop:
                                LDP             x3      , x4, [x0], #16
                                    LDP         x5      , x6, [x0], #16
                                    LDP         x7      , x8, [x0], #16
                                    LDP         x9      , x10,[x0], #16
                                    STP         x3      , x4, [x1], #16
                                    STP         x5      , x6, [x1], #16
                                    STP         x7      , x8, [x1], #16
                                    STP         x9      , x10,[x1], #16

                                LDP             x3      , x4, [x0], #16
                                    LDP         x5      , x6, [x0], #16
                                    LDP         x7      , x8, [x0], #16
                                    LDP         x9      , x10,[x0], #16
                                    STP         x3      , x4, [x1], #16
                                    STP         x5      , x6, [x1], #16
                                    STP         x7      , x8, [x1], #16
                                    STP         x9      , x10,[x1], #16

                                LDP             x3      , x4, [x0], #16
                                    LDP         x5      , x6, [x0], #16
                                    LDP         x7      , x8, [x0], #16
                                    LDP         x9      , x10,[x0], #16
                                    STP         x3      , x4, [x1], #16
                                    STP         x5      , x6, [x1], #16
                                    STP         x7      , x8, [x1], #16
                                    STP         x9      , x10,[x1], #16

                                LDP             x3      , x4, [x0], #16
                                    LDP         x5      , x6, [x0], #16
                                    LDP         x7      , x8, [x0], #16
                                    LDP         x9      , x10,[x0], #16
                                    STP         x3      , x4, [x1], #16
                                    STP         x5      , x6, [x1], #16
                                    STP         x7      , x8, [x1], #16
                                    STP         x9      , x10,[x1], #16

                                LDP             x3      , x4, [x0], #16
                                    LDP         x5      , x6, [x0], #16
                                    LDP         x7      , x8, [x0], #16
                                    LDP         x9      , x10,[x0], #16
                                    STP         x3      , x4, [x1], #16
                                    STP         x5      , x6, [x1], #16
                                    STP         x7      , x8, [x1], #16
                                    STP         x9      , x10,[x1], #16

                                LDP             x3      , x4, [x0], #16
                                    LDP         x5      , x6, [x0], #16
                                    LDP         x7      , x8, [x0], #16
                                    LDP         x9      , x10,[x0], #16
                                    STP         x3      , x4, [x1], #16
                                    STP         x5      , x6, [x1], #16
                                    STP         x7      , x8, [x1], #16
                                    STP         x9      , x10,[x1], #16

                                LDP             x3      , x4, [x0], #16
                                    LDP         x5      , x6, [x0], #16
                                    LDP         x7      , x8, [x0], #16
                                    LDP         x9      , x10,[x0], #16
                                    STP         x3      , x4, [x1], #16
                                    STP         x5      , x6, [x1], #16
                                    STP         x7      , x8, [x1], #16
                                    STP         x9      , x10,[x1], #16

                                LDP             x3      , x4, [x0], #16
                                    LDP         x5      , x6, [x0], #16
                                    LDP         x7      , x8, [x0], #16
                                    LDP         x9      , x10,[x0], #16
                                    STP         x3      , x4, [x1], #16
                                    STP         x5      , x6, [x1], #16
                                    STP         x7      , x8, [x1], #16
                                    STP         x9      , x10,[x1], #16

                                LDP             x3      , x4, [x0], #16
                                    LDP         x5      , x6, [x0], #16
                                    LDP         x7      , x8, [x0], #16
                                    LDP         x9      , x10,[x0], #16
                                    STP         x3      , x4, [x1], #16
                                    STP         x5      , x6, [x1], #16
                                    STP         x7      , x8, [x1], #16
                                    STP         x9      , x10,[x1], #16

                                LDP             x3      , x4, [x0], #16
                                    LDP         x5      , x6, [x0], #16
                                    LDP         x7      , x8, [x0], #16
                                    LDP         x9      , x10,[x0], #16
                                    STP         x3      , x4, [x1], #16
                                    STP         x5      , x6, [x1], #16
                                    STP         x7      , x8, [x1], #16
                                    STP         x9      , x10,[x1], #16

                                LDP             x3      , x4, [x0], #16
                                    LDP         x5      , x6, [x0], #16
                                    LDP         x7      , x8, [x0], #16
                                    LDP         x9      , x10,[x0], #16
                                    STP         x3      , x4, [x1], #16
                                    STP         x5      , x6, [x1], #16
                                    STP         x7      , x8, [x1], #16
                                    STP         x9      , x10,[x1], #16

                                LDP             x3      , x4, [x0], #16
                                    LDP         x5      , x6, [x0], #16
                                    LDP         x7      , x8, [x0], #16
                                    LDP         x9      , x10,[x0], #16
                                    STP         x3      , x4, [x1], #16
                                    STP         x5      , x6, [x1], #16
                                    STP         x7      , x8, [x1], #16
                                    STP         x9      , x10,[x1], #16

                                LDP             x3      , x4, [x0], #16
                                    LDP         x5      , x6, [x0], #16
                                    LDP         x7      , x8, [x0], #16
                                    LDP         x9      , x10,[x0], #16
                                    STP         x3      , x4, [x1], #16
                                    STP         x5      , x6, [x1], #16
                                    STP         x7      , x8, [x1], #16
                                    STP         x9      , x10,[x1], #16

                                LDP             x3      , x4, [x0], #16
                                    LDP         x5      , x6, [x0], #16
                                    LDP         x7      , x8, [x0], #16
                                    LDP         x9      , x10,[x0], #16
                                    STP         x3      , x4, [x1], #16
                                    STP         x5      , x6, [x1], #16
                                    STP         x7      , x8, [x1], #16
                                    STP         x9      , x10,[x1], #16

                                LDP             x3      , x4, [x0], #16
                                    LDP         x5      , x6, [x0], #16
                                    LDP         x7      , x8, [x0], #16
                                    LDP         x9      , x10,[x0], #16
                                    STP         x3      , x4, [x1], #16
                                    STP         x5      , x6, [x1], #16
                                    STP         x7      , x8, [x1], #16
                                    STP         x9      , x10,[x1], #16

                                LDP             x3      , x4, [x0], #16
                                    LDP         x5      , x6, [x0], #16
                                    LDP         x7      , x8, [x0], #16
                                    LDP         x9      , x10,[x0], #16
                                    STP         x3      , x4, [x1], #16
                                    STP         x5      , x6, [x1], #16
                                    STP         x7      , x8, [x1], #16
                                    STP         x9      , x10,[x1], #16

                                    subs x2,x2,#1
                                    bne move1k_loop
                                LDP             x3      , x4, [sp, #-1 * 16]
                                LDP             x5      , x6, [sp, #-2 * 16]
                                LDP             x7      , x8, [sp, #-3 * 16]
                                LDP             x9      , x10,[sp, #-4 * 16]
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
