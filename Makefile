#
# Copyright 2019 AbbeyCatUK
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
# http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  

.SILENT:

# common
FIND     			= find
OPTIMIZE 			= 0

# 32-bit toolchain
GCC_PATH_32			= /gcc-arm-8.3-2019.03-x86_64-arm-eabi
GCC_32      			= $(GCC_PATH_32)/bin/arm-eabi-gcc
AS_32       			= $(GCC_PATH_32)/arm-eabi/bin/as
AR_32       			= $(GCC_PATH_32)/arm-eabi/bin/ar
LD_32       			= $(GCC_PATH_32)/arm-eabi/bin/ld
OBJDUMP_32  			= $(GCC_PATH_32)/arm-eabi/bin/objdump
OBJCOPY_32  			= $(GCC_PATH_32)/arm-eabi/bin/objcopy

# 64-bit toolchain
GCC_PATH_64			= /gcc-arm-8.3-2019.03-x86_64-aarch64-elf
GCC_64      			= $(GCC_PATH_64)/bin/aarch64-elf-gcc
AS_64       			= $(GCC_PATH_64)/aarch64-elf/bin/as
AR_64       			= $(GCC_PATH_64)/aarch64-elf/bin/ar
LD_64       			= $(GCC_PATH_64)/aarch64-elf/bin/ld
OBJDUMP_64  			= $(GCC_PATH_64)/aarch64-elf/bin/objdump
OBJCOPY_64  			= $(GCC_PATH_64)/aarch64-elf/bin/objcopy

# 32/64 bit flags for Assembler and Compiler
ARCH_64	 			= -march=armv8-a -mtune=cortex-a53
FLAGS_C_64  			= $(ARCH_64) -std=gnu99 -fsigned-char -Wno-psabi -O$(OPTIMIZE) -fno-builtin -nostartfiles -ffreestanding -D ISA_TYPE=$(ISA_TYPE)
FLAGS_A_64			= $(ARCH_64) -O$(OPTIMIZE) -D ISA_TYPE=$(ISA_TYPE)

ARCH_32	 			= -march=armv8-a -mtune=cortex-a53 -mfpu=neon-fp-armv8 -mfloat-abi=hard
FLAGS_C_32  			= $(ARCH_32) -std=gnu99 -fsigned-char -Wno-psabi -O$(OPTIMIZE) -fno-builtin -nostartfiles -ffreestanding -mapcs-frame -mno-thumb-interwork -D ISA_TYPE=$(ISA_TYPE)
FLAGS_A_32			= $(ARCH_32) -O$(OPTIMIZE) -D ISA_TYPE=$(ISA_TYPE)

# -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  

# 32-bit

32-bit: clean kernel-32

32-bit: ISA_TYPE		= 32

32-bit: GCC			= $(GCC_32)
32-bit: AS			= $(AS_32)
32-bit: AR			= $(AR_32)
32-bit: LD			= $(LD_32)
32-bit: OBJDUMP			= $(OBJDUMP_32)
32-bit: OBJCOPY			= $(OBJCOPY_32)

32-bit: FLAGS_C			= $(FLAGS_C_32)
32-bit: FLAGS_A			= $(FLAGS_A_32)

32-bit: INCLUDE_DIR		= c/32-bit/include

# -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  

# 64-bit

64-bit: clean kernel-64

64-bit: ISA_TYPE		= 64

64-bit: GCC			= $(GCC_64)
64-bit: AS			= $(AS_64)
64-bit: AR			= $(AR_64)
64-bit: LD			= $(LD_64)
64-bit: OBJDUMP			= $(OBJDUMP_64)
64-bit: OBJCOPY			= $(OBJCOPY_64)

64-bit: FLAGS_C			= $(FLAGS_C_64)
64-bit: FLAGS_A			= $(FLAGS_A_64)

64-bit: INCLUDE_DIR		= c/64-bit/include/

# -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  

# clean
clean:
	$(FIND) . -type f -name "*.o"    -delete
	$(FIND) . -type f -name "*.a"    -delete
	$(FIND) . -type f -name "*.hex"  -delete
	$(FIND) . -type f -name "*.bin"  -delete
	$(FIND) . -type f -name "*.elf"  -delete
	$(FIND) . -type f -name "*.list" -delete

# -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  

# kernel
KERNEL_HOME 			= kernel
KERNEL_INCLUDES			= -I $(KERNEL_HOME)/c/common/include -I $(KERNEL_HOME)/$(INCLUDE_DIR)

KERNEL_FILES_C_COMMON		= $(patsubst %.c,%.o,$(shell find kernel/c/common   -type f -name '*.c'))
KERNEL_FILES_C_32		= $(patsubst %.c,%.o,$(shell find kernel/c/32-bit   -type f -name '*.c'))
KERNEL_FILES_C_64		= $(patsubst %.c,%.o,$(shell find kernel/c/64-bit   -type f -name '*.c'))
KERNEL_FLAGS_C	 		= $(FLAGS_C) $(KERNEL_INCLUDES)

KERNEL_FILES_A_32		= $(patsubst %.S,%.o,$(shell find kernel/asm/32-bit -type f -name '*.S'))
KERNEL_FLAGS_A_32		= $(FLAGS_A_32) $(KERNEL_INCLUDES)

KERNEL_FILES_A_64		= $(patsubst %.S,%.o,$(shell find kernel/asm/64-bit -type f -name '*.S'))
KERNEL_FLAGS_A_64		= $(FLAGS_A_64) $(KERNEL_INCLUDES)

kernel-32: uspi $(KERNEL_FILES_A_32) $(KERNEL_FILES_C_COMMON) $(KERNEL_FILES_C_32) kernel-link
kernel-64: uspi $(KERNEL_FILES_A_64) $(KERNEL_FILES_C_COMMON) $(KERNEL_FILES_C_64) kernel-link

kernel-link:
	$(LD)  	linker.ld -o kernel.elf \
			kernel/asm/$(ISA_TYPE)-bit/kernel.o  \
			kernel/asm/$(ISA_TYPE)-bit/swi.o     \
			kernel/asm/$(ISA_TYPE)-bit/stdlib.o  \
			kernel/asm/$(ISA_TYPE)-bit/irq.o     \
			kernel/asm/$(ISA_TYPE)-bit/mmu.o     \
			kernel/asm/$(ISA_TYPE)-bit/process.o \
			$(KERNEL_FILES_C_COMMON) \
			kernel/c/$(ISA_TYPE)-bit/*.o \
			$(USPI_HOME)/lib/libuspi.a $(USPI_HOME)/env/lib/libuspienv.a

	$(OBJCOPY) -I elf32-little -O binary --strip-debug --strip-unneeded --verbose kernel.elf kernel.bin 
	$(OBJCOPY) kernel.elf -O ihex kernel.hex
	$(OBJDUMP) -D kernel.elf > kernel.list

$(KERNEL_HOME)/c/common/%.o : $(KERNEL_HOME)/c/common/%.c
	$(GCC) -c $(KERNEL_FLAGS_C) $< -o $@  

$(KERNEL_HOME)/c/32-bit/%.o : $(KERNEL_HOME)/c/32-bit/%.c
	$(GCC_32) -c $(KERNEL_FLAGS_C) $< -o $@  

$(KERNEL_HOME)/c/64-bit/%.o : $(KERNEL_HOME)/c/64-bit/%.c
	$(GCC_64) -c $(KERNEL_FLAGS_C) $< -o $@  

$(KERNEL_HOME)/asm/32-bit/%.o : $(KERNEL_HOME)/asm/32-bit/%.S	
	$(GCC_32) -c $(KERNEL_FLAGS_A_32) $< -o $@

$(KERNEL_HOME)/asm/64-bit/%.o : $(KERNEL_HOME)/asm/64-bit/%.S	
	$(GCC_64) -c $(KERNEL_FLAGS_A_64) $< -o $@
	
# -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  

# uspi
USPI_HOME 			= uspi

USPI_INCLUDES			= -I $(USPI_HOME)/include -I $(USPI_HOME)/env/include
USPI_FLAGS_C 			= $(FLAGS_C) $(USPI_INCLUDES) -DRASPPI=$(USPI_RASPPI) -DNDEBUG # ACU -DNDEBUG silences assert()
USPI_FLAGS_A 			= $(FLAGS_A) $(USPI_INCLUDES) -DRASPPI=$(USPI_RASPPI) -DNDEBUG # ACU -DNDEBUG silences assert()

USPI_ENV_FILES_C   		= $(patsubst %.c,%.o,$(wildcard $(USPI_HOME)/env/lib/*.c))
USPI_ENV_FILES_A 		= $(patsubst %.S,%.o,$(wildcard $(USPI_HOME)/env/lib/*.S))
USPI_LIB_FILES_C   		= $(patsubst %.c,%.o,$(wildcard $(USPI_HOME)/lib/*.c))

USPI_RASPPI	 		= 3

uspi: uspienv uspilib

uspienv: $(USPI_ENV_FILES_C) $(USPI_ENV_FILES_A)
	$(AR) cr ${USPI_HOME}/env/lib/libuspienv.a $(USPI_ENV_FILES_C) $(USPI_ENV_FILES_A)

uspilib: $(USPI_LIB_FILES_C) $(USPI_LIB_FILES_A)
	$(AR) cr ${USPI_HOME}/lib/libuspi.a $(USPI_LIB_FILES_C) $(USPI_LIB_FILES_A)

$(USPI_HOME)/%.o : $(USPI_HOME)/%.c
	$(GCC) -c $(USPI_FLAGS_C) $< -o $@  

$(USPI_HOME)/%.o : $(USPI_HOME)/%.S	
	$(GCC) -c $(USPI_FLAGS_A) $< -o $@
