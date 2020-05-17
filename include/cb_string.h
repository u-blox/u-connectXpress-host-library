/**
 \copyright Copyright (C) 2020 u-blox 

u-blox reserves all rights in this deliverable (documentation, software, etc.,
hereafter “Deliverable”). 

u-blox grants you the right to use, copy, modify and distribute the
Deliverable provided hereunder for any purpose without fee.

THIS DELIVERABLE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
WARRANTY. IN PARTICULAR, NEITHER THE AUTHOR NOR U-BLOX MAKES ANY
REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY OF THIS
DELIVERABLE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.

In case you provide us a feedback or make a contribution in the form of a
further development of the Deliverable (“Contribution”), u-blox will have the
same rights as granted to you, namely to use, copy, modify and distribute the
Contribution provided to us for any purpose without fee.

Component:  u-connectXpress Host Library
\file
*/


#ifndef _CB_STRING_H_
#define _CB_STRING_H_

#include "cb_comdefs.h"
#include <stdarg.h>

/*! 
 *  @{
 * \libfile Internal functions.
 * \libfuns
*/

cb_uint32 cbSTR_strlen(const cb_char* str);
cb_uint32 cbSTR_strnlen(const cb_char *str, cb_uint32 length);
cb_char* cbSTR_strncpy(cb_char* to, const cb_char* from, cb_uint32 length);
cb_int32 cbSTR_strncmp(const cb_char* str1, const cb_char* str2, cb_uint32 length);
cb_int32 cbSTR_caseCompare(const cb_char* a, const cb_char* b, cb_uint32 length);
const cb_char* cbSTR_strrchr(const cb_char* str, cb_char ch);
cb_char* cbSTR_strtok(cb_char* s, const cb_char* delim);

/*---------------------------------------------------------------------------
 * Memory efficient snprintf function
 *
 * Recommended instead of the standard C-lib snprintf
 *-------------------------------------------------------------------------*/
void cbSTR_sprintf(cb_char* str, const cb_char *format, ...);
cb_uint32 cbSTR_snprintf(cb_char* pBuf, cb_uint32 maxBufSize, const cb_char *format, ...);
cb_uint32 cbSTR_snprintfVaList(cb_char *pBuf, cb_uint32 maxBufSize, const cb_char *format, va_list args);

cb_char* cbSTR_dataToHex(cb_uint8* pData, cb_uint32 len, cb_char* resultStr);
/*---------------------------------------------------------------------------
 * Little endian variant of cbSTR_dataToHex
 *-------------------------------------------------------------------------*/
cb_char* cbSTR_dataToHexLe(cb_uint8* pData, cb_uint32 len, cb_char* resultStr);

/**
* Convert an ascii hex string to corresponding byte array. The conversion will stop if
* an invalid character is detected, i.e a character that is not 0-9, a-f, or A-F
*
* @param to buffer to put the converted bytes in
* @param from null terminated ascii string representing bytes
* @param toSize size of buffer
*
* @return number of bytes actually converted.
*
*/
cb_uint32 cbSTR_toByteArray(cb_uint8 *to, const cb_char *from, cb_uint32 toSize);

cb_boolean cbSTR_isHex(cb_char c);
cb_uint8 cbSTR_toDecimal(cb_char c);

/*---------------------------------------------------------------------------
 * support of va_list params.
 *-------------------------------------------------------------------------*/
cb_uint32 cbSTR_snvprintf(void* str, cb_uint32 size, const cb_char* format, va_list ap);

/*---------------------------------------------------------------------------
 * Replacement/enhancement for strstr.
 * Find the needle in the haystack starting the search at offset.
 * Return the position of the needle or -1 if not found.
 *-------------------------------------------------------------------------*/
cb_int32 cbSTR_find(const cb_char* haystack, cb_uint32 haystackLength, const cb_char* needle, cb_uint32 needleLength, cb_uint32 offset);

cb_int32 cbSTR_atoi(const cb_char *nptr);
cb_int32 cbSTR_strtol(const cb_char* nptr, const cb_char** endptr, cb_uint32 base, cb_int32* pErrno);

/*! @} */

#endif

