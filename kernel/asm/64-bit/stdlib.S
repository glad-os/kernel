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

_fastcharplot_row_0:            MOV             x3      , x0, LSR #(7*8)                            // determine the 8-bit "row" pixels
                                AND             x3      , x3, #0xff
                                ADD             x3      , x1, x3, LSL #5                            // obtain offset into LUT
                                LDP             x4      , x5, [x3, #0*16]                           // LUT->VRAM (4 x 64-bit words = 8 x 32-bit pixels)
                                LDP             x6      , x7, [x3, #1*16]
                                STP             x4      , x5, [x2, #0*16]
                                STP             x6      , x7, [x2, #1*16]
                                ADD             x2      , x2, #640*4                                // move VRAM down one row

_fastcharplot_row_1:            MOV             x3      , x0, LSR #(6*8)                            // determine the 8-bit "row" pixels
                                AND             x3      , x3, #0xff
                                ADD             x3      , x1, x3, LSL #5                            // obtain offset into LUT
                                LDP             x4      , x5, [x3, #0*16]                           // LUT->VRAM (4 x 64-bit words = 8 x 32-bit pixels)
                                LDP             x6      , x7, [x3, #1*16]
                                STP             x4      , x5, [x2, #0*16]
                                STP             x6      , x7, [x2, #1*16]
                                ADD             x2      , x2, #640*4                                // move VRAM down one row

_fastcharplot_row_2:            MOV             x3      , x0, LSR #(5*8)                            // determine the 8-bit "row" pixels
                                AND             x3      , x3, #0xff
                                ADD             x3      , x1, x3, LSL #5                            // obtain offset into LUT
                                LDP             x4      , x5, [x3, #0*16]                           // LUT->VRAM (4 x 64-bit words = 8 x 32-bit pixels)
                                LDP             x6      , x7, [x3, #1*16]
                                STP             x4      , x5, [x2, #0*16]
                                STP             x6      , x7, [x2, #1*16]
                                ADD             x2      , x2, #640*4                                // move VRAM down one row

_fastcharplot_row_3:            MOV             x3      , x0, LSR #(4*8)                            // determine the 8-bit "row" pixels
                                AND             x3      , x3, #0xff
                                ADD             x3      , x1, x3, LSL #5                            // obtain offset into LUT
                                LDP             x4      , x5, [x3, #0*16]                           // LUT->VRAM (4 x 64-bit words = 8 x 32-bit pixels)
                                LDP             x6      , x7, [x3, #1*16]
                                STP             x4      , x5, [x2, #0*16]
                                STP             x6      , x7, [x2, #1*16]
                                ADD             x2      , x2, #640*4                                // move VRAM down one row

_fastcharplot_row_4:            MOV             x3      , x0, LSR #(3*8)                            // determine the 8-bit "row" pixels
                                AND             x3      , x3, #0xff
                                ADD             x3      , x1, x3, LSL #5                            // obtain offset into LUT
                                LDP             x4      , x5, [x3, #0*16]                           // LUT->VRAM (4 x 64-bit words = 8 x 32-bit pixels)
                                LDP             x6      , x7, [x3, #1*16]
                                STP             x4      , x5, [x2, #0*16]
                                STP             x6      , x7, [x2, #1*16]
                                ADD             x2      , x2, #640*4                                // move VRAM down one row

_fastcharplot_row_5:            MOV             x3      , x0, LSR #(2*8)                            // determine the 8-bit "row" pixels
                                AND             x3      , x3, #0xff
                                ADD             x3      , x1, x3, LSL #5                            // obtain offset into LUT
                                LDP             x4      , x5, [x3, #0*16]                           // LUT->VRAM (4 x 64-bit words = 8 x 32-bit pixels)
                                LDP             x6      , x7, [x3, #1*16]
                                STP             x4      , x5, [x2, #0*16]
                                STP             x6      , x7, [x2, #1*16]
                                ADD             x2      , x2, #640*4                                // move VRAM down one row

_fastcharplot_row_6:            MOV             x3      , x0, LSR #(1*8)                            // determine the 8-bit "row" pixels
                                AND             x3      , x3, #0xff
                                ADD             x3      , x1, x3, LSL #5                            // obtain offset into LUT
                                LDP             x4      , x5, [x3, #0*16]                           // LUT->VRAM (4 x 64-bit words = 8 x 32-bit pixels)
                                LDP             x6      , x7, [x3, #1*16]
                                STP             x4      , x5, [x2, #0*16]
                                STP             x6      , x7, [x2, #1*16]
                                ADD             x2      , x2, #640*4                                // move VRAM down one row

_fastcharplot_row_7:            MOV             x3      , x0, LSR #(0*8)                            // determine the 8-bit "row" pixels
                                AND             x3      , x3, #0xff
                                ADD             x3      , x1, x3, LSL #5                            // obtain offset into LUT
                                LDP             x4      , x5, [x3, #0*16]                           // LUT->VRAM (4 x 64-bit words = 8 x 32-bit pixels)
                                LDP             x6      , x7, [x3, #1*16]
                                STP             x4      , x5, [x2, #0*16]
                                STP             x6      , x7, [x2, #1*16]
                                ADD             x2      , x2, #640*4                                // move VRAM down one row

                                RET


                                .balign     8
_fontdata_8x8_ptr:              .dword      fontdata_8x8
_fontdata_64bit_ptr:            .dword      fontdata_64bit
_cursor_vram_address_ptr:       .dword      cursor_vram_address


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
