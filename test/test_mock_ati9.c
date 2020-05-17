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
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "test_tools.h"

#include "ucxh_library.h"
#include "ucxh_serial.h"
#include "ucxh_serial_mock.h"

#include "ex_config.h"

/** 
 * \file test_mock_ati9.c
 * \test test_mock_ati9.c Unit-test: Hello, World / Issue AT command to mocked serial driver.
 * \brief Hello, World / Issue AT command to mocked serial driver.
 *  
## Introduction
 
This test is a basic "Hello, World",  which - using AT mode and mocked serial port - tests

 + Open the  serial port
 + Enter AT mode
 + Transmit an AT command
 + Receive the response
 + Close the serial port

Full source in `test/test_mock_ati9.c`

## Typical response (When compiled with \ref `UCXH_SERIAL_TRACE_MOCK 3`)

    Trying Enter AT mode...
    (mocked) ucxhSERIAL_write request to write 3 bytes, returning 3 since writeResult=4 and res=3: '+++'
    (mocked) ucxhSERIAL_write request to write 2 bytes, returning 2 since writeResult=4 and res=2: 'AT'
    (mocked) ucxhSERIAL_write request to write 2 bytes, returning 2 since writeResult=4 and res=2: '↵↓'
    (mocked) ucxhSERIAL_read: returning 10 bytes: '+STARTUP↵↓'
    test_mock_ati9.c/onUnknownURC: '+STARTUP'
    (mocked) ucxhSERIAL_read: returning 4 bytes: 'OK↵↓'
    Trying Handling stray responses
    (mocked) ucxhSERIAL_read: returning 8 bytes: 'AT↵↓OK↵↓'
    test_mock_ati9.c/onUnknownURC: 'AT'
    (mocked) ucxhSERIAL_read: returning 0
    Trying Talking to ATI9...
    (mocked) ucxhSERIAL_write request to write 4 bytes, returning 4 since writeResult=4 and res=4: 'ATI9'
    (mocked) ucxhSERIAL_write request to write 2 bytes, returning 2 since writeResult=4 and res=2: '↵↓'
    Trying Waiting for OK
    (mocked) ucxhSERIAL_read: Totally 26230623 invocations skipped
    (mocked) ucxhSERIAL_read: returning 57 bytes: 'ATI9↵↓"2.1.0-017","NINA-W13X-2.1.0-017-0-g253211b6"↵↓OK↵↓'
    test_mock_ati9.c/onUnknownURC: 'ATI9'
    test_mock_ati9.c/onI9: Application Version: '"2.1.0-017"' Unique Identifier: '"NINA-W13X-2.1.0-017-0-g253211b6"'


### Description

    All outputs not preceeded by (mocked) should basically match the output of test_ati9.

*/

int gotI9 = 0;
int onI9(ucxhPARSER_urcIdentificationInformationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    assert(gotI9 == 0);
    printf("test_mock_ati9.c/onI9: Application Version: '%s' Unique Identifier: '%s'\n", pEvent->application_version, pEvent->unique_identifier);

    gotI9 ++;
    return true;
}

static const char *readResults[] = {
  /* */               "+STARTUP\r\n",
  /* +++ */           "OK\r\n",
  /* AT  */           "AT\r\nOK\r\n",
};

static const char *readResultsI9[] = {
  /* ATI9  */           "ATI9\r\n\"2.1.0-017\",\"NINA-W13X-2.1.0-017-0-g253211b6\"\r\nOK\r\n",
};

int main()
{
  bool result = false;
  
  ucxhSERIAL_setReadResult(ELEMENTS_OF(readResults), readResults, 0);

  // To make sure the Windows serial port is closed in case of premature exit (aka assert())
  atexit(onExit);
  
  // Register Callbacks
  ucxhPARSER_setOnUnknownURCReceived(onUnknownURC);
  ucxhPARSER_setOnIdentificationInformation(onI9);

  result = ucxhSERIAL_open();
  assert(result == true);

  ucxhSERIAL_setWriteBuffer(7); // Just to test the mock
  
  printf("Trying Enter AT mode...\n");
  ucxhMODULE_enterAtMode();
  assert(ucxhSERIAL_wasStringWritten("+++"));
  assert(ucxhSERIAL_wasStringWritten("A"));
  // assert(ucxhSERIAL_wasStringWritten("X")); // Expect fail
  assert(ucxhSERIAL_wasStringWritten("T\r\n"));
  assert(ucxhSERIAL_wasStringWritten(""));

  printf("Trying Handling stray responses\n");
  ucxhPARSER_wait(1);
  
  ucxhSERIAL_setReadResult(ELEMENTS_OF(readResultsI9), readResultsI9, 0);

  printf("Trying Talking to ATI9...\n");
  assert(gotI9 == 0);
  ucxhPARSER_sendCommand("ATI9");
  assert(ucxhSERIAL_wasStringWritten("ATI9\r\n"));

  printf("Trying Waiting for OK\n");
  ucxhAT_response_t atResponse = ucxhPARSER_waitForOk(-1); // -1 implies the default AT timeout. Otherwize, specify the number of seconds to wait.
  assert(atResponse == ucxhAT_RESPONSE_OK);
  assert(gotI9 == 1);
      
  ucxhSERIAL_close();
  
  return 0;
}
