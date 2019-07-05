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



#ifndef MMU_H
#define MMU_H



	// CP15 MMU register structure (Furber, p299)
	#define     ARM_CP15_REGISTER_ID                            "c0"
	#define     ARM_CP15_REGISTER_CONTROL                       "c1"
	#define     ARM_CP15_REGISTER_TRANSLATION_TABLE_BASE        "c2"
	#define     ARM_CP15_REGISTER_DOMAIN_ACCESS_CONTROL         "c3"
	#define     ARM_CP15_REGISTER_FAULT_STATUS                  "c5"
	#define     ARM_CP15_REGISTER_FAULT_ADDRESS                 "c6"
	#define     ARM_CP15_REGISTER_CACHE                         "c7"
	#define     ARM_CP15_REGISTER_TLB                           "c8"
	#define     ARM_CP15_REGISTER_READ_BUFFER                   "c9"
	#define     ARM_CP15_REGISTER_TLB_LOCKDOWN                  "c10"
	#define     ARM_CP15_REGISTER_PROCESS_ID_MAPPING            "c13"
	#define     ARM_CP15_REGISTER_DEBUG                         "c14"
	#define     ARM_CP15_REGISTER_TEST_AND_CLOCK                "c15"

	// (ARM Cortex TRM #4-130/#4-184) SCTLR Op1=0, CRn=c1, CRm=c0, Op2=0
	#define     ARM_CP15_SCTLR_MMU_ENABLE                       (1 <<  0)
	#define     ARM_CP15_SCTLR_CACHE_ENABLE                     (1 <<  2)
	#define     ARM_CP15_SCTLR_INSTRUCTION_CACHE_ENABLE         (1 << 12)

	#define     ARM_MMU_DOMAIN_ACCESS_CONTROL_CLIENT            (0b01)
	#define     ARM_MMU_DOMAIN_ACCESS_CONTROL_MANAGER           (0b11)

	// ACU @TODO - for AArch32, bit 5 of TTBCR (32-bit reg) is used to disable TTBR1 walks
	// whereas in AArch64, bit 23 of TCR_ELx is used for this (TCR_EL1 for us), so this flag actually differs across architectures - need to model this appropriately!!
	#define     ARM_MMU_TTB_DISABLE_TTBR1_WALKS                 (1 << 5)

	// region bits. Indicates the Cacheability attributes for the memory associated with the translation table walks:
	// 00 Normal memory, Non-cacheable.
	// 01 Normal memory, Write-Back Write-Allocate Cacheable.
	// 10 Normal memory, Write-Through Cacheable.
	// 11 Normal memory, Write-Back no Write-Allocate Cacheable.
	#define     ARM_MMU_INNER_NON_CACHEABLE                       		( (0b0 << 0) | (0b0 << 6) )
	#define     ARM_MMU_INNER_CACHEABLE_WRITE_BACK_WRITE_ALLOCATE 		( (0b0 << 0) | (0b1 << 6) )
	#define     ARM_MMU_INNER_CACHEABLE_WRITE_THROUGH             		( (0b1 << 0) | (0b0 << 6) )
	#define     ARM_MMU_INNER_CACHEABLE_WRITE_BACK_NO_WRITE_ALLOCATE	( (0b1 << 0) | (0b1 << 6) )
	#define     ARM_MMU_OUTER_NON_CACHEABLE                       		( (0b0 << 4) | (0b0 << 3) )
	#define     ARM_MMU_OUTER_CACHEABLE_WRITE_BACK_WRITE_ALLOCATE 		( (0b0 << 4) | (0b1 << 3) )
	#define     ARM_MMU_OUTER_CACHEABLE_WRITE_THROUGH             		( (0b1 << 4) | (0b0 << 3) )
	#define     ARM_MMU_OUTER_CACHEABLE_WRITE_BACK_NO_WRITE_ALLOCATE	( (0b1 << 4) | (0b1 << 3) )

	#define     ARM_MMU_TTB_ENTRY_TYPE_SECTION                  2
	#define     ARM_MMU_TTB_SHORT_ACCESS_PERMISSIONS_NO_ACCESS  (0b00 << 10)
	#define     ARM_MMU_TTB_SHORT_ACCESS_ONLY_P1                (0b01 << 10)
	#define     ARM_MMU_TTB_SHORT_ACCESS_PL0_FAULTS             (0b10 << 10)
	#define     ARM_MMU_TTB_SHORT_ACCESS_FULL_ACCESS            (0b11 << 10)



	void 			_kernel_mmu_init( void );
	void 			_kernel_mmu_enable( void );
	void 			_kernel_mmu_disable( void );
	unsigned int		_kernel_mmu_enabled( void );
	void 			_kernel_mmu_map_va_section( unsigned int va_section, unsigned int pa_section, int cache, int buffer );
	void 			_kernel_mmu_map_process_in( unsigned int n, unsigned int cache, unsigned int buffer );

	extern void		_kernel_mmu_configure_ttbcr( uint32_t ttbcr );
	extern void		_kernel_mmu_configure_ttbr0( uint32_t ttbr0 );
	extern void		_kernel_mmu_configure_dacr( uint32_t dacr );
	extern void		_kernel_mmu_configure_enable( uint32_t flags );
	extern void		_kernel_mmu_configure_disable( uint32_t flags );
	extern void		_kernel_mmu_clear_tlb_and_btc( void );





#endif /*MMU_H*/
