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

#include "ucxh_library.h"
#include "ucxh_serial.h"

#include "ex_config.h"

/** 
 * \file test_edm_at.c
 * \test test_edm_at.c Manual test: Hello, World / Issue AT command using EDM.
 * \brief Hello, World / Issue AT command using EDM.

## Introduction

This test is a basic Hello, World, which - using EDM - tests

 + Open the serial port
 + Enter EDM mode
 + Transmit an AT command
 + Receive the response
 + Close the serial port

The EDM packet parser invokes the same AT parser as used in the \ref test_ati9 , \ref test_datamode tests.

Full source in `test/test_edm_at.c`

## Typical response

    Opening serial port...
    Entering AT mode...
    Handling stray responses
    test_tools.c/onUnknownURC: 'AT'
    Talking to NINA...
    Waiting for OK
    test_tools.c/onUnknownURC: 'ATI9'
    test_edm_at.c/onI9: Application Version: '2.1.0-017' Unique Identifier: 'NINA-W13X-2.1.0-017-0-g253211b6'
    Entering extended data mode (EDM)...
    test_tools.c/onUnknownURC: 'ATO2'
    Reading EDM for 1s, expect nothing...
    Requesting ATI9 over EDM...
    Reading EDM for 1s, expect the ATI9 response...
    test_edm_at.c/onI9: Application Version: '2.1.0-017' Unique Identifier: 'NINA-W13X-2.1.0-017-0-g253211b6'
    Restarting module
    Test completed




The test enters EDM. Once entered, the test simply reads for a while
to verify that no data is incoming, so nothing is supposed to be printed

    Entering extended data mode (EDM)...
    test_edm_at.c/onUnknownURC: 'ATO2'
    Reading EDM for 1s, expect nothing...

An AT command is transmitted using EDM and wait for the response

    Requesting ATI9 over EDM...
    Reading EDM for 1s, expect the ATI9 response...
    test_edm_at.c/onUnknownURC: '"2.1.0-017","NINA-W13X-2.1.0-017-0-g253211b6"'

Finally, the test restart the module to ensure the module can be used in other tests.

    Restarting module
    Test completed
*/
int onI9(ucxhPARSER_urcIdentificationInformationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    printf("test_edm_at.c/onI9: Application Version: '%s' Unique Identifier: '%s'\n", pEvent->application_version, pEvent->unique_identifier);

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

  printf("Opening serial port...\n");  
  result = ucxhSERIAL_open();
  assert(result == true);

  printf("Entering AT mode...\n");
  result = ucxhMODULE_enterAtMode();
  assert(result == true);  // If failed, NINA is probably in Extended Data Mode.

  printf("Handling stray responses\n");
  ucxhPARSER_wait(1);
  
  printf("Talking to NINA...\n");
  ucxhPARSER_sendCommand("ATI9");

  printf("Waiting for OK\n");
  ucxhAT_response_t atResponse = ucxhPARSER_waitForOk(1);
  assert(atResponse == ucxhAT_RESPONSE_OK); // If failed, NINA is probably in Extended Data Mode.

  printf("Entering extended data mode (EDM)...\n");
  result = ucxhMODULE_enterExtendedDataMode();
  assert(result == true);

  printf("Reading EDM for 1s, expect nothing...\n");
  ucxhPARSER_wait(1);

  printf("Requesting ATI9 over EDM...\n");
  ucxhPARSER_sendCommand("ATI9");

  printf("Reading EDM for 1s, expect the ATI9 response...\n");
  ucxhPARSER_wait(1);

  // It is not possible to leave EDM without restarting

  printf("Restarting module\n");
  ucxhMODULE_restart();

  printf("Test completed\n");
  ucxhSERIAL_close();
  
  return 0;
}
