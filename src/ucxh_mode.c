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
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "ucxh_config.h"
#include "ucxh_library.h"
#include "ucxh_serial.h"

#include "ucxh_mode.h"


#define ucxhAT_CMD_ENTER_DATA_MODE "ATO1"
#define ucxhAT_CMD_ENTER_EXTENDED_DATA_MODE "ATO2"
#define ucxhAT_CMD_LEAVE_DATA_MODE "+++"


static ucxhAT_mode_t atCurrentMode = ucxhAT_MODE_UNKNOWN;

bool ucxhMODULE_enterAtMode(void)
{
  udprintf(UCXH_PARSER_TRACE_AT, 2, "ucxhMODULE_enterAtMode: Transmitting '%s' ... ", ucxhAT_CMD_LEAVE_DATA_MODE);
  usleep(1100000);  // 1.1s
  int bytesWritten = ucxhSERIAL_write(ucxhAT_CMD_LEAVE_DATA_MODE, 3);
  assert(bytesWritten == 3);
  usleep(1100000);  // 1.1s

  atCurrentMode = ucxhAT_MODE_AT;

  ucxhAT_sendCommand("AT");
  if (ucxhAT_waitForOk(-1) != ucxhAT_RESPONSE_OK) {  // If failed, module is probably in EDM mode.
    atCurrentMode = ucxhAT_MODE_UNKNOWN;
    return false;
  }

  return true;
}

bool ucxhMODULE_enterDataMode(void)
{
  udprintf(UCXH_PARSER_TRACE_AT, 2, "ucxhMODULE_enterDataMode: Transmitting '%s' ... ", ucxhAT_CMD_ENTER_DATA_MODE);
  ucxhAT_sendCommand(ucxhAT_CMD_ENTER_DATA_MODE);

  if (ucxhAT_waitForOk(-1) != ucxhAT_RESPONSE_OK) {  // If failed, module is probably in data or EDM mode.
    atCurrentMode = ucxhAT_MODE_UNKNOWN;
    return false;
  }
  
  usleep(55000); // 50 ms minimum before transmitting data

  atCurrentMode = ucxhAT_MODE_DATA;
  return true;
}

bool ucxhMODULE_enterExtendedDataMode(void)
{
  udprintf(UCXH_PARSER_TRACE_AT, 2, "ucxhMODULE_enterExtendedDataMode: Transmitting '%s' ... ", ucxhAT_CMD_ENTER_EXTENDED_DATA_MODE);
  ucxhAT_sendCommand(ucxhAT_CMD_ENTER_EXTENDED_DATA_MODE);
  
  if (ucxhAT_waitForOk(-1) != ucxhAT_RESPONSE_OK) {  // If failed, module is probably in data or EDM mode. TODO: Try leaving datamode, then retry entering EDM mode again.
    atCurrentMode = ucxhAT_MODE_UNKNOWN;
    return false;
  }

  usleep(55000); // 50 ms minimum before transmitting data

  atCurrentMode = ucxhAT_MODE_EDM;
  return true;
}

bool atLeaveExtendedDataMode(void)
{
  ucxhMODULE_restart();
  ucxhAT_wait(3);
  ucxhAT_sendCommand("AT");
  if (ucxhAT_waitForOk(-1) != ucxhAT_RESPONSE_OK) {  // If failed, module is probably in data or EDM mode due to startup mode configuration. 
    if (ucxhMODULE_enterAtMode()) { // If failed, we're in EDM mode due to startup mode configuration. TODO: Change startupmode to AT, save and restart
      atCurrentMode = ucxhAT_MODE_AT; 
      return true;
    }
    atCurrentMode = ucxhAT_MODE_EDM;
    return false;
  }

  atCurrentMode = ucxhAT_MODE_AT;
  return true;
}

ucxhAT_mode_t atGetCurrentMode(void)
{
  return atCurrentMode;
}