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
 * \file test_edm_at_read.c
 * \test test_edm_at_read.c Manual test: Using EDM, read status from the module.
 * \brief Tests, using EDM, read status from the module.
 
## Introduction
 
This is a test which - using AT commands over EDM and data over EDM channels - tests

 + Enter Extended Data Mode (EDM)
 + Read a lot of information from the module

The EDM packet parser invokes the same AT parser as used in the \ref test_ati9 , \ref test_datamode tests.

Full source in `test/test_edm_at_read.c`

## Typical response

    Opening serial port...
    Entering AT mode...
    Handling stray responses
    test_tools.c/onUnknownURC: 'AT'
    Talking to NINA...
    Waiting for OK
    test_tools.c/onUnknownURC: 'ATI9'
    test_tools.c/onURC: +I9:"2.1.0-017","NINA-W13X-2.1.0-017-0-g253211b6"
    Entering extended data mode (EDM)...
    test_tools.c/onUnknownURC: 'ATO2'
    Reading EDM for 2s, expect nothing...
    Query NINA...
    Reading for 10s...
    test_tools.c/onURC: +GMI:"u-blox"
    test_tools.c/onURC: +GMM:"NINA-W132"
    test_tools.c/onURC: +I9:"2.1.0-017","NINA-W13X-2.1.0-017-0-g253211b6"
    test_tools.c/onURC: +GMR:"2.1.0-017"
    test_tools.c/onURC: +UMLA:D4CA6EFD96E0
    test_tools.c/onURC: +UMLA:D4CA6EFD96E1
    test_tools.c/onURC: +UMLA:D4CA6EFD96E3
    test_tools.c/onURC: +GSN:"0233965910595296"
    test_tools.c/onUnknownURC: '"u-blox"'
    test_tools.c/onUnknownURC: '"NINA-W132"'
    test_tools.c/onUnknownURC: '"2.1.0-017"'
    test_tools.c/onUnknownURC: '"0233965910595296"'
    test_tools.c/onURC: +I0:"NINA-W152-00B-00"
    test_tools.c/onURC: +I10:"000000000000246F28119B14"
    test_tools.c/onUnknownURC: '+CSGT:1,"+STARTUP"'
    test_tools.c/onUnknownURC: '+UMSM:0'
    test_tools.c/onUnknownURC: '+UMSTAT:0,149'
    test_tools.c/onUnknownURC: '+UMSTAT:1,1'
    test_tools.c/onUnknownURC: '+UMRS:115200,1,8,1,1'
    Restarting module
    Test completed


 The line

     Entering extended data mode (EDM)...
     test_tools.c/onUnknownURC: 'ATO2'

  Indicates the module is forced into EDM mode. The following line are used to flush
  the EDM serial buffer, by reading for a short wile:

    Reading EDM for 2s, expect nothing...

  Then, a number of information is requested, but since the AT parser don't know how to
  handle those yet, or a callback for the result haven't been registered,
  they are handled by the catch-all onUnknownURC() or onURC() callbacks registered by this test:

    Query NINA...
    Reading for 10s...
    test_tools.c/onURC: +GMI:"u-blox"
    test_tools.c/onURC: +GMM:"NINA-W132"
    test_tools.c/onURC: +I9:"2.1.0-017","NINA-W13X-2.1.0-017-0-g253211b6"
    test_tools.c/onURC: +GMR:"2.1.0-017"
    test_tools.c/onURC: +UMLA:D4CA6EFD96E0
    test_tools.c/onURC: +UMLA:D4CA6EFD96E1
    test_tools.c/onURC: +UMLA:D4CA6EFD96E3
    test_tools.c/onURC: +GSN:"0233965910595296"
    test_tools.c/onUnknownURC: '"u-blox"'
    test_tools.c/onUnknownURC: '"NINA-W132"'
    test_tools.c/onUnknownURC: '"2.1.0-017"'
    test_tools.c/onUnknownURC: '"0233965910595296"'
    test_tools.c/onURC: +I0:"NINA-W152-00B-00"
    test_tools.c/onURC: +I10:"000000000000246F28119B14"
    test_tools.c/onUnknownURC: '+CSGT:1,"+STARTUP"'
    test_tools.c/onUnknownURC: '+UMSM:0'
    test_tools.c/onUnknownURC: '+UMSTAT:0,149'
    test_tools.c/onUnknownURC: '+UMSTAT:1,1'
    test_tools.c/onUnknownURC: '+UMRS:115200,1,8,1,1'

*/

static const char *atStartupList[] = {
  "AT\r",
  "AT+GMI\r",
  "AT+GMM\r",
  "ATI9\r",
  "AT+GMR\r",
  "AT+UMLA=1\r",
  "AT+UMLA=2\r",
  "AT+UMLA=3\r",
  "AT+UMLA=4\r",

  "AT+GSN\r",
  "AT+CGMI\r",
  "AT+CGMM\r",
  "AT+CGMR\r",
  "AT+CGSN\r",
  "ATI0\r",
  "ATI10\r",

  "AT+CSGT?\r",

  "AT+UMSM?\r",
  "AT+UMSTAT=0\r",
  "AT+UMSTAT=1\r",
  "AT+UMRS?\r",
};



int onStartup(void) {
  printf("test_edm_at_read.c/onStartup: Startup event\n");

  return 0;
}



int main()
{
  bool result = false;
  
  // To make sure the Windows serial port is closed in case of premature exit (aka assert())
  atexit(onExit);
  
  // Register callbacks
  ucxhEDM_setOnStartupReceived(onStartup);
  ucxhPARSER_setOnURCParsed(onURC);
  ucxhPARSER_setOnUnknownURCReceived(onUnknownURC);
  
  printf("Opening serial port...\n");  
  result = ucxhSERIAL_open();
  assert(result == true);

  printf("Entering AT mode...\n");
  result = ucxhMODULE_enterAtMode();
  assert(result == true);

  printf("Handling stray responses\n");
  ucxhPARSER_wait(1);
  
  printf("Talking to NINA...\n");
  ucxhPARSER_sendCommand("ATI9\r");

  printf("Waiting for OK\n");
  ucxhAT_response_t atResponse = ucxhPARSER_waitForOk(-1);
  assert(atResponse == ucxhAT_RESPONSE_OK); // If failed, NINA is probably in Extended Data Mode.

  printf("Entering extended data mode (EDM)...\n");
  result = ucxhMODULE_enterExtendedDataMode();
  assert(result == true);
  
  printf("Reading EDM for 2s, expect nothing...\n");
  ucxhPARSER_wait(2);

  printf("Query NINA...\n");
  ucxhPARSER_sendAtCommands(atStartupList, ELEMENTS_OF(atStartupList));
  
  printf("Reading for 10s...\n");
  ucxhPARSER_wait(10);


  printf("Restarting module\n");
  ucxhMODULE_restart();

  printf("Test completed\n");
  ucxhSERIAL_close();

  return 0;
}
