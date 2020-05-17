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

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "cb_at_util.h"
#include "cb_string.h"

#include "cb_tokenizer.h"



 cbRTSL_Status cb_Tokenize(cb_char* pInBuf, cb_uint32 bufSize, cb_uint32 maxTokens,
                                cb_char* tokenArray[], cb_uint8* numTokens)
{
    cbRTSL_Status status = cbSTATUS_OK;
    cb_uint32 tokenCounter = 0;
    cb_char* work;
    cb_char group;
    cb_char operation = '@';
    cb_uint16 i;

    cb_ASSERT(pInBuf != NULL);
    cb_ASSERT(tokenArray != NULL);
    cb_ASSERT(numTokens != NULL);

    *numTokens = 0;

    work = pInBuf;
    group = *work;


    if (group == '+')
    {   // TODO: check tokenCounter == 0 instead (wildcard for anything other than standard or extended).
        tokenArray[tokenCounter] = work;
        tokenCounter++;

        work++;
        work = cbUTIL_findEndOfATCommandToken(work);  // TODO: update so we know if it is query, assignment, or immediate.
        if (work == NULL)
        {
            status = cbSTATUS_ERROR;
        }

        else if (*work != '\0')
        {
            //checking if there is more memory for a single char and a null termination
            //+abcd=1,2 will now become +abcd=\01,2
            cb_ASSERT(cbSTR_strlen(pInBuf) + 2 <= bufSize);
            memmove(work + 1, work, cbSTR_strlen(work) + 1);
        }
        if (OK(status))
        {
            operation = *work;
            work = work + 1;

            if (work != NULL && *work != '\0')
            {
                if(/**work == '?' || *work == '='||*/ *work == ':')
                {
                    *work = '\0';
                }
                work = work + 1;
            }
        }
    } else {
        // Most likely an Echo, OK, ERROR or response to certain AT-commands such as 
        // ATE?, ATSn?, ATIn, ATE?, ATSn?  
        // AT+CGMI, AT+CGMM, AT+CGSN, AT+GMI, AT+GMM, AT+GMR, AT+GSN,
        // AT+UFWUPD
        // that do not comply with the +urc: responses

        // Keep everything in the first token, since we really don't know how to parse this
        // at this level.
        tokenArray[tokenCounter] = work;
        tokenCounter++;
    }

    // Validate operation.
    if (operation == ':') {
        if (*work == '\0')
        {
            // there has to be at least one parameter if there is a trailing '='
            tokenArray[tokenCounter] = "";
            tokenCounter++;
        }
        else
        {
            while (OK(status) && work != NULL && *work != '\0')
            {
                if (tokenCounter == maxTokens)
                {
                    status = cbSTATUS_ERROR;
                    break;
                }

                tokenArray[tokenCounter] = work;
                tokenCounter++;
                work = cbUTIL_findEndOfATParamToken(work);

                if (work == NULL)
                {
                    status = cbSTATUS_ERROR;
                }

                if (work != NULL && *work != '\0')
                {
                    if (*(work + 1) == '\0')
                    {
                        //in case of trailing commas
                        tokenArray[tokenCounter] = "";
                        tokenCounter++;
                    }
                    *work = '\0';
                    work = work + 1;
                }
            }
        }
    }

    *numTokens = (cb_uint8)tokenCounter;
    return status;
}
