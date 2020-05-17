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

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ucxh_library.h"
#include "ucxh_serial.h"
#ifdef CONFIG_USE_MOCKED_SERIAL
#include "ucxh_serial_mock.h"
#endif

#include "ex_config.h"

#include "test_tools.h"

#ifdef CONFIG_USE_MOCKED_SERIAL

void returnError(void) 
{
  const char *error[] = {
    "ERROR\r\n"
  };
  ucxhSERIAL_setReadResult(ELEMENTS_OF(error), error, 0);
  ucxhPARSER_wait(1);
}

void returnOk(void) 
{
  const char *ok[] = {
    "OK\r\n"
  };
  ucxhSERIAL_setReadResult(ELEMENTS_OF(ok), ok, 0);
  ucxhPARSER_wait(1);
}

void return2Ok(void) 
{
  const char *ok[] = {
    "OK\r\n",
    "OK\r\n",
  };
  ucxhSERIAL_setReadResult(ELEMENTS_OF(ok), ok, 0);
  ucxhPARSER_wait(1);
}

void return3Ok(void) 
{
  const char *ok[] = {
    "OK\r\n",
    "OK\r\n",
    "OK\r\n",
  };
  ucxhSERIAL_setReadResult(ELEMENTS_OF(ok), ok, 0);
  ucxhPARSER_wait(1);
}
#endif

void onExit(void) {
  ucxhSERIAL_close();
}

/** Act on incoming URCs, echo's and other stuff that weren't understood by the AT parser. */
int onUnknownURC(const char *urc)
{
  if(strlen(urc) > 0) {
    // Remember that URCs that are understood by the AT parser will not 
    // invoke this callback. Instead, they invoke the callback registered by 
    // ucxhPARSER_setOnURCParsed()

    printf("test_tools.c/onUnknownURC: '%s'\n", urc);
  }
}

/** Act on incoming data in data mode. */
int onData(int channel, const uint8_t *data, size_t length)
{
  assert(data != NULL);
  assert(channel == -1); // -1 when using datamode, otherwize EDM channel

  printf("test_tools.c/onData: Channel %d: Data event of %d bytes: '", channel, length);
  ucxhPARSER_printBuffer(data, length);
  printf("'\n");

  //... Handle the incoming data here...  

  return 0;
}

/** Act on incoming URCs */
int onURC(uint8_t numParams, const char *ppParams[])
{
  assert(ppParams != NULL);

  printf("test_tools.c/onURC: %s", ppParams[0]);
  for (int i = 1; i < numParams; i++) {
     printf("%s%s", (i == 1 ? "" : ","), ppParams[i]);
  }
  printf("\n");


  //... Act on incoming URCs

  // Remember that these are already handled automatically by the AT parser, most specifically,
  // +UUNU: automatically triggers AT+UNSTAT, and the response from AT+UNSTAT is used by the parser
  // to determine if the station has received all IP addresses from the AP.
  // URCs not understood by the AT parser, don't invoke this callback. Instead, they invoke
  // the callback registered by ucxhPARSER_setOnUnknownURCReceived

  return 0;
}


