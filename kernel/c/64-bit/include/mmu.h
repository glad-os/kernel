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



	// intermediate physical address size
	#define			MMU_TCR_IPS_32BITS					0b000
	#define			MMU_TCR_IPS_36BITS					0b001
	#define			MMU_TCR_IPS_40BITS					0b010
	#define			MMU_TCR_IPS_42BITS					0b011
	#define			MMU_TCR_IPS_44BITS					0b100
	#define			MMU_TCR_IPS_48BITS					0b101

	// granule size
	#define			MMU_TCR_TG0_4KB						0b00
	#define			MMU_TCR_TG0_64KB					0b01
	#define			MMU_TCR_TG0_16KB					0b10

	// translation table memory shareability
	#define			MMU_TCR_SH0_NON_SHAREABLE				0b00
	#define			MMU_TCR_SH0_OUTER_SHAREABLE				0b10
	#define			MMU_TCR_SH0_INNER_SHAREABLE				0b11

	// enable TTBR0 translation walks
	#define			MMU_TCR_EPD0_ENABLE					0
	#define			MMU_TCR_EPD0_DISABLE					1

	// outer cacheability for memory associated with translation table walks using TTBR0_EL1
	#define			MMU_TCR_ORGN0_NON_CACHEABLE				0b00
	#define			MMU_TCR_ORGN0_WRITE_BACK_WRITE_ALLOCATE			0b01
	#define			MMU_TCR_ORGN0_WRITE_THROUGH				0b10
	#define			MMU_TCR_ORGN0_WRITE_BACK_NO_WRITE_ALLOCATE		0b11

	// inner cacheability for memory associated with translation table walks using TTBR0_EL1
	#define			MMU_TCR_IRGN0_NON_CACHEABLE				0b00
	#define			MMU_TCR_IRGN0_WRITE_BACK_WRITE_ALLOCATE			0b01
	#define			MMU_TCR_IRGN0_WRITE_THROUGH				0b10
	#define			MMU_TCR_IRGN0_WRITE_BACK_NO_WRITE_ALLOCATE		0b11

	// MAIR definitions
	#define			MMU_MAIR_DEVICE_NGNRNE					0b00000000 // 0x00
	#define			MMU_MAIR_DEVICE_NGNRE					0b00000100 // 0x04
	#define			MMU_MAIR_NORMAL_OUTER_INNER_NOCACHE			0b01000100 // 0x44
	#define			MMU_MAIR_NORMAL_OUTER_INNER_WRITEBACK_RW_ALLOC		0b11111111 // 0xff

	// translation table definitions
	#define			MMU_TRANS_ENTRY_LEVEL_012_INVALID			0b00
	#define			MMU_TRANS_ENTRY_LEVEL_012_BLOCK				0b01
	#define			MMU_TRANS_ENTRY_LEVEL_012_TABLE				0b11
	#define			MMU_TRANS_ENTRY_LEVEL_3_PAGE				0b11

	// page descriptor attributes (bits 11:2)
	#define			MMU_PAGE_DESCRIPTOR_GLOBAL				(1 << 11)
	#define			MMU_PAGE_DESCRIPTOR_ACCESS_FLAG				(1 << 10)
	#define			MMU_PAGE_DESCRIPTOR_NON_SHAREABLE			(0b00 << 8)
	#define			MMU_PAGE_DESCRIPTOR_OUTER_SHAREABLE			(0b10 << 8)
	#define			MMU_PAGE_DESCRIPTOR_INNER_SHAREABLE			(0b11 << 8)

	#define			MMU_PAGE_DESCRIPTOR_ACCESS_EL1_RW_EL0_NONE		(0b00 << 6)
	#define			MMU_PAGE_DESCRIPTOR_ACCESS_EL1_RW_EL0_RW		(0b01 << 6)
	#define			MMU_PAGE_DESCRIPTOR_ACCESS_EL1_RO_EL0_NONE		(0b10 << 6)
	#define			MMU_PAGE_DESCRIPTOR_ACCESS_EL1_RO_EL0_RO		(0b11 << 6)

	#define			MMU_PAGE_DESCRIPTOR_NON_SECURE				(1 << 5)

	#define			MMU_PAGE_DESCRIPTOR_MAIR_0				(0b000 << 2)
	#define			MMU_PAGE_DESCRIPTOR_MAIR_1				(0b001 << 2)
	#define			MMU_PAGE_DESCRIPTOR_MAIR_2				(0b010 << 2)
	#define			MMU_PAGE_DESCRIPTOR_MAIR_3				(0b011 << 2)
	#define			MMU_PAGE_DESCRIPTOR_MAIR_4				(0b100 << 2)
	#define			MMU_PAGE_DESCRIPTOR_MAIR_5				(0b101 << 2)
	#define			MMU_PAGE_DESCRIPTOR_MAIR_6				(0b110 << 2)
	#define			MMU_PAGE_DESCRIPTOR_MAIR_7				(0b111 << 2)



	void 			_kernel_mmu_init( void );
	void 			_kernel_mmu_enable( void );
	void 			_kernel_mmu_disable( void );
	unsigned int		_kernel_mmu_enabled( void );
	void 			_kernel_mmu_map_process_in( unsigned int n, unsigned int cache, unsigned int buffer );



#endif /*MMU_H*/
