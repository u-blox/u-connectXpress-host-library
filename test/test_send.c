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
#include <limits.h>
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
 * \file test_send.c
 * \test test_send.c Unit test: Functions used for building and sending AT commands.
 * \brief Tests the functions used for building and sending AT commands.
 *  
## Introduction
 
## Typical response

### Description


*/

int main()
{
  bool result = ucxhSERIAL_open();
  assert(result == true);

  const char *OK_RESPONSES[] = {
    "OK\r\n"
  };

  ucxhSERIAL_setReadResult(ELEMENTS_OF(OK_RESPONSES), OK_RESPONSES, 1); // Make sure every command returns "OK"

  // AT and DATA mode transmission

  ucxhAT_sendCommand("AT+TEST=1");
  assert(ucxhSERIAL_wasStringWritten("AT+TEST=1\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  ucxhAT_sendCommand("AT+TEST=2\r");
  assert(ucxhSERIAL_wasStringWritten("AT+TEST=2\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  ucxhAT_sendCommand("AT+TEST=3\r\n");
  assert(ucxhSERIAL_wasStringWritten("AT+TEST=3\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  ucxhAT_sendCommand("AT+TEST=4\n");
  assert(ucxhSERIAL_wasStringWritten("AT+TEST=4\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  printf("ucxhAT_sendCommand() tested OK\n");
  
  ucxhPARSER_sendCommand("AT+TEST=1");
  assert(ucxhSERIAL_wasStringWritten("AT+TEST=1\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);
  assert(strcmp(ucxhPARSER_getLastCommand(), "+TEST") == 0);
  assert(strcmp(ucxhPARSER_getLastCommandParam(), "1") == 0);

  ucxhPARSER_sendCommand("AT+TEST=2\r");
  assert(ucxhSERIAL_wasStringWritten("AT+TEST=2\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);
  assert(strcmp(ucxhPARSER_getLastCommand(), "+TEST") == 0);
  assert(strcmp(ucxhPARSER_getLastCommandParam(), "2") == 0);

  ucxhPARSER_sendCommand("AT+TEST=3\r\n");
  assert(ucxhSERIAL_wasStringWritten("AT+TEST=3\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);
  assert(strcmp(ucxhPARSER_getLastCommand(), "+TEST") == 0);
  assert(strcmp(ucxhPARSER_getLastCommandParam(), "3") == 0);

  ucxhPARSER_sendCommand("AT+TEST=4\n");
  assert(ucxhSERIAL_wasStringWritten("AT+TEST=4\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);
  assert(strcmp(ucxhPARSER_getLastCommand(), "+TEST") == 0);
  assert(strcmp(ucxhPARSER_getLastCommandParam(), "4") == 0);
  
  ucxhPARSER_sendCommand("AT+DUMMY=5\n");
  assert(ucxhSERIAL_wasStringWritten("AT+DUMMY=5\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);
  assert(strcmp(ucxhPARSER_getLastCommand(), "+DUMMY") == 0);
  assert(strcmp(ucxhPARSER_getLastCommandParam(), "5") == 0);

  ucxhPARSER_sendCommand("AT+TEST=1A");
  assert(ucxhSERIAL_wasStringWritten("AT+TEST=1A\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);
  assert(strcmp(ucxhPARSER_getLastCommand(), "+TEST") == 0);
  assert(strcmp(ucxhPARSER_getLastCommandParam(), "") == 0);

  ucxhPARSER_sendCommand("AT+TEST=A1");
  assert(ucxhSERIAL_wasStringWritten("AT+TEST=A1\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);
  assert(strcmp(ucxhPARSER_getLastCommand(), "+TEST") == 0);
  assert(strcmp(ucxhPARSER_getLastCommandParam(), "") == 0);

  ucxhPARSER_sendCommand("AT+TEST?");
  assert(ucxhSERIAL_wasStringWritten("AT+TEST?\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);
  assert(strcmp(ucxhPARSER_getLastCommand(), "+TEST") == 0);
  assert(strcmp(ucxhPARSER_getLastCommandParam(), "") == 0);

  ucxhPARSER_sendCommand("AT+TEST=\"1\"");
  assert(ucxhSERIAL_wasStringWritten("AT+TEST=\"1\"\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);
  assert(strcmp(ucxhPARSER_getLastCommand(), "+TEST") == 0);
  assert(strcmp(ucxhPARSER_getLastCommandParam(), "") == 0);

  ucxhPARSER_sendCommand("AT+TEST");
  assert(ucxhSERIAL_wasStringWritten("AT+TEST\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);
  assert(strcmp(ucxhPARSER_getLastCommand(), "+TEST") == 0);
  assert(strcmp(ucxhPARSER_getLastCommandParam(), "") == 0);

  ucxhPARSER_sendCommand("AT+TEST\r");
  assert(ucxhSERIAL_wasStringWritten("AT+TEST\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);
  assert(strcmp(ucxhPARSER_getLastCommand(), "+TEST") == 0);
  assert(strcmp(ucxhPARSER_getLastCommandParam(), "") == 0);

  ucxhPARSER_sendCommand("AT+TEST\n");
  assert(ucxhSERIAL_wasStringWritten("AT+TEST\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);
  assert(strcmp(ucxhPARSER_getLastCommand(), "+TEST") == 0);
  assert(strcmp(ucxhPARSER_getLastCommandParam(), "") == 0);

  ucxhPARSER_sendCommand("AT+TEST\r\n");
  assert(ucxhSERIAL_wasStringWritten("AT+TEST\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);
  assert(strcmp(ucxhPARSER_getLastCommand(), "+TEST") == 0);
  assert(strcmp(ucxhPARSER_getLastCommandParam(), "") == 0);

  printf("ucxhPARSER_sendCommand() tested OK\n");

  const char *DUMMYDATA = "dummy\ndummy";
  ucxhDATA_sendData(DUMMYDATA, strlen(DUMMYDATA));
  assert(ucxhSERIAL_wasStringWritten(DUMMYDATA));

  printf("ucxhDATA_sendData() tested OK\n");


  //
  // Command builder
  //

  // Bool

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addBool(true));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=1\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addBool(false));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=0\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addBool(true));
  assert(ucxhPARSER_addBool(false));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=1,0\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  printf("ucxhPARSER_addBool() tested OK\n");

  // Signed Int

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addSignedInt(INT_MIN));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=-2147483648\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addSignedInt(INT_MAX));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=2147483647\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addSignedInt(1));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=1\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addSignedInt(9));
  assert(ucxhPARSER_addSignedInt(8));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=9,8\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addSignedInt(-9));
  assert(ucxhPARSER_addSignedInt(-8));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=-9,-8\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addSignedInt(-9));
  assert(ucxhPARSER_addSignedInt(7));
  assert(ucxhPARSER_addSignedInt(-8));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=-9,7,-8\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  printf("ucxhPARSER_addSignedInt() tested OK\n");

  // Unsigned Int

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addUnsignedInt(0));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=0\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addUnsignedInt(UINT_MAX));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=4294967295\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addUnsignedInt(9));
  assert(ucxhPARSER_addUnsignedInt(8));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=9,8\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addUnsignedInt(9));
  assert(ucxhPARSER_addUnsignedInt(8));
  assert(ucxhPARSER_addUnsignedInt(7));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=9,8,7\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  printf("ucxhPARSER_addUnsignedInt() tested OK\n");

  // String

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addString(""));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=\"\"\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addString(NULL));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=\"\"\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addString("X"));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=\"X\"\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addString("XYZZY"));
  assert(ucxhPARSER_addString("Y2"));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=\"XYZZY\",\"Y2\"\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addString("XYZZY"));
  assert(ucxhPARSER_addString(""));
  assert(ucxhPARSER_addString("Y2"));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=\"XYZZY\",\"\",\"Y2\"\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  printf("ucxhPARSER_addString() tested OK\n");

  // ByteArray

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addByteArray(0, NULL));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  uint8_t *ptr = "ABC";
  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addByteArray(0, ptr));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addByteArray(0, NULL));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addByteArray(3, ptr));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=414243\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  printf("ucxhPARSER_addByteArray() tested OK\n");

  // IP4vAddress

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addIpV4Address("0.0.0.0"));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=0.0.0.0\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addIpV4Address("127.0.0.1"));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=127.0.0.1\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addIpV4Address("255.255.255.255"));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=255.255.255.255\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(!ucxhPARSER_addIpV4Address("ABC"));
  assert(!ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten(""));

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(!ucxhPARSER_addIpV4Address("1.2.3"));
  assert(!ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten(""));

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(!ucxhPARSER_addIpV4Address("1.2.3.4.5"));
  assert(!ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten(""));

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(!ucxhPARSER_addIpV4Address("1.2.3.256"));
  assert(!ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten(""));

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(!ucxhPARSER_addIpV4Address("256.2.3.4"));
  assert(!ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten(""));

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(!ucxhPARSER_addIpV4Address("1.256.3.4"));
  assert(!ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten(""));

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(!ucxhPARSER_addIpV4Address("1.2.256.4"));
  assert(!ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten(""));

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addIpV4Address("1.23.123.234"));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=1.23.123.234\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  assert(ucxhPARSER_buildSetCmd("MYCMD"));
  assert(ucxhPARSER_addIpV4Address("1.23.123.234"));
  assert(ucxhPARSER_addIpV4Address("23.123.234.42"));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD=1.23.123.234,23.123.234.42\r\n"));
  assert(ucxhPARSER_waitForOk(-1) == ucxhAT_RESPONSE_OK);

  printf("ucxhPARSER_addIpV4Address() tested OK\n");

  assert(ucxhPARSER_buildActionCmd("MYCMD"));
  assert(ucxhPARSER_sendBuiltCommand());
  assert(ucxhSERIAL_wasStringWritten("ATMYCMD\r\n"));

  ucxhSERIAL_close();
  return 0;
}
