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



#include "stdlib.h"
#include <stdint.h>



/**
 *
 * _kernel_sprintf_i
 *
 * Takes supplied integer and converts it to string form.
 *
 */
int _kernel_sprintf_i( char *s, int i )
{

	// just get it working for now
	unsigned long int length;
	unsigned long int divisor;
	unsigned long int quotient;
	length = 0;

	if ( i == 0 )
	{

		*s++ = 48;
		length++;

	} else {

		divisor = 1000000000;
		while ( divisor >= 1 )
		{
			quotient = i / divisor;
			if ( quotient || length ) {
				*s++ = quotient + 48;
				length++;
				i -= (quotient * divisor);
			}
			divisor /= 10;
		}

	}

	// stick \0 at end
	*s = 0;
	length++;

	return length;

}



/**
 *
 * _kernel_memcpy
 *
 * Copies block of memory from source to destination. Does not cater for overlapping regions.
 *
 */
void *_kernel_memcpy( void *dst, void *src, unsigned int n )
{

	char *pd = (char *) dst;
	char *ps = (char *) src;

	while (n--)
	{
		*pd++ = *ps++;
	}

	return dst;
}



/**
 *
 * _kernel_strcmp
 *
 * Compares two strings, returning indication of whether 2nd string is equal, greater than or less than string 1.
 *
 */
int _kernel_strcmp( const char *pString1, const char *pString2 )
{

	while (   *pString1 != '\0'
	       && *pString2 != '\0')
	{
		if (*pString1 > *pString2)
		{
			return 1;
		}
		else if (*pString1 < *pString2)
		{
			return -1;
		}

		pString1++;
		pString2++;
	}

	if (*pString1 > *pString2)
	{
		return 1;
	}
	else if (*pString1 < *pString2)
	{
		return -1;
	}

	return 0;

}



/**
 *
 * _kernel_strlen
 *
 * Returns length of supplied string (excluding zero terminator).
 *
 */
int _kernel_strlen( const char *pString )
{

	unsigned long nResult = 0;

	while (*pString++)
	{
		nResult++;
	}

	return nResult;

}



/**
 *
 * _kernel_strcpy
 *
 * Copies source string to destination string (including zero-terminator).
 *
 */
char *_kernel_strcpy( char *pDest, const char *pSrc )
{

	char *p = pDest;

	while (*pSrc)
	{
		*p++ = *pSrc++;
	}

	*p = '\0';

	return pDest;

}



/**
 *
 * _kernel_strcat
 *
 * Appends source string to destination string.
 *
 */
char *_kernel_strcat( char *pDest, const char *pSrc )
{

	char *p = pDest;

	while (*p)
	{
		p++;
	}

	while (*pSrc)
	{
		*p++ = *pSrc++;
	}

	*p = '\0';

	return pDest;

}


/**
 *
 * _kernel_set_flags
 *
 * Given a relevant sized unsigned integer (32/64 bit), allows the field to be set appropriately
 * 
 */
uintptr_t _kernel_set_flags( uintptr_t reg, bitfield b, uintptr_t value )
{

	reg &= ~( ( ( 1 << (b.hi_bit-b.lo_bit+1) ) - 1 ) << b.lo_bit );
	reg |= value << b.lo_bit;

	return reg;

}

