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
 * \file test_getsystem.c
 * \test test_getsystem.c Unit test: AT-commands UMLA .
 * \brief Tests from chapter 4: AT-commands +UMLA .
 *  
## Introduction
 
This mocked test tests parsing of URCs for

 + UMLA

See test_getsystem

Full source in `test/test_getsystem.c`

## Typical response

### Description

*/



#define ATPLUS_UMLA1   "AT+UMLA=1\r\n"
#define ATPLUS_UMLA2   "AT+UMLA=2\r\n"
#define ATPLUS_UMLA3   "AT+UMLA=3\r\n"
#define ATPLUS_UMLA4   "AT+UMLA=4\r\n"


#define PLUS_UMLA1_RESPONSE  "D4CA6E889F81"
#define PLUS_UMLA2_RESPONSE  "D4CA6E889F82"
#define PLUS_UMLA3_RESPONSE  "D4CA6E889F83"
#define PLUS_UMLA4_RESPONSE  "D4CA6E889F84"

static const char *readResultsPlusUMLA1[] = {
  /* AT+UMLA=1  */    ATPLUS_UMLA1 "+UMLA:" PLUS_UMLA1_RESPONSE EOL OK,
};

static const char *readResultsPlusUMLA2[] = {
  /* AT+UMLA=2  */    ATPLUS_UMLA2 "+UMLA:" PLUS_UMLA2_RESPONSE EOL OK,
};

static const char *readResultsPlusUMLA3[] = {
  /* AT+UMLA=3  */    ATPLUS_UMLA3 "+UMLA:" PLUS_UMLA3_RESPONSE EOL OK,
};

static const char *readResultsPlusUMLA4[] = {
  /* AT+UMLA=4  */    ATPLUS_UMLA4 "+UMLA:" PLUS_UMLA4_RESPONSE EOL OK,
};






int gotPlusUMLA1 = 0;
int gotPlusUMLA2 = 0;
int gotPlusUMLA3 = 0;
int gotPlusUMLA4 = 0;
int onPlusUMLA(ucxhPARSER_urcLocalAddressEvent_t *pEvent)
{
    assert(pEvent != NULL);
    const char *hardware_interface_name ="UNKNOWN";

    switch (pEvent->interface_id) {
      case ucxhPARSER_HARDWARE_INTERFACE_BLUETOOTH:     assert(gotPlusUMLA1 == 0); assert(strcmp(pEvent->mac_address, PLUS_UMLA1_RESPONSE) == 0); gotPlusUMLA1 ++; hardware_interface_name = "Bluetooth"; break;
      case ucxhPARSER_HARDWARE_INTERFACE_WIFI_STATION:  assert(gotPlusUMLA2 == 0); assert(strcmp(pEvent->mac_address, PLUS_UMLA2_RESPONSE) == 0); gotPlusUMLA2 ++; hardware_interface_name = "WiFi STA"; break;
      case ucxhPARSER_HARDWARE_INTERFACE_ETHERNET:      assert(gotPlusUMLA3 == 0); assert(strcmp(pEvent->mac_address, PLUS_UMLA3_RESPONSE) == 0); gotPlusUMLA3 ++; hardware_interface_name = "Ethernet"; break;
      case ucxhPARSER_HARDWARE_INTERFACE_WIFI_AP:       assert(gotPlusUMLA4 == 0); assert(strcmp(pEvent->mac_address, PLUS_UMLA4_RESPONSE) == 0); gotPlusUMLA4 ++; hardware_interface_name = "WiFI AP"; break;
      default:
      assert(0);
    }

    printf("test_getsystem.c/onUMLA: Interface_id: %d (%s) MAC Address '%s'\n", pEvent->interface_id, hardware_interface_name, pEvent->mac_address);

    return true;
}


int main()
{
  bool result = false;
  ucxhAT_response_t atResponse = ucxhAT_RESPONSE_OK;
  
  // To make sure the Windows serial port is closed in case of premature exit (aka assert())
  atexit(onExit);
  
  // Register Callbacks
  ucxhPARSER_setOnUnknownURCReceived(onUnknownURC);
  ucxhPARSER_setOnLocalAddress(onPlusUMLA);

  result = ucxhSERIAL_open();
  assert(result == true);

  //
  //
  //


  // UMLA=1

  printf("Trying AT+UMLA=1...\n");
  assert(gotPlusUMLA1 == 0);
  ucxhPARSER_sendCommand(ATPLUS_UMLA1);
  THEN_READ(readResultsPlusUMLA1);
  assert(ucxhSERIAL_wasStringWritten(ATPLUS_UMLA1));

  printf("Trying Waiting for OK\n");
  atResponse = ucxhPARSER_waitForOk(-1); // -1 implies the default AT timeout. Otherwize, specify the number of seconds to wait.
  assert(atResponse == ucxhAT_RESPONSE_OK);
  assert(gotPlusUMLA1 == 1);
  printf("AT+UMLA=1 OK!\n");


  // UMLA=2

  printf("Trying AT+UMLA=2...\n");
  assert(gotPlusUMLA2 == 0);
  ucxhPARSER_sendCommand(ATPLUS_UMLA2);
  THEN_READ(readResultsPlusUMLA2);
  assert(ucxhSERIAL_wasStringWritten(ATPLUS_UMLA2));

  printf("Trying Waiting for OK\n");
  atResponse = ucxhPARSER_waitForOk(-1); // -1 implies the default AT timeout. Otherwize, specify the number of seconds to wait.
  assert(atResponse == ucxhAT_RESPONSE_OK);
  assert(gotPlusUMLA2 == 1);
  printf("AT+UMLA=2 OK!\n");

  // UMLA=3

  printf("Trying AT+UMLA=3...\n");
  assert(gotPlusUMLA3 == 0);
  ucxhPARSER_sendCommand(ATPLUS_UMLA3);
  THEN_READ(readResultsPlusUMLA3);
  assert(ucxhSERIAL_wasStringWritten(ATPLUS_UMLA3));

  printf("Trying Waiting for OK\n");
  atResponse = ucxhPARSER_waitForOk(-1); // -1 implies the default AT timeout. Otherwize, specify the number of seconds to wait.
  assert(atResponse == ucxhAT_RESPONSE_OK);
  assert(gotPlusUMLA3 == 1);
  printf("AT+UMLA=3 OK!\n");

  // UMLA=4

  printf("Trying AT+UMLA=4...\n");
  assert(gotPlusUMLA4 == 0);
  ucxhPARSER_sendCommand(ATPLUS_UMLA4);
  THEN_READ(readResultsPlusUMLA4);
  assert(ucxhSERIAL_wasStringWritten(ATPLUS_UMLA4));

  printf("Trying Waiting for OK\n");
  atResponse = ucxhPARSER_waitForOk(-1); // -1 implies the default AT timeout. Otherwize, specify the number of seconds to wait.
  assert(atResponse == ucxhAT_RESPONSE_OK);
  assert(gotPlusUMLA4 == 1);
  printf("AT+UMLA=4 OK!\n");

  assert(gotPlusUMLA1 == 1);
  assert(gotPlusUMLA2 == 1);
  assert(gotPlusUMLA3 == 1);
  assert(gotPlusUMLA4 == 1);

  gotPlusUMLA1 = 0;
  gotPlusUMLA2 = 0;
  gotPlusUMLA3 = 0;
  gotPlusUMLA4 = 0;

  // UMLA=1

  printf("Trying ucxhCOMMAND_readLocalAddress(ucxhPARSER_HARDWARE_INTERFACE_BLUETOOTH)...\n");
  assert(gotPlusUMLA1 == 0);
  ucxhCOMMAND_readLocalAddress(ucxhPARSER_HARDWARE_INTERFACE_BLUETOOTH);
  THEN_READ(readResultsPlusUMLA1);
  assert(ucxhSERIAL_wasStringWritten(ATPLUS_UMLA1));

  printf("Trying Waiting for OK\n");
  atResponse = ucxhPARSER_waitForOk(-1); // -1 implies the default AT timeout. Otherwize, specify the number of seconds to wait.
  assert(atResponse == ucxhAT_RESPONSE_OK);
  assert(gotPlusUMLA1 == 1);
  printf("ucxhCOMMAND_readLocalAddress(ucxhPARSER_HARDWARE_INTERFACE_BLUETOOTH) OK!\n");

  // UMLA=2

  printf("Trying ucxhCOMMAND_readLocalAddress(ucxhPARSER_HARDWARE_INTERFACE_WIFI_STATION)...\n");
  assert(gotPlusUMLA2 == 0);
  ucxhCOMMAND_readLocalAddress(ucxhPARSER_HARDWARE_INTERFACE_WIFI_STATION);
  THEN_READ(readResultsPlusUMLA2);
  assert(ucxhSERIAL_wasStringWritten(ATPLUS_UMLA2));

  printf("Trying Waiting for OK\n");
  atResponse = ucxhPARSER_waitForOk(-1); // -1 implies the default AT timeout. Otherwize, specify the number of seconds to wait.
  assert(atResponse == ucxhAT_RESPONSE_OK);
  assert(gotPlusUMLA2 == 1);
  printf("ucxhCOMMAND_readLocalAddress(ucxhPARSER_HARDWARE_INTERFACE_WIFI_STATION) OK!\n");

  // UMLA=3

  printf("Trying ucxhCOMMAND_readLocalAddress(ucxhPARSER_HARDWARE_INTERFACE_ETHERNET)...\n");
  assert(gotPlusUMLA3 == 0);
  ucxhCOMMAND_readLocalAddress(ucxhPARSER_HARDWARE_INTERFACE_ETHERNET);
  THEN_READ(readResultsPlusUMLA3);
  assert(ucxhSERIAL_wasStringWritten(ATPLUS_UMLA3));

  printf("Trying Waiting for OK\n");
  atResponse = ucxhPARSER_waitForOk(-1); // -1 implies the default AT timeout. Otherwize, specify the number of seconds to wait.
  assert(atResponse == ucxhAT_RESPONSE_OK);
  assert(gotPlusUMLA3 == 1);
  printf("ucxhCOMMAND_readLocalAddress(ucxhPARSER_HARDWARE_INTERFACE_ETHERNET) OK!\n");

  // UMLA=4

  printf("Trying ucxhCOMMAND_readLocalAddress(ucxhPARSER_HARDWARE_INTERFACE_WIFI_AP)...\n");
  assert(gotPlusUMLA4 == 0);
  ucxhCOMMAND_readLocalAddress(ucxhPARSER_HARDWARE_INTERFACE_WIFI_AP);
  THEN_READ(readResultsPlusUMLA4);
  assert(ucxhSERIAL_wasStringWritten(ATPLUS_UMLA4));

  printf("Trying Waiting for OK\n");
  atResponse = ucxhPARSER_waitForOk(-1); // -1 implies the default AT timeout. Otherwize, specify the number of seconds to wait.
  assert(atResponse == ucxhAT_RESPONSE_OK);
  assert(gotPlusUMLA4 == 1);
  printf("ucxhCOMMAND_readLocalAddress(ucxhPARSER_HARDWARE_INTERFACE_WIFI_AP) OK!\n");

  assert(gotPlusUMLA1 == 1);
  assert(gotPlusUMLA2 == 1);
  assert(gotPlusUMLA3 == 1);
  assert(gotPlusUMLA4 == 1);


  printf("test_system completed OK!\n");

  ucxhSERIAL_close();
  
  return 0;
}
