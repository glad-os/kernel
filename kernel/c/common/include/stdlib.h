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



#ifndef STDLIB_H
#define STDLIB_H



	typedef struct {
		unsigned int	hi_bit;
		unsigned int	lo_bit;
	} bitfield;



	int _kernel_sprintf_i( char *s, int i );
	void *_kernel_memcpy( void *dst, void *src, unsigned int n );
	int _kernel_strcmp( const char *pString1, const char *pString2 );

	int _kernel_strlen( const char *pString );
	char *_kernel_strcpy( char *pDest, const char *pSrc );
	char *_kernel_strcat( char *pDest, const char *pSrc );

	uintptr_t _kernel_set_flags( uintptr_t reg, bitfield b, uintptr_t value );



#endif /*STDLIB_H*/
