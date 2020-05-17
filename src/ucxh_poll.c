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

#include "ucxh_poll.h"


#define ucxhAT_DATA_MODE_DATA_MAX_SIZE 256

int (*ucxhPARSER_onDataReceived)(int channel, const uint8_t *data, size_t size);

bool ucxhPARSER_setOnDataReceived(int (*callback)(int, const uint8_t *, size_t))
{
  ucxhPARSER_onDataReceived = callback;
  return true;
}


bool ucxhAT_poll(void)
{
  static bool noAtPollInProgress = true;
  bool parsed = false;
  ucxhAT_mode_t atMode = atGetCurrentMode();

  // Protection agains recursion
  assert(noAtPollInProgress);
  noAtPollInProgress = false;

  if (atMode == ucxhAT_MODE_DATA) {
    uint8_t inDataBuffer[ucxhAT_DATA_MODE_DATA_MAX_SIZE];
    int bytesRead = ucxhSERIAL_read(inDataBuffer, sizeof(inDataBuffer));
    
    if (bytesRead == 0) {
      // Nothing in incoming buffer
    } else if (bytesRead > 0) {
      if (ucxhPARSER_onDataReceived != NULL) {
        int callback_result = ucxhPARSER_onDataReceived(-1, inDataBuffer, bytesRead);
      } else {
        udprintf(UCXH_PARSER_TRACE_POLL, 1, "ucxhAT_poll: Unhandled data received: '%s'\n", inDataBuffer);
      }
      parsed = true;
    }
  } else {
    // assume AT mode
    assert(atMode != ucxhAT_MODE_EDM);
    
    size_t atMaxBufferSize = -1;
    char *responseLine = atReadLine(&atMaxBufferSize);
    
    if (responseLine != NULL) {
      udprintf(UCXH_PARSER_TRACE_POLL, 2, "ucxhAT_poll: Invoking ucxhMODULE_prePollWifi()...\n", "");
      ucxhMODULE_prePollWifi();
      udprintf(UCXH_PARSER_TRACE_POLL, 2, "ucxhAT_poll: ucxhMODULE_prePollWifi() returned...\n", "");

      atParseResponse(responseLine);
      
      udprintf(UCXH_PARSER_TRACE_POLL, 2, "ucxhAT_poll: Invoking ucxhMODULE_postPollWifi()...\n", "");
      ucxhMODULE_postPollWifi();
      udprintf(UCXH_PARSER_TRACE_POLL, 2, "ucxhAT_poll: ucxhMODULE_postPollWifi() returned...\n", "");

      parsed = true;
    } 
  } 
  noAtPollInProgress = true;

  return parsed;
}

static uint8_t edmBuffer[ucxhEDM_BUFFER_SIZE] = {0}; // static since it is so large

bool ucxhEDM_poll(void)
{
  static bool noEdmPollInProgress = true;
  ucxhAT_mode_t atMode = atGetCurrentMode(); 
  ucxhEDM_event_t eventData = {0};

  // Protection agains recursion
  assert(noEdmPollInProgress);
  noEdmPollInProgress = false;

  assert(atMode == ucxhAT_MODE_EDM);

  int consumed = atEdmReadPacket(ucxhSERIAL_read, edmBuffer, sizeof(edmBuffer), &eventData);

  if (consumed > 0) {
    udprintf(UCXH_PARSER_TRACE_POLL, 2, "ucxhEDM_poll: atEdmReadPacket succeeded, eventData.type=%u\n", eventData.type);
    udprintf(UCXH_PARSER_TRACE_POLL, 2, "ucxhEDM_poll: Invoking ucxhMODULE_prePollWifi()...\n", "");
    ucxhMODULE_prePollWifi();
    udprintf(UCXH_PARSER_TRACE_POLL, 2, "ucxhEDM_poll: ucxhMODULE_prePollWifi() invoked...\n", "");

    atEdmParsePayloadEvent(&eventData, sizeof(edmBuffer));

    udprintf(UCXH_PARSER_TRACE_POLL, 2, "ucxhEDM_poll: Invoking ucxhMODULE_postPollWifi()...\n", "");
    ucxhMODULE_postPollWifi();
    udprintf(UCXH_PARSER_TRACE_POLL, 2, "ucxhEDM_poll: ucxhMODULE_postPollWifi() invoked...\n", "");
  }

  noEdmPollInProgress = true;
  return (consumed > 0);
}

bool ucxhPARSER_poll(void)
{
  static bool noPollInProgress = true;
  bool consumed = false;

  // Protection against recursion
  assert(noPollInProgress);
  noPollInProgress = false;

  if (atGetCurrentMode() == ucxhAT_MODE_EDM) {
    consumed = (ucxhEDM_poll() > 0);
  } else {
    consumed = ucxhAT_poll();
  }

  noPollInProgress = true;
  return consumed;
}


