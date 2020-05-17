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
*/

#include "cb_string.h"
#include "cb_comdefs.h"

#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>


/*===========================================================================
 * DEFINES
 *=========================================================================*/

/*===========================================================================
 * TYPES
 *=========================================================================*/

/*===========================================================================
 * DECLARATIONS
 *=========================================================================*/
static int print(char **out, const char *format, va_list args);

/*===========================================================================
 * DEFINITIONS
 *=========================================================================*/

/*===========================================================================
 * FUNCTIONS
 *=========================================================================*/

void cbSTR_sprintf(cb_char* str, const cb_char *format, ...)
{
    cb_uint32 size;
    va_list args;
    va_start(args, format);
    
    size = cbSTR_snprintfVaList(str, cbSTR_BUFFER_SIZE, format, args);
    
    cb_ASSERT(size <= cbSTR_BUFFER_SIZE);
}

cb_uint32 cbSTR_snprintf(cb_char* str, cb_uint32 size, const cb_char *format, ...)
{
    va_list args;
    cb_char *out = str;
    cb_uint32 n;

    va_start(args, format);
    
	/* TODO: check for buffer overflow! check size variable */
	n = (cb_uint32)print(&out, (const cb_char*)format, args);
	cb_ASSERT(n <= size);

	return n; 
}

cb_char* cbSTR_dataToHex(cb_uint8* pData, cb_uint32 len, cb_char* resultStr)
{
    cb_uint32 i;

    resultStr[0] = 0;

    if (pData != NULL) {
        for (i = 0; i < len; i++) {
            cbSTR_sprintf(&(resultStr[i * 2]), "%02X", pData[i]);
        }
    } else {
        cbSTR_sprintf(&(resultStr[0]), "(null)");
    }

    return resultStr;
}

cb_char* cbSTR_dataToHexLe(cb_uint8* pData, cb_uint32 len, cb_char* resultStr)
{
    cb_uint32 i;

    resultStr[0] = 0;

    if (pData != NULL) {
        for (i = 0; i < len; i++) {
            cbSTR_sprintf(&(resultStr[i * 2]), "%02X", pData[len - i - 1]);
        }
    } else {
        cbSTR_sprintf(&(resultStr[0]), "(null)");
    }

    return resultStr;
}


cb_uint32 cbSTR_snprintfVaList(cb_char *str, cb_uint32 size, const cb_char *format, va_list args)
{
    cb_char *out = str;
    cb_uint32 n;

    n = (cb_uint32)print(&out, format, args);
    cb_ASSERT(n <= size);

    return n;
}

cb_uint32 cbSTR_snvprintf(void* str, cb_uint32 size, const cb_char* format, va_list ap)
{
    cb_char* out = str;
    cb_uint32 n;

    /* TODO: check for buffer overflow! check size variable */
    n = (cb_uint32)print(&out, format, ap);
    cb_ASSERT(n <= size);

    return n;
}


cb_uint32 cbSTR_strlen(const cb_char *str)
{
    return (cb_uint32)strlen(str);
}


cb_uint32 cbSTR_strnlen(const cb_char *str, cb_uint32 length)
{
    return (cb_uint32)strnlen(str, length);
}

cb_char* cbSTR_strncpy(cb_char* to, const cb_char* from, cb_uint32 length)
{
    return strncpy(to, from, (size_t)length);
}
//
cb_int32 cbSTR_strncmp(const cb_char* str1, const cb_char* str2, cb_uint32 length)
{
    return strncmp(str1, str2, (size_t)length);
}

cb_int32 cbSTR_caseCompare(const cb_char* a, const cb_char* b, cb_uint32 length)
{
    if (length == 0) {
        return 0;
    }

    while (length-- != 0 && tolower((int)*a) == tolower((int)*b)) {
        if (length == 0 || *a == '\0') {
            break;
        }
        a++;
        b++;
    }

    return tolower((int)*a) - tolower((int)*b);
}

const cb_char* cbSTR_strrchr(const cb_char* str, cb_char ch)
{
    cb_uint32 endPos = cbSTR_strlen(str);
    const cb_char* p = str + endPos;

    do {
        if (*p == ch) {
            return p;
        }
        p--;
    } while (p >= str);

    return NULL;
}


// Replacement/enhancement for strstr. Find the needle in the haystack starting the search at offset. Return the position of the needle or -1 if not found.
cb_int32 cbSTR_find(const cb_char* haystack, cb_uint32 haystackLength, const cb_char* needle, cb_uint32 needleLength, cb_uint32 offset)
{
    const cb_char* p;

    if (haystack == NULL || haystackLength == 0) {
        return -1;
    }

    if (needle == NULL || needleLength == 0) {
        return -1;
    }

    if (offset + needleLength > haystackLength) {
        return -1;
    }

    p = haystack + offset;
    for (; offset + needleLength <= haystackLength; offset++) {
        if (*p == *needle) {
            if (0 == memcmp(p, needle, needleLength)) {
                return (cb_int32)offset;
            }
        }
        p++;
    }

    return -1;
}

cb_uint32 cbSTR_toByteArray(cb_uint8 *to, const cb_char *from, cb_uint32 toSize)
{
    cb_uint32 i = 0;
    cb_uint32 len;
    cb_uint32 toIndex = 0;

    len = cbSTR_strlen(from);

    while ((toIndex < toSize) && ((i + 1) < len)) {
        if (!cbSTR_isHex(from[i]) || !cbSTR_isHex(from[i + 1])) {
            // Invalid byte array return converted size
            return toIndex;
        }

        to[toIndex++] = (cb_uint8)((cbSTR_toDecimal(from[i]) << 4) | cbSTR_toDecimal(from[i + 1]));
        i += 2;
    }

    return toIndex;
}

cb_int32 cbSTR_atoi(const cb_char *nptr)
{
    return cbSTR_strtol(nptr, NULL, 10, NULL);
}

cb_int32 cbSTR_strtol(const cb_char* nptr, const cb_char** endptr, cb_uint32 base, cb_int32* pErrno)
{
    cb_boolean negative = FALSE;
    cb_uint32 value = 0;
    cb_int32 returnVal = 0;
    cb_int32 myErrno = 0;

    switch (base) {
        case 0:
        case 8:
        case 10:
        case 16:
            break;
        default:
            if (pErrno != NULL) {
                *pErrno = EINVAL;
                return 0;
            }
    }


    while (isspace((int)*nptr)) {
        nptr++;
    }

    if (*nptr == '-') {
        negative = TRUE;
        nptr++;
    } else if (*nptr == '+') {
        nptr++;
    }

    if (base == 0) {
        base = 10;
        if (*nptr == '0') {
            base = 8;
            nptr++;
            if (tolower((int)*nptr) == 'x') {
                base = 16;
                nptr++;
            }
        }
    }

    while (cbSTR_isHex(*nptr)) {
        cb_uint32 temp = cbSTR_toDecimal(*nptr);
        value = value * base + temp;
        nptr++;
        if (value > cb_INT32_MAX) {
            myErrno = ERANGE;
            break;
        }
    }

    if (myErrno == ERANGE) {
        returnVal = cb_INT32_MAX;
        if (negative) {
            returnVal = cb_INT32_MIN;
        }
    } else {
        returnVal = (cb_int32)value;
        if (negative) {
            returnVal = -(cb_int32)value;
        }
    }

    if (endptr != NULL) {
        *endptr = nptr;
    }

    if (pErrno != NULL) {
        *pErrno = myErrno;
    }

    return returnVal;
}

/*---------------------------------------------------------------------------
* check if the supplied character is hexadecimal.
*-------------------------------------------------------------------------*/
cb_boolean cbSTR_isHex(cb_char c)
{
    int intChar = c;
    intChar = tolower(intChar);

    return ((intChar >= '0' && intChar <= '9') || (intChar >= 'a' && intChar <= 'f'));
}

/*---------------------------------------------------------------------------
* Convert a decimal or hexadecimal ASCII character to an integer
*-------------------------------------------------------------------------*/
cb_uint8 cbSTR_toDecimal(cb_char c)
{
    c = (cb_char)tolower((int)c);

    if (c >= '0' && c <='9') {
        return (cb_uint8)(c - '0');
    }
    if (c >= 'a' && c <='f') {
        return (cb_uint8)(10 + c - 'a');
    }

    return 0;
}

// Based on http://derykstack.blogspot.com/2011/12/implementation-of-strtok-in-stringh.html
cb_char* cbSTR_strtok(cb_char* s, const cb_char* delim)
{
    static char *last;
    char * tok;

    /* pick up from previous stopping point, if told to do so */
    if (s == NULL && (s = last) == NULL)
        return NULL;

    tok = s + strspn(s, delim);

    s = tok + strcspn(tok, delim);

    /*
    * Save state for next call, and return token. If there
    * is no token, both *tok and *s will be '\0'. If there
    * is one token that is ended with '\0', *s will be '\0'.
    */
    if (*s == '\0')
        last = NULL;
    else {
        *s++ = '\0';
        last = s;
    }

    return *tok == '\0' ? NULL : tok;
}

/*===========================================================================
 * STATIC FUNCTIONS
 *=========================================================================*/

/*
    Copyright 2001, 2002 Georges Menie (www.menie.org)
    stdarg version contributed by Christian Ettinger

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
    putchar is the only external dependency for this file,
    if you have a working putchar, leave it commented out.
    If not, uncomment the define below and
    replace outbyte(c) by your own function call.

#define putchar(c) outbyte(c)
*/


static void printchar(char **str, char c)
{
    /*extern int putchar(int c);*/

    if (str) {
        **str = c;
        ++(*str);
    } else {
        /*(void)putchar(c);*/
    }
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

static int prints(char **out, const char *string, int width, int pad)
{
    register int pc = 0;
    register char padchar = ' ';

    if (width > 0) {
        register int len = 0;
        register const char* ptr;
        for (ptr = string; *ptr; ++ptr) {
            ++len;
        }
        if (len >= width) {
            width = 0;
        } else {
            width -= len;
        }
        if (pad & PAD_ZERO) {
            padchar = '0';
        }
    }
    if (!(pad & PAD_RIGHT)) {
        for (; width > 0; --width) {
            printchar(out, padchar);
            ++pc;
        }
    }
    for (; *string ; ++string) {
        printchar(out, *string);
        ++pc;
    }
    for (; width > 0; --width) {
        printchar(out, padchar);
        ++pc;
    }

    return pc;
}

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12

static int printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
{
    char print_buf[PRINT_BUF_LEN];
    register char* s;
    register int t, neg = 0, pc = 0;
    register int u = i;

    if (i == 0) {
        print_buf[0] = '0';
        print_buf[1] = '\0';
        return prints(out, print_buf, width, pad);
    }

    if (sg && b == 10 && i < 0) {
        neg = 1;
        u = -i;
    }

    s = print_buf + PRINT_BUF_LEN - 1;
    *s = '\0';

    while (u) {
        t = u % b;
        if (t >= 10) {
            t += letbase - '0' - 10;
        }
        *--s = (char)(t + '0');
        u /= b;
    }

    if (neg) {
        if (width && (pad & PAD_ZERO)) {
            printchar(out, '-');
            ++pc;
            --width;
        } else {
            *--s = '-';
        }
    }

    return pc + prints(out, s, width, pad);
}
/* the following should be enough for 64 bit int */
#define PRINT_BUF_LONG_LEN 20

static int printll(char **out, long long i, int b, int sg, int width, int pad, int letbase)
{
    char print_buf[PRINT_BUF_LONG_LEN];
    register char* s;
    register int neg = 0, pc = 0;
    long long u = i;
    long long t;

    if ( i == 0 ) {
        print_buf[0] = '0';
        print_buf[1] = '\0';
        return prints(out, print_buf, width, pad);
    }

    if ( sg && b == 10 && i < 0 ) {
        neg = 1;
        u = -i;
    }

    s = print_buf + PRINT_BUF_LONG_LEN - 1;
    *s = '\0';

    while ( u ) {
        t = u % b;
        if ( t >= 10 ) {
            t += letbase - '0' - 10;
        }
        *--s = (char) (t + '0');
        u /= b;
    }

    if ( neg ) {
        if ( width && (pad & PAD_ZERO) ) {
            printchar(out, '-');
            ++pc;
            --width;
        } else {
            *--s = '-';
        }
    }

    return pc + prints(out, s, width, pad);
}

static int print(char **out, const char *format, va_list args )
{
    register int width, pad;
    register int pc = 0;
    char scr[2];

    for (; *format != 0; ++format) {
        if (*format == '%') {
            ++format;
            width = pad = 0;
            if (*format == '\0') {
                break;
            }
            if (*format == '%') {
                goto out;
            }
            if (*format == '-') {
                ++format;
                pad = PAD_RIGHT;
            }
            while (*format == '0') {
                ++format;
                pad |= PAD_ZERO;
            }
            for (; *format >= '0' && *format <= '9'; ++format) {
                width *= 10;
                width += *format - '0';
            }
            if (*format == 's') {
                register char* s = va_arg(args, char*);
                /*@@HL register char *s = (char *)va_arg( args, int );*/
                pc += prints(out, s ? s : "(null)", width, pad);
                continue;
            }
            if (*format == 'd') {
                pc += printi(out, va_arg(args, int), 10, 1, width, pad, 'a');
                continue;
            }
            if (*format == 'x') {
                pc += printi(out, va_arg(args, int), 16, 0, width, pad, 'a');
                continue;
            }
            if (*format == 'p') {
                pc += printi(out, va_arg(args, int), 16, 0, width, pad, 'a');
                continue;
            }
            if (*format == 'X') {
                pc += printi(out, va_arg(args, int), 16, 0, width, pad, 'A');
                continue;
            }
            if (*format == 'u') {
                pc += printi(out, va_arg(args, int), 10, 0, width, pad, 'a');
                continue;
            }
            if ( *format == 'l' ) { 
                pc += printll(out, va_arg(args, long long), 10, 1, width, pad, 'a');
                continue;
            }
            if (*format == 'c') {
                /* char are converted to int then pushed on the stack */
                scr[0] = (char)va_arg(args, int);
                scr[1] = '\0';
                pc += prints(out, scr, width, pad);
                continue;
            }
            // BD address
            if( *format == 'b' ) {
                int i;
                register unsigned char *s = va_arg( args, unsigned char*);

                for(i=0;i<6;i++)
                {
                    pc += printi (out, s[i], 16, 0, 2, 2, 'a');
                }
                continue;
            }
        }
        else {
        out:
            printchar(out, *format);
            ++pc;
        }
    }
    if (out) {
        **out = '\0';
    }
    va_end(args);
    return pc;
}

