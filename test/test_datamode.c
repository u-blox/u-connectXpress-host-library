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
#include <unistd.h>

#include "test_tools.h"

#include "ucxh_library.h"
#include "ucxh_serial.h"

#include "ex_config.h"

/** 
 * \file test_datamode.c
 * \test test_datamode.c Manual test: Transmit over Data mode.
 * \brief Tests transmit over Data mode.
   
## Introduction

This example is a basic "Hello, World",  which - using AT mode - tests

 + Open the serial port
 + Enter AT mode
 + Transmit an AT command
 + Receive the response
 + Close the serial port

Full source in `test/test_datamode.c`

## Typical response

    Opening serial port...
    Entering AT mode...
    Handling stray responses
    test_tools.c/onUnknownURC: 'AT'
    Talking to NINA...
    test_tools.c/onUnknownURC: 'AT'
    test_tools.c/onUnknownURC: 'AT+GMI'
    test_tools.c/onURC: +GMI:"u-blox"
    test_tools.c/onUnknownURC: 'AT+GMM'
    test_tools.c/onURC: +GMM:"NINA-W132"
    test_tools.c/onUnknownURC: 'ATI9'
    test_tools.c/onURC: +I9:"2.1.0-017","NINA-W13X-2.1.0-017-0-g253211b6"
    test_tools.c/onUnknownURC: 'AT+GMR'
    test_tools.c/onURC: +GMR:"2.1.0-017"
    test_tools.c/onUnknownURC: 'AT+UMLA=2'
    test_tools.c/onURC: +UMLA:D4CA6EFD96E0
    Entering Data mode...
    test_tools.c/onUnknownURC: 'ATO1'
    Sending a data line in Data mode
    Expect no echo or response in data mode, so just wait for a while to verify
    Re-entering AT-mode...
    Talking to NINA again...
    test_tools.c/onUnknownURC: 'AT'
    test_tools.c/onUnknownURC: 'ATI9'
    test_tools.c/onURC: +I9:"2.1.0-017","NINA-W13X-2.1.0-017-0-g253211b6"



### Description

The lines

    Handling stray responses
    test_datamode.c/onUnknownURC: 'AT'

are used to flush the incoming serial buffer. In this case, they indicate 
that the host received something in AT mode which the u-connectXpress host library
don't know how to parse - in this case the echo of the `AT` command sent as part
of the sequence used to force the module into AT mode.

The example registers a callback, `onUnknownURC()`, which outputs whatever
the AT parser didn't handle.

Similar applies for the lines

    test_tools.c/onUnknownURC: 'AT'
    test_tools.c/onUnknownURC: 'AT+GMI'
    test_tools.c/onURC: +GMI:"u-blox"
    test_tools.c/onUnknownURC: 'AT+GMM'
    test_tools.c/onURC: +GMM:"NINA-W132"
    test_tools.c/onUnknownURC: 'ATI9'
    test_tools.c/onURC: +I9:"2.1.0-017","NINA-W13X-2.1.0-017-0-g253211b6"
    test_tools.c/onUnknownURC: 'AT+GMR'
    test_tools.c/onURC: +GMR:"2.1.0-017"
    test_tools.c/onUnknownURC: 'AT+UMLA=2'
    test_tools.c/onURC: +UMLA:D4CA6EFD96E0

with the exception that the lines states `test_tools.c/onURC:` indicates that 
the catch-all callback for ucxhPARSER_setOnURCParsed() has been invoked.

The u-connectXpress host library do not parse the echo from the module, nor
does it know that the transmitted commands are  expected to return
strings or URC's containing information.

The lines 

    Entering Data mode...
    test_tools.c/onUnknownURC: 'ATO1'

indicates that the module is forced into Data mode.

    Sending a data line in Data mode
    Expect no echo or response in data mode, so just wait for a while to verify

These lines indicates that some data is transmitted in data mode. Data mode does not echo any 
data, so nothing but the following line should arrive:

    Re-entering AT-mode...
    Talking to NINA again...

Now the module is back into AT mode, and a couple of commands are transmitted using AT mode.
Just as above, the module echos the responses back, which aren't handled by the AT 
parser library, hence the callback is triggered:

    test_tools.c/onUnknownURC: 'AT'
    test_tools.c/onUnknownURC: 'ATI9'
    test_tools.c/onURC: +I9:"2.1.0-017","NINA-W13X-2.1.0-017-0-g253211b6"

    
*/

static const char *atList[] = {
  "AT",
  "AT+GMI",
  "AT+GMM",
  "ATI9",
  "AT+GMR",
  "AT+UMLA=2"
};

static const char *atList2[] = {
  "ATI9"
};



int main()
{
  bool result = false;
  
  // To make sure the Windows serial port is closed in case of premature exit (aka assert())
  atexit(onExit);
  

  // Register callbacks
  ucxhPARSER_setOnDataReceived(onData);
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
  ucxhPARSER_sendAtCommands(atList, ELEMENTS_OF(atList));
  ucxhPARSER_wait(1);
  
  printf("Entering Data mode...\n");
  result = ucxhMODULE_enterDataMode();
  assert(result == true);

  ucxhPARSER_wait(1);

  printf("Sending a data line in Data mode\n");
  const char *str = "Hello, World!\n";
  size_t strLength = strlen(str);
  ucxhDATA_sendData(str, strLength);
  
  printf("Expect no echo or response in data mode, so just wait for a while to verify\n");
  ucxhPARSER_wait(2);
    
  printf("Re-entering AT-mode...\n");
  result = ucxhMODULE_enterAtMode();
  assert(result == true);
  
  printf("Talking to NINA again...\n");
  ucxhPARSER_sendAtCommands(atList2, ELEMENTS_OF(atList2));
  ucxhPARSER_wait(1);
  
  ucxhSERIAL_close();
  
  return 0;
}
