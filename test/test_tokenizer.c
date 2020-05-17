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

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "test_tools.h"

#undef OK

#include "cb_tokenizer.h"
#include "ucxh_library.h"
#include "ucxh_serial.h"

#include "ex_config.h"

/** 
 * \file test_tokenizer.c
 * \test test_tokenizer.c Unit test: Test the tokenizer of the URC parser.
 * \brief Tests the tokenizer of the URC parser.
   
## Introduction

Unit-tests. 

Full source in `test/test_tokenizer.c`

## Typical response

    Tokenizer OK

*/

char buffer[256];
char *tokenArray[8];

int main()
{
  bool result = false;

  char *pInBuf = NULL;
  uint8_t numTokens = -1;
  cbRTSL_Status status = cbSTATUS_ERROR;

  strcpy(buffer, "+UNSTAT:7,42,\"abcd\"");
  status = cb_Tokenize(buffer, sizeof(buffer), ELEMENTS_OF(tokenArray),
                       tokenArray, &numTokens);
  assert(numTokens == 4);
  assert(strcmp(tokenArray[0], "+UNSTAT:") == 0);
  assert(strcmp(tokenArray[1], "7") == 0);
  assert(strcmp(tokenArray[2], "42") == 0);
  assert(strcmp(tokenArray[3], "\"abcd\"") == 0);
  assert(status == cbSTATUS_OK);
  memset(tokenArray, 0x00, sizeof(tokenArray));
  memset(buffer, 0x00, sizeof(buffer));
  numTokens = -1;

  strcpy(buffer, "+UNSTAT:7");
  status = cb_Tokenize(buffer, sizeof(buffer), ELEMENTS_OF(tokenArray),
                       tokenArray, &numTokens);
  assert(numTokens == 2);
  assert(strcmp(tokenArray[0], "+UNSTAT:") == 0);
  assert(strcmp(tokenArray[1], "7") == 0);
  assert(status == cbSTATUS_OK);
  memset(tokenArray, 0x00, sizeof(tokenArray));
  memset(buffer, 0x00, sizeof(buffer));
  numTokens = -1;

  strcpy(buffer, "+UNSTAT:\"abcd\"");
  status = cb_Tokenize(buffer, sizeof(buffer), ELEMENTS_OF(tokenArray),
                       tokenArray, &numTokens);
  assert(numTokens == 2);
  assert(strcmp(tokenArray[0], "+UNSTAT:") == 0);
  assert(strcmp(tokenArray[1], "\"abcd\"") == 0);
  assert(status == cbSTATUS_OK);
  memset(tokenArray, 0x00, sizeof(tokenArray));
  memset(buffer, 0x00, sizeof(buffer));
  numTokens = -1;

  strcpy(buffer, "AT+UNSTAT=7,42,\"abcd\"");  // Echo
  status = cb_Tokenize(buffer, sizeof(buffer), ELEMENTS_OF(tokenArray),
                       tokenArray, &numTokens);
  assert(numTokens == 1);
  assert(strcmp(tokenArray[0], "AT+UNSTAT=7,42,\"abcd\"") == 0);
  assert(status == cbSTATUS_OK);
  memset(tokenArray, 0x00, sizeof(tokenArray));
  memset(buffer, 0x00, sizeof(buffer));
  numTokens = -1;

  strcpy(buffer, "OK"); // Reponse
  cb_Tokenize(buffer, sizeof(buffer), ELEMENTS_OF(tokenArray),
              tokenArray, &numTokens);
  assert(numTokens == 1);
  assert(strcmp(tokenArray[0], "OK") == 0);
  assert(status == cbSTATUS_OK);
  memset(tokenArray, 0x00, sizeof(tokenArray));
  memset(buffer, 0x00, sizeof(buffer));
  numTokens = -1;

  strcpy(buffer, "ERROR");  // Response
  status = cb_Tokenize(buffer, sizeof(buffer), ELEMENTS_OF(tokenArray),
                       tokenArray, &numTokens);
  assert(numTokens == 1);
  assert(strcmp(tokenArray[0], "ERROR") == 0);
  assert(status == cbSTATUS_OK);
  memset(tokenArray, 0x00, sizeof(tokenArray));
  memset(buffer, 0x00, sizeof(buffer));
  numTokens = -1;

  strcpy(buffer, "\"u-blox\""); // Response to certain commands
  status = cb_Tokenize(buffer, sizeof(buffer), ELEMENTS_OF(tokenArray),
                       tokenArray, &numTokens);
  assert(numTokens == 1);
  assert(strcmp(tokenArray[0], "\"u-blox\"") == 0);
  assert(status == cbSTATUS_OK);
  memset(tokenArray, 0x00, sizeof(tokenArray));
  memset(buffer, 0x00, sizeof(buffer));
  numTokens = -1;

  strcpy(buffer, "0");  // Response to certain commands
  status = cb_Tokenize(buffer, sizeof(buffer), ELEMENTS_OF(tokenArray),
                       tokenArray, &numTokens);
  assert(numTokens == 1);
  assert(strcmp(tokenArray[0], "0") == 0);
  assert(status == cbSTATUS_OK);
  memset(tokenArray, 0x00, sizeof(tokenArray));
  memset(buffer, 0x00, sizeof(buffer));
  numTokens = -1;

  strcpy(buffer, "+STARTUP"); // Greeting
  status = cb_Tokenize(buffer, sizeof(buffer), ELEMENTS_OF(tokenArray),
                       tokenArray, &numTokens);
  assert(numTokens == 1);
  assert(strcmp(tokenArray[0], "+STARTUP") == 0);
  assert(status == cbSTATUS_OK);
  memset(tokenArray, 0x00, sizeof(tokenArray));
  memset(buffer, 0x00, sizeof(buffer));
  numTokens = -1;

  strcpy(buffer, "+XYZZY:"); // Not really a valid format, but could imply default values being transmitted for all fields. Will return 1 empty field.
  status = cb_Tokenize(buffer, sizeof(buffer), ELEMENTS_OF(tokenArray),
                       tokenArray, &numTokens);
  assert(numTokens == 2);
  assert(strcmp(tokenArray[0], "+XYZZY:") == 0);
  assert(strcmp(tokenArray[1], "") == 0);
  assert(status == cbSTATUS_OK);
  memset(tokenArray, 0x00, sizeof(tokenArray));
  memset(buffer, 0x00, sizeof(buffer));
  numTokens = -1;

  strcpy(buffer, "+++");  // Echo from Leave Data Mode while in AT Mode.
  status = cb_Tokenize(buffer, sizeof(buffer), ELEMENTS_OF(tokenArray),
                       tokenArray, &numTokens);
  assert(numTokens == 1);
  assert(strcmp(tokenArray[0], "+++") == 0);
  assert(status == cbSTATUS_ERROR);  // Is actually confusing the tokenizer a little, but we can live with that.
  memset(tokenArray, 0x00, sizeof(tokenArray));
  memset(buffer, 0x00, sizeof(buffer));
  numTokens = -1;

  strcpy(buffer, ""); // Empty string
  status = cb_Tokenize(buffer, sizeof(buffer), ELEMENTS_OF(tokenArray),
                       tokenArray, &numTokens);
  assert(numTokens == 1);
  assert(strcmp(tokenArray[0], "") == 0);
  assert(status == cbSTATUS_OK);
  memset(tokenArray, 0x00, sizeof(tokenArray));
  memset(buffer, 0x00, sizeof(buffer));
  numTokens = -1;

  strcpy(buffer, "A");
  status = cb_Tokenize(buffer, sizeof(buffer), ELEMENTS_OF(tokenArray),
                       tokenArray, &numTokens);
  assert(numTokens == 1);
  assert(strcmp(tokenArray[0], "A") == 0);
  assert(status == cbSTATUS_OK);
  memset(tokenArray, 0x00, sizeof(tokenArray));
  memset(buffer, 0x00, sizeof(buffer));
  numTokens = -1;

  strcpy(buffer, "+");  // Stray '+' character
  status = cb_Tokenize(buffer, sizeof(buffer), ELEMENTS_OF(tokenArray),
                       tokenArray, &numTokens);
  assert(numTokens == 1);
  assert(strcmp(tokenArray[0], "+") == 0);
  assert(status == cbSTATUS_ERROR);  // Is actually confusing the tokenizer a little, but we can live with that.
  memset(tokenArray, 0x00, sizeof(tokenArray));
  memset(buffer, 0x00, sizeof(buffer));
  numTokens = -1;

  strcpy(buffer, "+A");  // missing ':' on short command
  status = cb_Tokenize(buffer, sizeof(buffer), ELEMENTS_OF(tokenArray),
                       tokenArray, &numTokens);
  assert(numTokens == 1);
  assert(strcmp(tokenArray[0], "+A") == 0);
  assert(status == cbSTATUS_OK);
  memset(tokenArray, 0x00, sizeof(tokenArray));
  memset(buffer, 0x00, sizeof(buffer));
  numTokens = -1;

  strcpy(buffer, "+AB");  // missing ':' on short command
  status = cb_Tokenize(buffer, sizeof(buffer), ELEMENTS_OF(tokenArray),
                       tokenArray, &numTokens);
  assert(numTokens == 1);
  assert(strcmp(tokenArray[0], "+AB") == 0);
  assert(status == cbSTATUS_OK);
  memset(tokenArray, 0x00, sizeof(tokenArray));
  memset(buffer, 0x00, sizeof(buffer));
  numTokens = -1;

  strcpy(buffer, ":");  // Stray :-character
  status = cb_Tokenize(buffer, sizeof(buffer), ELEMENTS_OF(tokenArray),
                       tokenArray, &numTokens);
  assert(numTokens == 1);
  assert(strcmp(tokenArray[0], ":") == 0);
  assert(status == cbSTATUS_OK);
  memset(tokenArray, 0x00, sizeof(tokenArray));
  memset(buffer, 0x00, sizeof(buffer));
  numTokens = -1;

  strcpy(buffer, "Y:");  // missing '+' character on short command
  status = cb_Tokenize(buffer, sizeof(buffer), ELEMENTS_OF(tokenArray),
                       tokenArray, &numTokens);
  assert(numTokens == 1);
  assert(strcmp(tokenArray[0], "Y:") == 0);
  assert(status == cbSTATUS_OK);
  memset(tokenArray, 0x00, sizeof(tokenArray));
  memset(buffer, 0x00, sizeof(buffer));
  numTokens = -1;

  strcpy(buffer, "YZ:");  // missing '+' character on short command
  status = cb_Tokenize(buffer, sizeof(buffer), ELEMENTS_OF(tokenArray),
                       tokenArray, &numTokens);
  assert(numTokens == 1);
  assert(strcmp(tokenArray[0], "YZ:") == 0);
  assert(status == cbSTATUS_OK);
  memset(tokenArray, 0x00, sizeof(tokenArray));
  memset(buffer, 0x00, sizeof(buffer));
  numTokens = -1;

  printf("Tokenizer OK\n");
  return 0;
}
