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


#include "cb_at_util.h"
#include "cb_string.h"

#include <ctype.h>

#define cbUTIL_ASCII_BD_ADDR_LEN 12
cb_char* cbUTIL_eatWhiteSpace(cb_char* str) //removes leading spaces in the string
{
    if (str == NULL) 
    {
        return str;
    }

    while (isblank((cb_uint8)*str))
    {
        str++;
    }
    return str; 
}

cb_char* cbUTIL_eatEol(cb_char* str) //Removes EOL characeters in the string
{
    if (str == NULL)
    {
        return str;
    }
    int index = 0;
    while (str[index] != '\0')
    {
        if (((cb_char)str[index]) == '\n' || 
            ((cb_char)str[index]) == '\r')
        {
            str[index] = '\0';
        }
        index++;
    }

    return str;
}

cb_char* cbUTIL_eatSubstring(cb_char* start) //remove characters inside quotes along with "
{
    cb_ASSERT(start != NULL);
    cb_ASSERT(*start == '\"');
    cb_char* end = start + 1;
    
    while (*end != '\"')
    {
        if (*end == '\0')
        {
            // TODO output debug string
            return NULL;
        }
        end++;
    }

    end++;    // eat last "
    return end;
}

cbRTSL_Status cbUTIL_stripQuotes(cb_char** str) 
{
    cb_ASSERT(str != NULL);

    if (*str == NULL)
    {
        return cbSTATUS_OK;
    }
    
    if (**str == '\"')
    {
        cb_uint32 length;

        (*str)++;   // Remove first "
        length = cbSTR_strlen(*str);
        if ((*str)[length - 1] != '\"')
        {
            return cbSTATUS_ERROR;
        }
        (*str)[length - 1] = '\0'; // Remove last "
    }
    return cbSTATUS_OK;
}

cb_char* cbUTIL_findEndOfATParamToken(cb_char* tokenStart) 
{
    cb_ASSERT(tokenStart != NULL);
    cb_char* tokenEnd = tokenStart;

    if (*tokenStart == '\"')
    {
        tokenEnd = cbUTIL_eatSubstring(tokenEnd);
        if (tokenEnd == NULL)
        {
            return NULL;
        }
    }
    else
    {
        while (isprint((cb_uint8)*tokenEnd) && *tokenEnd != ',')
        {
            tokenEnd++;
        }
    }

    if (*tokenEnd != ',' && *tokenEnd != '\0' )
    {
        return NULL;
    }

    return tokenEnd;
}

cb_char* cbUTIL_findEndOfATCommandToken(cb_char* tokenStart) 
{
    cb_ASSERT(tokenStart != NULL);
    cb_char* tokenEnd;
    if (!isalpha((cb_uint8)*tokenStart))
    {
        // According to ITU Recommendation V.250 for AT commands
        // At command tokens begin with alphabetical characters but can hold numeric characters
        return NULL;
    }
    tokenEnd = tokenStart + 1;
    while (isalnum((cb_uint8)*tokenEnd))
    {
        tokenEnd++;
    }

    if (*tokenEnd != ':' && *tokenEnd != '\0')
    {
        return NULL;
    }

    return tokenEnd;
}

cb_boolean cbUTIL_hasOnlyNumbers(const cb_char* tokenString) // checks if all characters of a string are numbers
{
    cb_boolean hasOnlyNumbers = TRUE;
    if (*tokenString == '-')
    {
        tokenString += 1;
    }

    do
    {
        if (!isdigit((cb_uint8)*tokenString))
        {
            hasOnlyNumbers = FALSE;
            break;
        }
        tokenString = tokenString + 1;
    } while (hasOnlyNumbers && *tokenString != '\0');

    return hasOnlyNumbers;
}

cb_boolean cbUTIL_isValidByteArray(const cb_char* tokenString) // checks if all characters of a string are numbers
{
    cb_uint32 len = 0;
    cb_boolean validByteArray = TRUE;
    cb_ASSERT(FALSE); //This function is not tested and verified!! If you intend to use it, verify it as well
    do
    {
        if (!isxdigit((cb_uint8)*tokenString))
        {
            validByteArray = FALSE;
            break;
        }
        tokenString = tokenString + 1;
        len++;
    } while (validByteArray && *tokenString != '\0');

    if (len % 2 != 0)
    {
        validByteArray = FALSE;
    }

    return validByteArray;
}

cb_boolean cbUTIL_isValidBDAddr(const cb_char* tokenString) // checks if all characters of a string are numbers
{
    cb_uint32 len = 0;
    cb_boolean valiBdAddrArray = TRUE;
    cb_ASSERT(FALSE); //This function is not tested and verified!! If you intend to use it, verify it as well
    do
    {
        if (!isxdigit((cb_uint8)tokenString[0]))
        {
            valiBdAddrArray = FALSE;
            break;
        }
        tokenString = tokenString + 1;
        len++;
    } while (valiBdAddrArray && tokenString[0] != '\0' && len < cbUTIL_ASCII_BD_ADDR_LEN);

    if (valiBdAddrArray && len == cbUTIL_ASCII_BD_ADDR_LEN)
    {
        // Parsed the first 12 characters, next can be p, r or null
        if (tokenString[0] != 'r' && tokenString[0] != 'p' && tokenString[0] != 'R' && tokenString[0] != 'P' && tokenString[0] != '\0')
        {
            valiBdAddrArray = FALSE;
        }
        // if p or r, verify it's null terminated
        if (valiBdAddrArray && tokenString[0] != '\0' && tokenString[1] != '\0')
        {
            valiBdAddrArray = FALSE;
        }
    }
    else
    {
        valiBdAddrArray = FALSE;
    }

    return valiBdAddrArray;
}

