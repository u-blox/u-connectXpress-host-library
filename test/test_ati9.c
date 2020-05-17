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

#include "ex_config.h"

/** 
 * \file test_ati9.c
 * \test test_ati9.c Manual test: Hello, world.
 * \brief Tests Hello, World / Issue AT command.
 *  
## Introduction
 
This test is a basic "Hello, World",  which - using AT mode - tests

 + Open the serial port
 + Enter AT mode
 + Transmit an AT command
 + Receive the response
 + Close the serial port

Full source in `test/test_ati9.c`

## Typical response

    Opening serial port...
    Entering AT mode...
    test_ati9.c/onStartup
    Handling stray responses
    test_ati9.c/onUnknownURC: 'AT'
    Talking to NINA...
    Waiting for OK
    test_ati9.c/onUnknownURC: 'ATI9'
    test_ati9.c/onI9: Application Version: '"2.1.0-017"' Unique Identifier: '"NINA-W13X-2.1.0-017-0-g253211b6"'
    test_ati9 completed OK!

### Description

The line

    test_ati9.c/onStartup

may or may not appear, depending on if the module was reset prior to the execution of this test, or not.

The lines

    Handling stray responses
    test_ati9.c/onUnknownURC: 'AT'

indicates that the host received something in AT mode which the u-connectXpress host library
don't know how to parse - in this case the echo of the `AT` command sent as part
of the sequence used to force the module into AT mode.

The test registers a callback, `onUnknownURC()`, which outputs whatever
the AT parser didn't handle.

Similar applies for the line

    test_ati9.c/onUnknownURC: 'ATI9'

The u-connectXpress host library do not parse the echo `ATI9` echo from the module.

Finally, the event which  contains the response from the module to the ATI9 command: Its software version:

    test_ati9.c/onI9: Application Version: '"2.1.0-017"' Unique Identifier: '"NINA-W13X-2.1.0-017-0-g253211b6"'

*/


int gotI9 = 0;
int onI9(ucxhPARSER_urcIdentificationInformationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    assert(gotI9 == 0);
    printf("test_ati9.c/onI9: Application Version: '%s' Unique Identifier: '%s'\n", pEvent->application_version, pEvent->unique_identifier);

    gotI9 ++;
    return true;
}

int onStartup(ucxhPARSER_urcStartupEvent_t *pEvent)
{
    printf("test_ati9.c/onStartup\n");

    return true;
}
int main()
{
  bool result = false;
  
  // To make sure the Windows serial port is closed in case of premature exit (aka assert())
  atexit(onExit);
  
  // Register Callbacks
  ucxhPARSER_setOnUnknownURCReceived(onUnknownURC);
  ucxhPARSER_setOnIdentificationInformation(onI9);
  ucxhPARSER_setOnStartup(onStartup);

  printf("Opening serial port...\n");  
  result = ucxhSERIAL_open();
  assert(result == true);

  printf("Entering AT mode...\n");
  ucxhMODULE_enterAtMode();

  printf("Handling stray responses\n");
  ucxhPARSER_wait(1);
  
  printf("Talking to NINA...\n");
  assert(gotI9 == 0);
  ucxhPARSER_sendCommand("ATI9");

  printf("Waiting for OK\n");
  ucxhAT_response_t atResponse = ucxhPARSER_waitForOk(-1); // -1 implies the default AT timeout. Otherwize, specify the number of seconds to wait.
  assert(atResponse == ucxhAT_RESPONSE_OK); // If failed, NINA is probably in Extended Data Mode.
  assert(gotI9 == 1); // Verify we've gotten the callback
    

  printf("test_ati9 completed OK!\n");
  ucxhSERIAL_close();
  
  return 0;
}
