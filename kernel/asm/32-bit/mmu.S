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


.global                         _kernel_mmu_invalidate_tlb
.global                         _kernel_mmu_invalidate_btc

.global                         _kernel_mmu_configure_ttbcr
.global                         _kernel_mmu_configure_ttbr0
.global                         _kernel_mmu_configure_dacr
.global                         _kernel_mmu_configure_enable
.global                         _kernel_mmu_configure_disable
.global                         _kernel_mmu_clear_tlb_and_btc

.global                         _kernel_mmu_invalidate_i_cache
.global                         _kernel_mmu_clean_l1_d_cache
.global                         _kernel_mmu_clean_l2_d_cache
.global                         _kernel_mmu_clean_and_invalidate_cache_va


                                //
                                // _kernel_mmu_clear_tlb_and_btc
                                //
                                // Clears both the TLB and BTC
                                //
                                // On entry:
                                // -
                                //
                                // On exit:
                                // -
                                //
_kernel_mmu_clear_tlb_and_btc:
                                STMFD           sp!     , {lr}
                                DSB             SY
                                BL              _kernel_mmu_invalidate_tlb
                                BL              _kernel_mmu_invalidate_btc
                                DSB             SY
                                ISB
                                LDMFD           sp!     , {pc}


                                //
                                // _kernel_mmu_configure_ttbcr
                                //
                                // Configures the TTBCR (Translation Table Base Control Register)
                                //
                                // On entry:
                                // r0 - contains the TTBCR value FLAGS TO SET
                                //
                                // On exit:
                                // -
                                //
_kernel_mmu_configure_ttbcr:
                                MRC             p15     , 0, r1, c2, c0, 2
                                ORR             r1      , r1, r0
                                MCR             p15     , 0, r1, c2, c0, 2
                                MOV             pc      , lr


                                //
                                // _kernel_mmu_configure_ttbr0
                                //
                                // Configures the TTBR0 (Translation Table Base Register 0)
                                //
                                // On entry:
                                // r0 - contains the TTBR0 value
                                //
                                // On exit:
                                // -
                                //
_kernel_mmu_configure_ttbr0:
                                MCR             p15     , 0, r0, c2, c0, 0
                                MOV             pc      , lr


                                //
                                // _kernel_mmu_configure_dacr
                                //
                                // Configures the DACR (Domain Access Control Register)
                                //
                                // On entry:
                                // r0 - contains the DACR value
                                //
                                // On exit:
                                // -
                                //
_kernel_mmu_configure_dacr:
                                MCR             p15     , 0, r0, c3, c0, 0
                                MOV             pc      , lr


                                //
                                // _kernel_mmu_configure_enable
                                //
                                // Configures the MMU, enabling required flags
                                //
                                // On entry:
                                // r0 - contains the flags to enable
                                //
                                // On exit:
                                // -
                                //
_kernel_mmu_configure_enable:
                                MRC             p15     , 0, r1, c1, c0, 0
                                ORR             r1      , r1, r0
                                MCR             p15     , 0, r1, c1, c0, 0
                                MOV             pc      , lr


                                //
                                // _kernel_mmu_configure_disable
                                //
                                // Configures the MMU, disabling required flags
                                //
                                // On entry:
                                // r0 - contains the flags to disable
                                //
                                // On exit:
                                // -
                                //
_kernel_mmu_configure_disable:
                                MRC             p15     , 0, r1, c1, c0, 0
                                BIC             r1      , r1, r0
                                MCR             p15     , 0, r1, c1, c0, 0
                                MOV             pc      , lr


                                //
                                // _kernel_mmu_invalidate_tlb
                                //
                                // Invalidates ("flushes") the TLB (Translation Lookaside Buffer)
                                //
                                // On entry:
                                // -
                                //
                                // On exit:
                                // r0 = corrupted
                                //
_kernel_mmu_invalidate_tlb:
                                MOV             r0      , #0
                                MCR             p15     , #0, r0, c8, c7, #0
                                MOV             pc      , lr


                                //
                                // _kernel_mmu_invalidate_btc
                                //
                                // Invalidates ("flushes") the BTC (Branch Target Cache)
                                //
                                // On entry:
                                // -
                                //
                                // On exit:
                                // r0 = corrupted
                                //
_kernel_mmu_invalidate_btc:
                                // invalidate btc? (branch target cache)
                                MOV             r0      , #0
                                MCR             p15     , #0, r0, c7, c5, #6
                                MOV             pc      , lr


                                //
                                // _kernel_mmu_invalidate_i_cache
                                //
                                // Invalidates ("flushes") the "I" cache (Instruction cache)
                                //
                                // On entry:
                                // -
                                //
                                // On exit:
                                // r0 = corrupted
                                //
                                // References:
                                // [1] ARM DDI 0500G (4-133)
                                //
_kernel_mmu_invalidate_i_cache:
                                // TODO what should R0 be on entry here - I haven't set it?!
                                MCR             p15     , #0, r0, c7, c5, #0                        // [1] ICIALLU "Invalidate all Instruction Caches to PoU"
                                DSB
                                ISB
                                MOV             pc      , lr


                                //
                                // _kernel_mmu_clean_l1_d_cache
                                //
                                // Cleans the Level 1 (L1) Data cache
                                //
                                // ASSOCIATIVITY = 4-way set associative
                                // LINELEN       = 64 bytes
                                // NSETS         = 64 sets (16Kb / 4*64bytes = 64 sets)
                                // so therefore...
                                // A = LOG2(ASSOCIATIVITY) = 2
                                // L = LOG2(LINELEN) = 6
                                // S = LOG2(NSETS) = 6
                                // B = (L+S) = 12
                                //
                                // On entry:
                                // -
                                //
                                // On exit:
                                // r0-r2 = corrupted
                                //
                                // References:
                                // [1] ARM DDI 0487A.f (G6-4350)
                                //
_kernel_mmu_clean_l1_d_cache:
                                MOV             r0      , #0                                        // r0 = Way
l1_way_loop:
                                MOV             r1      , #0                                        // r1 = Set
l1_set_loop:
                                MOV             r2      , #0
                                ADD             r2      , r2, r0, LSL #30
                                ADD             r2      , r2, r1, LSL # 6
                                ADD             r2      , r2, #(0<<1)                               // indicates L1 cache
                                MCR             p15     , 0, r2, c7, c14 , 1                        // DCCISW operation

                                ADD             r1      , r1, #1
                                CMP             r1      , #64
                                BNE             l1_set_loop
                                ADD             r0      , r0, #1
                                CMP             r0      , #4
                                BNE             l1_way_loop

                                DSB
                                ISB
                                MOV             pc      , lr


                                //
                                // _kernel_mmu_clean_l2_d_cache
                                //
                                // Cleans the Level 2 (L2) Data cache
                                // ASSOCIATIVITY = 16-way set associative
                                // LINELEN       = 64 bytes
                                // NSETS         = 1024 sets (512Kb / 4*64bytes = 1024 sets)
                                // so therefore...
                                // A = LOG2(ASSOCIATIVITY) = 4
                                // L = LOG2(LINELEN) = 6
                                // S = LOG2(NSETS) = 10
                                // B = (L+S) = 16
                                //
                                // On entry:
                                // -
                                //
                                // On exit:
                                // r0-r2 = corrupted
                                //
                                // References:
                                // [1] ARM DDI 0487A.f (G6-4350)
                                //
_kernel_mmu_clean_l2_d_cache:
                                MOV             r0      , #0                                        // r0 = Way
l2_way_loop:
                                MOV             r1      , #0                                        // r1 = Set
l2_set_loop:
                                MOV             r2      , #0
                                ADD             r2      , r2, r0, LSL #28
                                ADD             r2      , r2, r1, LSL # 6
                                ADD             r2      , r2, #(1<<1)                               // indicates L2 cache
                                MCR             p15     , 0, r2, c7, c14, 1                         // DCCISW operation

                                ADD             r1      , r1, #1
                                CMP             r1      , #1024
                                BNE             l2_set_loop
                                ADD             r0      , r0, #1
                                CMP             r0      , #16
                                BNE             l2_way_loop

                                DSB
                                ISB
                                MOV             pc      , lr


                                //
                                // On entry:
                                // r0 = virtual address
                                //
_kernel_mmu_clean_and_invalidate_cache_va:
                                MCR             p15     , 0, r0, c7, c14, 1                         // DCCISW operation

                                DSB
                                ISB
                                MOV             pc      , lr
