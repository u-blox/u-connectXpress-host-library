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
#include <time.h>
#include <unistd.h>

#include "ucxh_config.h"
#include "ucxh_library.h"
#include "ucxh_serial.h"


#include "ucxh_wait.h"

bool ucxhAT_wait(int32_t numSeconds) 
{
  if (numSeconds < 0) {
    numSeconds = ucxhAT_DEFAULT_AT_TIMEOUT;
  } else if (numSeconds > 0) {
    numSeconds++;
  }

  time_t until = time(NULL) + numSeconds;

  bool gotData = false;
  do {
    udprintf(UCXH_PARSER_TRACE_WAIT, 4, "ucxhAT_wait: Invoking ucxhAT_poll()\n", "");
    gotData |= ucxhAT_poll();
    udprintf(UCXH_PARSER_TRACE_WAIT, 4, "ucxhAT_wait: ucxhAT_poll() returned %d\n", gotData);
  } while (time(NULL) < until);

  return gotData;
}


ucxhAT_response_t ucxhAT_waitForOk(int32_t numSeconds) 
{
  if (numSeconds < 0) {
    numSeconds = ucxhAT_DEFAULT_AT_TIMEOUT;
  } else if (numSeconds > 0) {
    numSeconds++;
  }

  time_t until = time(NULL) + numSeconds;
  bool gotData = false;
  ucxhAT_response_t atResponse = ucxhAT_RESPONSE_UNKNOWN;

  do {
    udprintf(UCXH_PARSER_TRACE_WAIT, 3, "ucxhAT_waitForOk: Invoking ucxhAT_poll()\n", "");
    gotData = ucxhAT_poll();
    udprintf(UCXH_PARSER_TRACE_WAIT, 3, "ucxhAT_wait: ucxhAT_poll() returned %d\n", gotData);
    if (gotData) {
      atResponse = atGetLastResponse();
      udprintf(UCXH_PARSER_TRACE_WAIT, 2, "ucxhAT_wait: atGetLastResponse() returned %d\n", atResponse);
    } else {
      atResponse = ucxhAT_RESPONSE_UNKNOWN;
    }
  } while (time(NULL) < until && atResponse != ucxhAT_RESPONSE_OK && atResponse != ucxhAT_RESPONSE_ERROR);
  
  if (atResponse != ucxhAT_RESPONSE_OK && atResponse != ucxhAT_RESPONSE_ERROR) {
    udprintf(UCXH_PARSER_TRACE_WAIT, 2, "ucxhAT_waitForOk: Timeout!\n", "");
  }
  
  return atResponse;
}

bool ucxhEDM_wait(int32_t numSeconds)
{
  if (numSeconds < 0) {
    numSeconds = ucxhAT_DEFAULT_AT_TIMEOUT;
  } else if (numSeconds > 0) {
    numSeconds++;
  }
  
  time_t until = time(NULL) + numSeconds;
  bool success = false;

  do {
      udprintf(UCXH_PARSER_TRACE_WAIT, 4, "ucxhEDM_wait: Invoking ucxhEDM_poll()\n", "");
      success |= ucxhEDM_poll();
      udprintf(UCXH_PARSER_TRACE_WAIT, 4, "ucxhEDM_wait: ucxhEDM_poll() returned %d\n", success);
  } while (time(NULL) < until);

  return success;
}

ucxhAT_response_t ucxhEDM_waitForOk(int32_t numSeconds) 
{
  if (numSeconds < 0) {
    numSeconds = ucxhAT_DEFAULT_AT_TIMEOUT;
  } else if (numSeconds > 0) {
    numSeconds++;
  }

  time_t until = time(NULL) + numSeconds;
  ucxhAT_response_t atResponse = ucxhAT_RESPONSE_UNKNOWN;

  do {
    udprintf(UCXH_PARSER_TRACE_WAIT, 3, "ucxhEDM_waitForOk: Invoking ucxhEDM_poll()\n", "");
    bool success = ucxhEDM_poll();
    udprintf(UCXH_PARSER_TRACE_WAIT, 3, "ucxhEDM_waitForOk: ucxhEDM_poll() returned %d\n", success);
    atResponse = atEdmGetLastResponse();
  } while (time(NULL) < until && atResponse != ucxhAT_RESPONSE_OK && atResponse != ucxhAT_RESPONSE_ERROR);
  
  if (atResponse != ucxhAT_RESPONSE_OK && atResponse != ucxhAT_RESPONSE_ERROR) {
    udprintf(UCXH_PARSER_TRACE_WAIT, 2, "ucxhEDM_waitForOk: Timeout!\n", "");
  }
  
  return atResponse;
}

bool ucxhPARSER_wait(int32_t numSeconds)
{
  if (atGetCurrentMode() == ucxhAT_MODE_EDM) {
    return ucxhEDM_wait(numSeconds);
  } else {
    return ucxhAT_wait(numSeconds);
  }
}

ucxhAT_response_t ucxhPARSER_waitForOk(int32_t numSeconds)
{
  if (atGetCurrentMode() == ucxhAT_MODE_EDM) {
    return ucxhEDM_waitForOk(numSeconds);
  } else {
    return ucxhAT_waitForOk(numSeconds);
  }
}