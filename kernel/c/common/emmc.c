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
#include "define.h"
#include "emmc.h"
#include "gpio.h"
#include "start.h"
#include "video.h"
#include "systimer.h"
#include "stdlib.h"
#include "kernel.h"
#include "mailbox.h"



unsigned int ocr, rca, csd[4], capacity;
unsigned char manufacturer[6], sector[512];
unsigned int tran_speed_exp[] = { 100000 / 10,	100000,	1000000, 10000000, 0, 0, 0, 0 };
unsigned char tran_speed_man[] = { 0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80 };
unsigned int emmc_clock_rate = 0;

void _kernel_emmc_extract_manufacturer( void );
void _kernel_emmc_command( unsigned int command, unsigned int parameter, unsigned int response_type );
int _kernel_emmc_configure_sd_clock( unsigned int f );
void _kernel_emmc_report_card_details( void );
void _kernel_emmc_reset_host( void );
void _kernel_emmc_init_card( void );

volatile uint32_t 	mailbox_buffer[ 256 ] __attribute__( ( aligned(16) ) );


/**
 *
 * _kernel_emmc_init
 *
 * Initialisation process for EMMC subsystem.
 *
 */
int _kernel_emmc_init( void )
{

	// --- check the mailbox and keep a note of how fast the EMMC clock is going ---
	unsigned int index;

	// check what the clock rate is set at
	// 0 => request
	index = 1;
	mailbox_buffer[ index++ ] = 0;

	/* format: tag, value buffer size, 0 ('process request'), <<params/value buffer>>) */
	mailbox_buffer[ index++ ] = ARM_MAILBOX_TAG_CLOCK_RATE;
	mailbox_buffer[ index++ ] = 8;
	mailbox_buffer[ index++ ] = 0;			// this gets changes to 0x80000004 -> top bit = b31 set: response, and b30-b0: value length in bytes

	mailbox_buffer[ index++ ] = 1;			// clock id
	mailbox_buffer[ index++ ] = 0;			// this is where result should be found (index #6)

	mailbox_buffer[ index++ ] = ARM_MAILBOX_TAG_TERMINATOR;
	mailbox_buffer[ 0 ] = index * 4;		// Buffer size
	_kernel_mailbox_write( 8, (uintptr_t) mailbox_buffer );
    	_kernel_mailbox_read( 8 );
	emmc_clock_rate = mailbox_buffer[ 6 ] / 6;
	//_kernel_video_print_string( "EMMC Clock Rate : " ); _kernel_video_print_decimal( emmc_clock_rate / (1 MHZ) ); _kernel_video_print_string( " MHz\n" );
	// -- end of mailbox check

	unsigned int gpio_pin;

	_kernel_video_print_string( "SD CARD: " );

	// configure GPIO pins so they connect to the EMMC interface
	put_word( ARM_GPIO_REGISTERS + 0xd0, 3 ); // GPPinMuxSD

	_kernel_emmc_reset_host();
	_kernel_emmc_init_card();
	_kernel_emmc_report_card_details();

	return 0;

}


/**
 *
 * _kernel_emmc_reset_host
 *
 * Rests the EMMC host.
 *
 */
void _kernel_emmc_reset_host( void ) {

	unsigned int i;

	// issue a reset to the host controller
	put_word( EMMC_CONTROL1, get_word( EMMC_CONTROL1 ) | 1 ); // IMPORTANT: use 4 DAT lines (err, wrong - CLKINTLEN, but if I don't do this, Pi4 not always works! N/Y/N/N)
	put_word( EMMC_CONTROL1, get_word( EMMC_CONTROL1 ) | EMMC_REGISTER_CONTROL_1_SRST_HC );
	i = 10000;
	do {
		_kernel_systimer_wait_msec( 10 );
	}
	while ( ( get_word( EMMC_CONTROL1 ) & EMMC_REGISTER_CONTROL_1_SRST_HC ) && i-- );
	if ( !i ) {
		_kernel_panic( "EMMC", "Reset failed" );
	}

	// enable the clock ('internal'), and set max timeout
	put_word( EMMC_CONTROL1, get_word( EMMC_CONTROL1 ) | EMMC_REGISTER_CONTROL_1_CLK_INTLEN | EMMC_REGISTER_CONTROL_1_TOUNIT_MAX );
	_kernel_systimer_wait_msec( 10 );

	// during initialisation, set clock to a standard 400KHz
	_kernel_emmc_configure_sd_clock( 400 KHZ );

	// enable all interrupts
	put_word( EMMC_IRPT_EN,   0xffffffff );
	put_word( EMMC_IRPT_MASK, 0xffffffff );

	// select idle state
	_kernel_emmc_command( SPI_CMD_GO_IDLE_STATE, 0, EMMC_COMMAND_RESPONSE_TYPE_NO_RESPONSE );

}



/**
 *
 * _kernel_emmc_init_card
 *
 * Initialises the SD card.
 *
 */
void _kernel_emmc_init_card( void ) {

	unsigned int i, e, m;

	// (interface condition - relates to voltage information etc.)
	// @todo - remove magic number 0x000001AA
	_kernel_emmc_command( SPI_CMD_SEND_IF_COND, 0x000001AA, EMMC_COMMAND_RESPONSE_TYPE_48_BITS );

	// (operation condition - periodically resend until initialisation completes)
	_kernel_emmc_command( SPI_CMD_APP_CMD, 0, EMMC_COMMAND_RESPONSE_TYPE_NO_RESPONSE );
	_kernel_emmc_command( SPI_ACMD_SD_SEND_OP_COND, EMMC_SEND_OPP_COND_ARG_HC, EMMC_COMMAND_RESPONSE_TYPE_48_BITS );
	i = 10;
	while( !( ocr & EMMC_SEND_OPP_COND_RESPONSE_COMPLETE ) && i-- ) {
		_kernel_systimer_wait_msec( 500 );
	    	_kernel_emmc_command( SPI_CMD_APP_CMD, 0, EMMC_COMMAND_RESPONSE_TYPE_NO_RESPONSE );
		_kernel_emmc_command( SPI_ACMD_SD_SEND_OP_COND, EMMC_SEND_OPP_COND_ARG_HC, EMMC_COMMAND_RESPONSE_TYPE_48_BITS );
	}
	if ( !i ) {
		_kernel_panic( "EMMC", "Card initialisation failed" );
	}

	// initialisation process requires the following commands to take place
	_kernel_emmc_command( SPI_CMD_ALL_SEND_CID, 0, EMMC_COMMAND_RESPONSE_TYPE_136_BITS );
	_kernel_emmc_command( SPI_CMD_SEND_REL_ADDR, 0, EMMC_COMMAND_RESPONSE_TYPE_48_BITS );
	_kernel_emmc_command( SPI_CMD_SEND_CID, rca, EMMC_COMMAND_RESPONSE_TYPE_136_BITS );
	_kernel_emmc_command( SPI_CMD_SEND_CSD, rca, EMMC_COMMAND_RESPONSE_TYPE_136_BITS );

	// configure clock speed: maximum speed declared in the CSD (via exponent/mantissa values)
	e = tran_speed_exp[ ( csd[ 0 ] & 0b00000111 )      ];
	m = tran_speed_man[ ( csd[ 0 ] & 0b11111000 ) >> 3 ];
	// _kernel_video_print_string( "CARD SPEED : " ); _kernel_video_print_decimal( (e*m) / (1 MHZ) ); _kernel_video_print_string( " MHz\n" );
	_kernel_emmc_configure_sd_clock( e * m );

	_kernel_emmc_command( SPI_CMD_CARD_SELECT, rca, EMMC_COMMAND_RESPONSE_TYPE_48_BITS_BUSY );

}



/**
 *
 * _kernel_emmc_report_card_details
 *
 * Checks and displays on-screen basic card details (size and filesystem format).
 *
 */
void _kernel_emmc_report_card_details( void ) {

	unsigned long csize;
	char decimal[20];

	// @todo - CSD V2 assumed, needs support for other variants
	// @todo - code to extract specific bit patterns would make this code clearer (csize held in bits 48..69!)
	csize = ( ( csd[ 1 ] & 0x0000003f ) << 16 ) + ( csd[ 2 ] >> 16 );
	capacity = ( csize + 1 ) / 2;

	// report card size
	_kernel_sprintf_i( decimal, capacity );
	_kernel_video_print_string( (char *)&decimal ); _kernel_video_print_string( "MB " );
	_kernel_video_print_string( "(" ); _kernel_video_print_string( (char *)&manufacturer ); _kernel_video_print_string( ") " );

	// report if card is FAT32
	_kernel_emmc_read_block( 0, (unsigned char *)&sector );
	( sector[510] == 0x55 && sector[511] == 0xaa ) ?
		_kernel_video_print_string( "[FAT32]\n\n" ):
		_kernel_video_print_string( "[UNRECOGNISED]\n\n" );

}



/**
 *
 * _kernel_emmc_extract_manufacturer
 *
 * Extracts the manufacturer string from the card.
 *
 */
void _kernel_emmc_extract_manufacturer( void ) {

	unsigned char *c;

	// manufacturer held in little endian format
	c = (unsigned char *)EMMC_RESP0;
	manufacturer[ 0 ] = *( c + ( 2 * 4 ) + 3 );
	manufacturer[ 1 ] = *( c + ( 2 * 4 ) + 2 );
	manufacturer[ 2 ] = *( c + ( 2 * 4 ) + 1 );
	manufacturer[ 3 ] = *( c + ( 2 * 4 ) + 0 );
	manufacturer[ 4 ] = *( c + ( 1 * 4 ) + 3 );
	manufacturer[ 5 ] = 0x00;

}



/**
 *
 * _kernel_emmc_command
 *
 * Issues a command to the EMMC controller.
 *
 */
void _kernel_emmc_command( unsigned int command, unsigned int parameter, unsigned int response_type ) {

	unsigned int total, i;

	// ensure command is clear to be issued beforehand
	i = 1000;
	while ( ( get_word( EMMC_STATUS ) & ( EMMC_REGISTER_STATUS_CMD_INHIBIT | EMMC_REGISTER_STATUS_DAT_INHIBIT ) ) && i-- )
        	_kernel_systimer_wait_msec( 1 );
	if ( !i ) {
		_kernel_panic( "EMMC", "Command timed out - inhibited" );
	}

	// clear the "command complete" interrupt
	put_word( EMMC_INTERRUPT, 1 /*get_word( EMMC_INTERRUPT )*/ );

	// send the command
	put_word( EMMC_ARG1, parameter );
	total = ( command << 24 ) | ( response_type << 16 );
	if ( command == SPI_CMD_READ_SINGLE_BLOCK ) {
		total = total | SPI_CMD_IS_DATA | SPI_CMD_DATA_DIRECTION_CARD_TO_HOST;
	}
	put_word( EMMC_CMDTM, total );

	// certain commands require a delay
	switch ( command ) {
		case SPI_CMD_SEND_IF_COND:
		case SPI_CMD_APP_CMD:
			_kernel_systimer_wait_msec( 100 );
			break;
		case SPI_ACMD_SD_SEND_OP_COND:
			_kernel_systimer_wait_msec( 1000 );
			break;
	}

	// await response confirming command has completed
	i = 1000;
	while ( i && !( get_word( EMMC_INTERRUPT ) & EMMC_REGISTER_INTERRUPT_COMMAND_DONE ) ) {
		_kernel_systimer_wait_msec( 1 );
		i--;
	}
	if ( !i ) {
		_kernel_panic( "EMMC", "Timeout waiting for command to complete" );
	}
	//put_word( EMMC_INTERRUPT, EMMC_REGISTER_INTERRUPT_COMMAND_DONE );

	// get response data
	switch ( command ) {
		case SPI_ACMD_SD_SEND_OP_COND:
			ocr = get_word( EMMC_RESP0 );
			break;
		case SPI_CMD_SEND_REL_ADDR:
			rca = get_word( EMMC_RESP0 ) & EMMC_SEND_REL_ADDR_RESPONSE_RCA_MASK;
			break;
		case SPI_CMD_ALL_SEND_CID:
		case SPI_CMD_SEND_CID:
			manufacturer[ 0 ] = get_word( EMMC_RESP3 );
			manufacturer[ 1 ] = get_word( EMMC_RESP2 );
			manufacturer[ 2 ] = get_word( EMMC_RESP1 );
			manufacturer[ 3 ] = get_word( EMMC_RESP0 );
			_kernel_emmc_extract_manufacturer();
			break;
		case SPI_CMD_SEND_CSD:
			// PartA2_SD Host_Controller_Simplified_Specification_Ver4.20.pdf
			csd[ 0 ] = get_word( EMMC_RESP3 );
			csd[ 1 ] = get_word( EMMC_RESP2 );
			csd[ 2 ] = get_word( EMMC_RESP1 );
			csd[ 3 ] = get_word( EMMC_RESP0 );
			// shift into place (only bits 127..8 are actually returned, and returned in bits 119..0)
			csd[ 0 ] = ( csd[ 0 ] << 8 ) + ( csd[ 1 ] >> 24 );
			csd[ 1 ] = ( csd[ 1 ] << 8 ) + ( csd[ 2 ] >> 24 );
			csd[ 2 ] = ( csd[ 2 ] << 8 ) + ( csd[ 3 ] >> 24 );
			csd[ 3 ] = ( csd[ 3 ] << 8 );
			break;
	}

}



/**
 *
 * _kernel_emmc_configure_sd_clock
 *
 * Configures the SD clock.
 *
 */
int _kernel_emmc_configure_sd_clock( unsigned int f )
{

    unsigned int d, c, x, s, h, i;

	// determine the ratio of the required clock rate against the EMMC clock rate
    c = emmc_clock_rate / f;
    s = 32;
    h = 0;

    i = 10000;
    while ( ( get_word( EMMC_STATUS ) & ( EMMC_REGISTER_STATUS_CMD_INHIBIT | EMMC_REGISTER_STATUS_DAT_INHIBIT ) ) && i-- )
    	_kernel_systimer_wait_msec( 1 );
    if ( !i ) {
		_kernel_panic( "EMMC", "SD clock configure timeout" );
    }

    // disable the clock
    put_word( EMMC_CONTROL1, get_word( EMMC_CONTROL1 ) & ~EMMC_REGISTER_CONTROL_1_CLK_EN );
    _kernel_systimer_wait_msec( 10 );

	// calculate the clock value required
    x = c - 1;
    if ( !x )
    	s = 0;
    else {
        if ( !( x & 0xffff0000u ) ) { x <<= 16; s -= 16; }
        if ( !( x & 0xff000000u ) ) { x <<=  8; s -=  8; }
        if ( !( x & 0xf0000000u ) ) { x <<=  4; s -=  4; }
        if ( !( x & 0xc0000000u ) ) { x <<=  2; s -=  2; }
        if ( !( x & 0x80000000u ) ) { x <<=  1; s -=  1; }
        if ( s > 0 ) s--;
        if ( s > 7 ) s = 7;
    }
    d = c;
    if ( d <= 2 ) {
    	d = 2; s = 0;
    }

    // _kernel_video_print_string( "[SDCLK] Divider value calculated as 0x" ); _kernel_video_print_hex( d ); _kernel_video_print_string( "\n" );
    h = ( d & 0x300 ) >> 2;
    d = ( ( ( d & 0x0ff ) << 8 ) | h );

    // set and enable the clock; wait and check it stabilises
    put_word( EMMC_CONTROL1, ( get_word( EMMC_CONTROL1 ) & 0xffff003f ) | d );
    _kernel_systimer_wait_msec( 10 );

    put_word( EMMC_CONTROL1, get_word( EMMC_CONTROL1 ) | EMMC_REGISTER_CONTROL_1_CLK_EN );
    _kernel_systimer_wait_msec( 10 );

    i = 1000;
    while ( !( get_word( EMMC_CONTROL1 ) & EMMC_REGISTER_CONTROL_1_CLK_STABLE ) && i-- ) {
    	_kernel_systimer_wait_msec( 10 );
    }
    if ( !i ) {
		_kernel_panic( "EMMC", "Failed to get stable clock" );
    }

    return 0;

}



/**
 *
 * _kernel_emmc_read_block
 *
 * Reads a single block of data from the device.
 *
 */
void _kernel_emmc_read_block( unsigned int block_number, unsigned char *ptr ) {

	unsigned int count, loop, counter, data, total, i;

	while ( get_word( EMMC_STATUS ) & ( 1 | SR_DAT_INHIBIT ) ) { }

	// SDSC Card (CCS=0) uses byte unit address; SDHC/SDXC Cards (CCS=1) use block unit address (512 bytes)
	put_word( EMMC_BLKSIZECNT, (1 << 16) | 512 );
	put_word( EMMC_INTERRUPT, (1<<1) ); // clear the "data transfer finished" interrupt
	_kernel_emmc_command( SPI_CMD_READ_SINGLE_BLOCK, block_number, EMMC_COMMAND_RESPONSE_TYPE_48_BITS );

	while ( !( get_word( EMMC_INTERRUPT ) & EMMC_REGISTER_INTERRUPT_READ_READY ) ) {}
	for ( counter = 0; counter < 512; counter += 4 ) {
		data = get_word( EMMC_DATA );
		*ptr++ = ( data       ) &0xff;
		*ptr++ = ( data >>  8 ) &0xff;
		*ptr++ = ( data >> 16 ) &0xff;
		*ptr++ = ( data >> 24 ) &0xff;
	}

	put_word( EMMC_INTERRUPT, EMMC_REGISTER_INTERRUPT_READ_READY );

}
