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



#include "define.h"



#ifndef KERNEL_EMMC_H
#define KERNEL_EMMC_H



	// BCM2835-ARM.pdf (65/205)
	#define 	ARM_EMMC_REGISTERS								( ARM_PERIPHERAL_BASE_ADDRESS + 0x300000 )
	#define 	EMMC_ARG2										( ARM_EMMC_REGISTERS + 0x00 )
	#define 	EMMC_BLKSIZECNT									( ARM_EMMC_REGISTERS + 0x04 )
	#define 	EMMC_ARG1										( ARM_EMMC_REGISTERS + 0x08 )
	#define 	EMMC_CMDTM										( ARM_EMMC_REGISTERS + 0x0c )
	#define 	EMMC_RESP0										( ARM_EMMC_REGISTERS + 0x10 )
	#define 	EMMC_RESP1										( ARM_EMMC_REGISTERS + 0x14 )
	#define 	EMMC_RESP2										( ARM_EMMC_REGISTERS + 0x18 )
	#define 	EMMC_RESP3										( ARM_EMMC_REGISTERS + 0x1c )
	#define 	EMMC_DATA										( ARM_EMMC_REGISTERS + 0x20 )
	#define 	EMMC_STATUS										( ARM_EMMC_REGISTERS + 0x24 )
	#define 	EMMC_CONTROL0									( ARM_EMMC_REGISTERS + 0x28 )
	#define 	EMMC_CONTROL1									( ARM_EMMC_REGISTERS + 0x2c )
	#define 	EMMC_INTERRUPT									( ARM_EMMC_REGISTERS + 0x30 )
	#define 	EMMC_IRPT_MASK									( ARM_EMMC_REGISTERS + 0x34 )
	#define 	EMMC_IRPT_EN									( ARM_EMMC_REGISTERS + 0x38 )
	#define 	EMMC_CONTROL2									( ARM_EMMC_REGISTERS + 0x3c )
	#define 	EMMC_SLOTISR_VER								( ARM_EMMC_REGISTERS + 0xfc )

	#define		EMMC_REGISTER_INTERRUPT_READ_READY				(1<<5)
	#define		EMMC_REGISTER_INTERRUPT_COMMAND_DONE			(1<<0)

	#define		SPI_CMD_GO_IDLE_STATE							0
	#define		SPI_CMD_ALL_SEND_CID							2
	#define		SPI_CMD_SEND_REL_ADDR							3
	#define		SPI_CMD_SWITCH_FUNC								6
	#define		SPI_CMD_CARD_SELECT								7
	#define		SPI_CMD_SEND_IF_COND							8
	#define		SPI_CMD_SEND_CSD								9
	#define		SPI_CMD_SEND_CID								10
	#define		SPI_CMD_STOP_TRANSMISSION						12
	#define		SPI_CMD_SEND_STATUS								13
	#define		SPI_CMD_SET_BLOCKLEN							16
	#define		SPI_CMD_READ_SINGLE_BLOCK						17
	#define		SPI_CMD_READ_MULTIPLE_BLOCK						18
	#define		SPI_CMD_WRITE_BLOCK								24
	#define		SPI_CMD_WRITE_MULTIPLE_BLOCK					25
	#define		SPI_CMD_PROGRAM_CSD								27
	#define		SPI_CMD_SET_WRITE_PROT							28
	#define		SPI_CMD_CLR_WRITE_PROT							29
	#define		SPI_CMD_SEND_WRITE_PROT							30
	#define		SPI_CMD_ERASE_WR_BLK_START_ADDR					32
	#define		SPI_CMD_ERASE_WR_BLK_END_ADDR					33
	#define		SPI_CMD_ERASE									38
	#define		SPI_CMD_LOCK_UNLOCK								42
	#define		SPI_CMD_APP_CMD									55
	#define		SPI_CMD_GEN_CMD									56
	#define		SPI_CMD_READ_OCR								58
	#define		SPI_CMD_CRC_ON_OFF								59

	#define		SPI_ACMD_SD_SEND_OP_COND						41

	#define		SPI_CMD_IS_DATA									(1<<21)
	#define		SPI_CMD_DATA_DIRECTION_CARD_TO_HOST				(1<<4)

	#define		SR_BUFFER_READY								0x00000800
	#define 	SR_DAT_INHIBIT       							0x00000002
	
	// INTERRUPT register settings
	#define 	INT_AUTO_ERROR   								0x01000000
	#define 	INT_DATA_END_ERR 								0x00400000
	#define 	INT_DATA_CRC_ERR 								0x00200000
	#define 	INT_DATA_TIMEOUT 								0x00100000
	#define 	INT_INDEX_ERROR  								0x00080000
	#define 	INT_END_ERROR    								0x00040000
	#define 	INT_CRC_ERROR    								0x00020000
	#define 	INT_CMD_TIMEOUT  								0x00010000
	#define 	INT_ERR          								0x00008000
	#define 	INT_ENDBOOT      								0x00004000
	#define 	INT_BOOTACK      								0x00002000
	#define 	INT_RETUNE       								0x00001000
	#define 	INT_CARD         								0x00000100
	#define 	INT_READ_RDY     								0x00000020
	#define 	INT_WRITE_RDY    								0x00000010
	#define 	INT_BLOCK_GAP    								0x00000004
	#define 	INT_DATA_DONE    								0x00000002
	#define 	INT_CMD_DONE     								0x00000001
	#define 	INT_ERROR_MASK   								(INT_CRC_ERROR|INT_END_ERROR|INT_INDEX_ERROR| \
                          										INT_DATA_TIMEOUT|INT_DATA_CRC_ERR|INT_DATA_END_ERR| \
									 							INT_ERR|INT_AUTO_ERROR)
	#define 	INT_ALL_MASK     								(INT_CMD_DONE|INT_DATA_DONE|INT_READ_RDY|INT_WRITE_RDY|INT_ERROR_MASK)

	#define		EMMC_GPIO_PIN_CD								47
	#define		EMMC_GPIO_PIN_CLK								48
	#define		EMMC_GPIO_PIN_CMD								49
	#define		EMMC_GPIO_PIN_DAT0								50
	#define		EMMC_GPIO_PIN_DAT1								51
	#define		EMMC_GPIO_PIN_DAT2								52
	#define		EMMC_GPIO_PIN_DAT3								53

	#define		EMMC_COMMAND_RESPONSE_TYPE_NO_RESPONSE			0b00
	#define		EMMC_COMMAND_RESPONSE_TYPE_136_BITS				0b01
	#define		EMMC_COMMAND_RESPONSE_TYPE_48_BITS				0b10
	#define		EMMC_COMMAND_RESPONSE_TYPE_48_BITS_BUSY			0b11

	#define 	EMMC_REGISTER_STATUS_READ_AVAILABLE   			0x00000800
	#define 	EMMC_REGISTER_STATUS_READ_AVAILABLE   			0x00000800
	#define 	EMMC_REGISTER_STATUS_DAT_INHIBIT      			0x00000002
	#define 	EMMC_REGISTER_STATUS_CMD_INHIBIT      			0x00000001
	#define 	EMMC_REGISTER_STATUS_APP_CMD          			0x00000020

	#define 	EMMC_REGISTER_CONTROL_0_SPI_MODE_EN      		0x00100000
	#define 	EMMC_REGISTER_CONTROL_0_HCTL_HS_EN       		0x00000004
	#define 	EMMC_REGISTER_CONTROL_0_HCTL_DWITDH      		0x00000002

	#define 	EMMC_REGISTER_CONTROL_1_SRST_DATA        		0x04000000
	#define 	EMMC_REGISTER_CONTROL_1_SRST_CMD         		0x02000000
	#define 	EMMC_REGISTER_CONTROL_1_SRST_HC          		0x01000000
	#define 	EMMC_REGISTER_CONTROL_1_TOUNIT_DIS       		0x000f0000
	#define 	EMMC_REGISTER_CONTROL_1_TOUNIT_MAX       		0x000e0000
	#define 	EMMC_REGISTER_CONTROL_1_CLK_GENSEL       		0x00000020
	#define 	EMMC_REGISTER_CONTROL_1_CLK_EN           		0x00000004
	#define 	EMMC_REGISTER_CONTROL_1_CLK_STABLE       		0x00000002
	#define 	EMMC_REGISTER_CONTROL_1_CLK_INTLEN       		0x00000001

	#define 	EMMC_SEND_OPP_COND_FLAG_HCS           			0x40000000
	#define 	EMMC_SEND_OPP_COND_FLAG_SDXC_POWER    			0x10000000
	#define 	EMMC_SEND_OPP_COND_FLAG_S18R          			0x01000000
	#define 	EMMC_SEND_OPP_COND_FLAG_VOLTAGE       			0x00ff8000
	#define 	EMMC_SEND_OPP_COND_ARG_HC						(EMMC_SEND_OPP_COND_FLAG_HCS|EMMC_SEND_OPP_COND_FLAG_SDXC_POWER|EMMC_SEND_OPP_COND_FLAG_S18R|EMMC_SEND_OPP_COND_FLAG_VOLTAGE)

	#define 	EMMC_SEND_OPP_COND_RESPONSE_COMPLETE    		0x80000000
	#define 	EMMC_SEND_OPP_COND_RESPONSE_CCS         		0x40000000
	#define 	EMMC_SEND_OPP_COND_RESPONSE_S18A        		0x01000000

	#define 	EMMC_SEND_REL_ADDR_RESPONSE_RCA_MASK    		0xffff0000
	#define 	EMMC_SEND_REL_ADDR_RESPONSE_ERR_MASK    		0x0000e000
	#define 	EMMC_SEND_REL_ADDR_RESPONSE_STATE_MASK  		0x00001e00

	#define 	CSD2V2_C_SIZE              						0x3fffff00
	#define 	CSD2V2_C_SIZE_SHIFT        						8

	#define		KHZ												* 1000
	#define		MHZ												* 1000000

	int 			_kernel_emmc_init 						( void );
	void 			_kernel_emmc_read_block					( unsigned int block_number, unsigned char *ptr );



#endif /*KERNEL_EMMC_H*/
