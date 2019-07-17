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



#include "keyboard.h"
#include "video.h"

// [64-bit] exclude USPi from the build for the moment

#include "../../../uspi/env/include/uspienv/util.h"
#include "../../../uspi/env/include/uspienv.h"
#include "../../../uspi/include/uspi.h"
#include "../../../uspi/include/uspios.h"



volatile char buffered = 0;

// [64-bit] exclude KeyPressedHandler for the moment
static void KeyPressedHandler( const char *pString );



/**
 *
 * _kernel_keyboard_init
 *
 * Initialisation process for keyboard subsystem.
 *
 */
void _kernel_keyboard_init( void )
{

	// [64-bit] exclude USPi from the build for the moment
	
	//_kernel_video_print_string( "Env Init...\n" );
	if ( !USPiEnvInitialize() )
	{
		_kernel_video_print_string( "USB ENVIRONMENT FAIL\n" );
		return;
	}

	//_kernel_video_print_string( "Init...\n" );
	if ( !USPiInitialize() )
	{
		_kernel_video_print_string( "USB INITIALIZATION FAIL\n" );
		return;
	}

	//_kernel_video_print_string( " Check...\n" );
	if ( !USPiKeyboardAvailable() )
	{
		_kernel_video_print_string( "KEYBOARD FAIL\n" );
		return;
	}

	USPiKeyboardRegisterKeyPressedHandler( KeyPressedHandler );
	

}



/**
 *
 * KeyPressedHandler
 *
 * Initialisation process for systimer subsystem.
 *
 */
// [64-bit] exclude KeyPressedHandler for the moment

static void KeyPressedHandler( const char *pString )
{

	buffered = pString[ 0 ];

}




/**
 *
 * _kernel_keyboard_readc
 *
 * Returns value of next key pressed.
 *
 */
unsigned int _kernel_keyboard_readc( void )
{

	unsigned int tmp;

	// wait for key, find out code, and return with it
	while ( !buffered ) { }

	tmp = buffered;
	buffered = 0;

	return tmp;

}
