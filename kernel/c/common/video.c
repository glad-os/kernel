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
#include "const.h"
#include "video.h"
#include "mailbox.h"
#include "font_acorn.h"
#include "start.h"
#include "stdlib.h"
#include "define.h"


unsigned int            mode, width, height, cursor_x, cursor_y;
volatile uint32_t       mailbox_buffer[ 256 ] __attribute__( ( aligned(16) ) );
uintptr_t               video_ram_address = 0, cursor_vram_address = 0;
extern unsigned char    pi_logo_map[];
uintptr_t               colour_f, colour_b;


void _kernel_video_display_memory	( unsigned int tag );
void _kernel_video_scroll_up			( void );
void _kernel_video_show_revision		( void );
void _kernel_video_show_buildtype	( void );


void _kernel_video_display_logo( void )
{

	unsigned int x,y, a, r, g, b, i;
	i = 0;
	for ( y = 0; y < 32; y++ ) {
		for ( x = 0; x < 32; x++ ) {
			b = pi_logo_map[ i++ ];
			g = pi_logo_map[ i++ ];
			r = pi_logo_map[ i++ ];
			a = pi_logo_map[ i++ ];
			_kernel_video_plot_pixel( (VIDEO_WIDTH-32)+x, y, (a<<24)+(r<<16)+(g<<8)+b );
		}
	}

}



/**
 *
 * _kernel_video_init
 *
 * Initialisation process for video subsystem.
 *
 */
int _kernel_video_init( void )
{

	extern uint64_t _kernel_peek_value( void );

	_kernel_video_set_mode( 7 );
	_kernel_video_display_logo();
	_kernel_video_show_revision();
	_kernel_video_display_memory( ARM_MAILBOX_TAG_ARM_MEMORY );
	_kernel_video_show_buildtype();
	_kernel_video_print_string( "\n\n" "IMP OS " OS_VERSION " (" OS_COMPILE_DATETIME ")\n\n" );

	return 0;

}



/**
 *
 * _kernel_video_show_buildtype
 *
 * Identifies whether this is a 32 or 64 bit build
 *
 */
void _kernel_video_show_buildtype( void )
{

	#if ISA_TYPE == 64
		_kernel_video_set_colour( 1, 0x0000ff00 );
		_kernel_video_print_string( " 64-bit" );
	#else
		_kernel_video_set_colour( 1, 0x00ff0000 );
		_kernel_video_print_string( " 32-bit" );
	#endif

	_kernel_video_set_colour( 1, 0x00ffffff );

}



/**
 *
 * _kernel_video_show_revision
 *
 * Identifies the Raspberry Pi board revision and echoes it on-screen.
 *
 */
void _kernel_video_show_revision( void )
{

	unsigned int index, board_revision;

	// 0 => request
	index = 1;
	mailbox_buffer[ index++ ] = 0;

	/* format: tag, value buffer size, 0 ('process request'), <<params/value buffer>>) */
	mailbox_buffer[ index++ ] = ARM_MAILBOX_TAG_BOARD_REVISION;
	mailbox_buffer[ index++ ] = 4;
	mailbox_buffer[ index++ ] = 0;			// this gets changes to 0x80000004 -> top bit = b31 set: response, and b30-b0: value length in bytes
	mailbox_buffer[ index++ ] = 0;			// this is where result should be found (index #5)

	mailbox_buffer[ index++ ] = ARM_MAILBOX_TAG_TERMINATOR;

	mailbox_buffer[ 0 ] = index * 4;		// Buffer size

	_kernel_mailbox_write( 8, (uintptr_t) mailbox_buffer );
	_kernel_mailbox_read( 8 );

	board_revision = mailbox_buffer[ 5 ];

	// https://elinux.org/RPi_HardwareHistory
	char *RASPBERRY_PI_MODEL_A 		= "A";
	char *RASPBERRY_PI_MODEL_B 		= "B";
	char *RASPBERRY_PI_MODEL_B_PLUS 	= "B+";
	char *RASPBERRY_PI_COMPUTE_MODULE_1 	= "COMPUTE MODULE 1";
	char *RASPBERRY_PI_MODEL_A_PLUS 	= "A+";
	char *RASPBERRY_PI_2_MODEL_B 		= "2 MODEL B";
	char *RASPBERRY_PI_ZERO 		= "ZERO";
	char *RASPBERRY_PI_ZERO_W 		= "ZERO W";
	char *RASPBERRY_PI_3_MODEL_B 		= "3 MODEL B";
	char *RASPBERRY_PI_COMPUTE_MODULE_3	= "COMPUTE MODULE 3";
	char *RASPBERRY_PI_3_MODEL_B_PLUS 	= "3 MODEL B+";
	char *RASPBERRY_PI_3_MODEL_A_PLUS 	= "3 MODEL A+";

	_kernel_video_print_string( "RASPBERRY PI " );

    	switch ( board_revision )
    	{

		case 0x00000002 : 
		case 0x00000003 : 
		case 0x00000004 : 
		case 0x00000005 : 
		case 0x00000006 : 
		case 0x0000000d : 
		case 0x0000000e : 
		case 0x0000000f : 
			_kernel_video_print_string( RASPBERRY_PI_MODEL_B );
			break;

		case 0x00000007 : 
		case 0x00000008 : 
		case 0x00000009 : 
			_kernel_video_print_string( RASPBERRY_PI_MODEL_A );
			break;

		case 0x00a01040 : 
		case 0x00a01041 : 
		case 0x00a21041 : 
		case 0x00a22042 : 
			_kernel_video_print_string( RASPBERRY_PI_2_MODEL_B );
			break;

		case 0x00000010 : 
		case 0x00000013 : 
		case 0x00900032 : 
			_kernel_video_print_string( RASPBERRY_PI_MODEL_B_PLUS );
			break;

		case 0x00000011 : 
		case 0x00000014 : 
			_kernel_video_print_string( RASPBERRY_PI_COMPUTE_MODULE_1 );
			break;

		case 0x00000012 : 
		case 0x00000015 : 
		case 0x00900021 : 
			_kernel_video_print_string( RASPBERRY_PI_MODEL_A_PLUS );
			break;

		case 0x00900092 : 
		case 0x00900093 : 
		case 0x00920093 : 
			_kernel_video_print_string( RASPBERRY_PI_ZERO );
			break;

		case 0x009000c1 : 
			_kernel_video_print_string( RASPBERRY_PI_ZERO_W );
			break;

		case 0x00a02082 : 
		case 0x00a22082 : 
		case 0x00a32082 : 
			_kernel_video_print_string( RASPBERRY_PI_3_MODEL_B );
			break;

		case 0x00a020a0 : 
			_kernel_video_print_string( RASPBERRY_PI_COMPUTE_MODULE_3 );
			break;

		case 0x00a020d3 : 
			_kernel_video_print_string( RASPBERRY_PI_3_MODEL_B_PLUS );
			break;

		case 0x009020e0 : 
			_kernel_video_print_string( RASPBERRY_PI_3_MODEL_A_PLUS );
			break;

		default       	: 
			_kernel_video_print_string( "Unknown" );
			break;

	}

	_kernel_video_print_string( " " );

}



/**
 *
 * _kernel_video_set_mode
 *
 * Identifies the Raspberry Pi board revision and echoes it on-screen.
 *
 */
void _kernel_video_set_mode( unsigned int n )
{

	unsigned int index = 1, p_width = VIDEO_WIDTH, p_height = VIDEO_HEIGHT, bpp = 32;

	// @todo Mailbox facility needs a more useful implementation to make these things easier to express
	mailbox_buffer[ index++ ] = 0;
	mailbox_buffer[ index++ ] = ARM_MAILBOX_TAG_SET_PHYSICAL_WIDTH_HEIGHT;
	mailbox_buffer[ index++ ] = 8;
	mailbox_buffer[ index++ ] = 0;
	mailbox_buffer[ index++ ] = p_width;
	mailbox_buffer[ index++ ] = p_height;
	mailbox_buffer[ index++ ] = ARM_MAILBOX_TAG_SET_VIRTUAL_WIDTH_HEIGHT;
	mailbox_buffer[ index++ ] = 8;
	mailbox_buffer[ index++ ] = 0;
	mailbox_buffer[ index++ ] = p_width;
	mailbox_buffer[ index++ ] = p_height;
	mailbox_buffer[ index++ ] = ARM_MAILBOX_TAG_SET_DEPTH;
	mailbox_buffer[ index++ ] = 4;
	mailbox_buffer[ index++ ] = 0;
	mailbox_buffer[ index++ ] = bpp;
	mailbox_buffer[ index++ ] = ARM_MAILBOX_TAG_ALLOCATE_BUFFER;
	mailbox_buffer[ index++ ] = 8;
	mailbox_buffer[ index++ ] = 0;
	mailbox_buffer[ index++ ] = 0;
	mailbox_buffer[ index++ ] = 0;
	mailbox_buffer[ index++ ] = ARM_MAILBOX_TAG_TERMINATOR;
	mailbox_buffer[ 0 ] 	  = index * 4;

	_kernel_mailbox_write( 8, (uintptr_t) mailbox_buffer );
	_kernel_mailbox_read( 8 );

	mode = n;
	video_ram_address = (uintptr_t) mailbox_buffer[ 19 ] & 0x3fffffff;
	width = VIDEO_WIDTH; height = VIDEO_HEIGHT;

	// mode change always sets foreground/background to white/black, clears the screen, and resets cursor position
	_kernel_video_set_colour( 1, 0x00ffffff );
	_kernel_video_set_colour( 0, 0x00000000 );
	_kernel_video_cls();

}



/**
 *
 * _kernel_video_display_memory
 *
 * Identifies the Raspberry Pi available RAM and echoes it on-screen.
 *
 */
void _kernel_video_display_memory( unsigned int tag )
{

	unsigned int *mailbox_buffer = (unsigned int *) ( 948 * MBYTE ); // 947 will sit in ARM memory and get cached
	unsigned int index = 1;

	mailbox_buffer[ index++ ] = 0;						// process request (return value should be 0x80000000 for success, 0x80000001 for failure/partial response)
	mailbox_buffer[ index++ ] = tag;					// e.g. ARM_MAILBOX_TAG_ARM_MEMORY
	mailbox_buffer[ index++ ] = 8;						// value buffer size (in bytes)
	mailbox_buffer[ index++ ] = 0;						// request code
	mailbox_buffer[ index++ ] = 0;						// [value buffer]
	mailbox_buffer[ index++ ] = 48*1024; 					// [value buffer]
	mailbox_buffer[ index++ ] = ARM_MAILBOX_TAG_TERMINATOR;
	mailbox_buffer[ index++ ] = 0xffffffff;
	index--;
	mailbox_buffer[ 0 ] = index * 4;					// Buffer size (in bytes)

	_kernel_mailbox_write( 8, (uintptr_t) mailbox_buffer );
	index = _kernel_mailbox_read( 8 );

	_kernel_video_print_decimal( (unsigned int) ( mailbox_buffer[ 6 ] / MBYTE ) ); _kernel_video_print_string( "MB" );

}



/**
 *
 * _kernel_video_cls
 *
 * Clears the display with the current background colour and resets the character position to top-left
 *
 */
void _kernel_video_cls( void )
{

    // for now, hardwire this - 
	unsigned int src, dst, kb;

	kb = (VIDEO_WIDTH * 4 * (VIDEO_HEIGHT-8)) / 1024;
	blank1k( video_ram_address, kb );

	// cls automatically resets the character position to top-left
	_kernel_video_set_character_position( 0,0 );

}



/**
 *
 * _kernel_video_set_colour
 *
 * Sets the foreground or background colour.
 *
 * 0xAARRGGBB
 *
 */
void _kernel_video_set_colour( unsigned int foreground, unsigned int colour )
{

	if ( foreground ) {
        // set a 64-bit value showing the "filter" for this colour
        colour_f =  ((uintptr_t) colour << 32 ) + colour;
	} else {
        // set a 64-bit value showing the "filter" for this colour
        colour_b =  ((uintptr_t) colour << 32 ) + colour;
	}

}



/**
 *
 * _kernel_video_set_character_position
 *
 * Sets the character position.
 *
 */
void _kernel_video_set_character_position( unsigned int x, unsigned int y )
{

	cursor_x = x;
	cursor_y = y;
    cursor_vram_address = video_ram_address + ((y*8)*width*4) + ((x*8)*4);

}



/**
 *
 * _kernel_video_print_string
 *
 * Displays the specified zero-terminated character string.
 *
 */
int _kernel_video_print_string( char *string )
{

	unsigned int index, chr;

	index = 0;
	while ( ( chr = string[ index++ ] ) )
	{
		_kernel_video_print_char( chr );
		if ( chr == 0x00 ) { return 0; }
	}

    return 0;

}



/**
 *
 * _kernel_video_print_char
 *
 * Displays the specified character.
 *
 */
void _kernel_video_print_char( char c )
{

	int loop_x , loop_y, colour;
	int print = 1, advance = 1;

	switch ( c )
	{

		case 0x7f :
			if ( cursor_x == 0 )
			{
                _kernel_video_set_character_position( (width/8)-1, cursor_y );
				if ( cursor_y > 0 ) { _kernel_video_set_character_position( cursor_x, cursor_y-1 ); }
			} else {
                _kernel_video_set_character_position( cursor_x - 1, cursor_y );
			}
			c = 0x20;
			advance = 0;
			break;

		case 0x0a :
            _kernel_video_set_character_position( 0, cursor_y + 1 );
			if ( cursor_y >= (height/8) )
			{
				_kernel_video_scroll_up();
                _kernel_video_set_character_position( 0, cursor_y - 1 );
			}
			print = 0; advance = 0;
			break;

	}

	if ( print )
	{

        _fastcharplot( c );

		if ( advance )
		{
            _kernel_video_set_character_position( cursor_x + 1, cursor_y );
			if ( cursor_x >= (width/8) )
			{
                _kernel_video_set_character_position( 0, cursor_y + 1 );
			}
			if ( cursor_y >= (height/8) )
			{
				_kernel_video_scroll_up();
                _kernel_video_set_character_position( cursor_x, cursor_y - 1 );
			}
		}
	}

}



/**
 *
 * _kernel_video_plot_pixel
 *
 * Plots a single pixel at the given (x,y) co-ordinate using the specified colour.
 *
 */
void _kernel_video_plot_pixel( unsigned int x, unsigned int y, unsigned int colour )
{

	put_word( video_ram_address + (y*width*4) + (x*4), colour );

}



/**
 *
 * _kernel_video_get_pixel
 *
 * Returns the pixel value at the given (x,y) co-ordinate.
 *
 */
unsigned int _kernel_video_get_pixel( unsigned int x, unsigned int y )
{

	return get_word( video_ram_address + (y*width*4) + (x*4) );

}



/**
 *
 * _kernel_video_get_mode
 *
 * Returns the current mode.
 *
 */
unsigned int _kernel_video_get_mode( void )
{

	return mode;

}



/**
 *
 * _kernel_video_print_decimal
 *
 * Given an integer, displays it in decimal.
 *
 */
void _kernel_video_print_decimal( unsigned int number )
{

	/* hideously poor - needs proper algorithm! */
	int divisor = 100000000, quotient;
	char decmap[10] = { '0','1','2','3','4','5','6','7','8','9' };
	int nonzero = 0;

	if ( !number )
	{
		_kernel_video_print_char( decmap[0] );
		return;
	}

	while ( divisor >= 1 )
	{

		quotient = number / divisor;
		if ( quotient ) {
			nonzero = 1;
			number -= ( quotient * divisor );
		}
		if ( nonzero ) {
			_kernel_video_print_char( decmap[ quotient ] );
		}
		divisor /= 10;

	}

}



/**
 *
 * _kernel_video_scroll_up
 *
 * Shifts contents of video RAM up by one character row, and then blanks bottom character row.
 *
 */
void _kernel_video_scroll_up( void )
{

	unsigned int src, dst, kb;

	src = video_ram_address + ( VIDEO_WIDTH * 4 * 8);
    dst = video_ram_address;
	kb = (VIDEO_WIDTH * 4 * (VIDEO_HEIGHT-8)) / 1024;
   	move1k( src, dst, kb );

    // blank the bottom row
    src = video_ram_address + ( VIDEO_WIDTH*4 * (VIDEO_HEIGHT-8) );
    kb = ( VIDEO_WIDTH * 4 * 8 ) / 1024;
	blank1k( src, kb );

}



/**
 *
 * _kernel_video_print_hex
 *
 * Given an integer, displays it in hexadecimal.
 *
 */
void _kernel_video_print_hex( unsigned int h )
{

	char c[ 16 ] = "0123456789ABCDEF", toprint[ 9 ] = "--------\0";
	int i;

	for ( i = 0; i < 8; i++ )
	{
		toprint[ i ] = c[ ( h >> ( ( 7-i ) << 2 ) ) & 0xf ];
	}

	int result = _kernel_video_print_string( toprint );

}
