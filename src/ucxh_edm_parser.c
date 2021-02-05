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
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "ucxh_config.h"
#include "ucxh_library.h"
#include "ucxh_serial.h"

#include "ucxh_edm_parser.h"




/**
 * Returns the next line from the buffer, excluding any leading LF, without any trailing CR or CR LF
 */
const char *atEdmReadLine(char *inbuffer)
{
  assert(inbuffer != NULL);

  static char *token = NULL;

  if (token == NULL) {
    if (strlen(inbuffer) == 0) {
      // Nothing in incoming buffer
      token = NULL;
      return NULL;
    } else {
      // Find first EOL, if present
      
      token = strtok(inbuffer, ucxhAT_LINE_DELIMITER);
      if (token == NULL) {
        // Treat the entire inbuffer as a complete line
        udprintf(UCXH_PARSER_TRACE_EDM_LL_PARSER, 2, "atEdmReadLine: No EOL found\n", "");
        
        token = inbuffer;
      }
    }
  }
  
  const char *line = token;
  // Return token excluding EOL indicator CR, excluding any leading LF
  while (line[0] == LF) {
    line ++;
  }
  udprintf(UCXH_PARSER_TRACE_EDM_LL_PARSER, 2, "atEdmReadLine: Returning token '%s'", line);
  token = strtok (NULL, ucxhAT_LINE_DELIMITER);
  return line;
}


static ucxhAT_response_t atResponse = ucxhAT_RESPONSE_UNKNOWN;

static int (*ucxhEDM_onConnectIpV4Received)(const ucxhEDM_connectionEventIPv4_t *);
bool ucxhEDM_setOnConnectIpV4Received(int (*callback)(const ucxhEDM_connectionEventIPv4_t *))
{
  ucxhEDM_onConnectIpV4Received = callback;
  return true;
}

static int (*ucxhEDM_onConnectIpV6Received)(const ucxhEDM_connectionEventIPv6_t *);
bool ucxhEDM_setOnConnectIpV6Received(int (*callback)(const ucxhEDM_connectionEventIPv6_t *))
{
  ucxhEDM_onConnectIpV6Received = callback;
  return true;
}

static int (*ucxhEDM_onConnectBTReceived)(const ucxhEDM_connectionEventBT_t *);
bool ucxhEDM_setOnConnectBTReceived(int (*callback)(const ucxhEDM_connectionEventBT_t *))
{
  ucxhEDM_onConnectBTReceived = callback;
  return true;
}

static int (*ucxhEDM_onDisconnectReceived)(const ucxhEDM_disconnectEvent_t *);
bool ucxhEDM_setOnDisconnectReceived(int (*callback)(const ucxhEDM_disconnectEvent_t *))
{
  ucxhEDM_onDisconnectReceived = callback;
  return true;
}

static int (*ucxhEDM_onStartupReceived)(void);
bool ucxhEDM_setOnStartupReceived(int (*callback)(void))
{
  ucxhEDM_onStartupReceived = callback;
  return true;
}

void atEdmParsePayloadEvent(const ucxhEDM_event_t* eventData, size_t atMaxBufferSize)
{
  assert(eventData != NULL);
  

  switch(eventData->type) {
    case ucxhEDM_EVENT_CONNECT_BT:
      if (ucxhEDM_onConnectBTReceived != NULL) {
        ucxhEDM_onConnectBTReceived(&eventData->params.btConnectEvent);
      } else {
          udprintf(UCXH_PARSER_TRACE_EDM_PARSER, 2, "ucxhEDM_EVENT_CONNECT_BT ... %s\n", "");
      }
      break;

    case ucxhEDM_EVENT_CONNECT_IPv4:
      if (ucxhEDM_onConnectIpV4Received != NULL) {
        ucxhEDM_onConnectIpV4Received(&eventData->params.ipv4ConnectEvent);
      } else {
          udprintf(UCXH_PARSER_TRACE_EDM_PARSER, 2, "ucxhEDM_EVENT_CONNECT_IPv4: Channel %d: Connection event to localport %u from %u.%u.%u.%u:%u, protocol %s, lastChannel=%d\n",
            eventData->params.ipv4ConnectEvent.hChannel, 
            eventData->params.ipv4ConnectEvent.localPort,
            eventData->params.ipv4ConnectEvent.remoteAddress[0],
            eventData->params.ipv4ConnectEvent.remoteAddress[1],
            eventData->params.ipv4ConnectEvent.remoteAddress[2],
            eventData->params.ipv4ConnectEvent.remoteAddress[3],
            eventData->params.ipv4ConnectEvent.remotePort,
              (eventData->params.ipv4ConnectEvent.protocol == ucxhEDM_IP_PROTOCOL_TCP ? "TCP" : 
              (eventData->params.ipv4ConnectEvent.protocol == ucxhEDM_IP_PROTOCOL_UDP ? "UDP" : 
              (eventData->params.ipv4ConnectEvent.protocol == ucxhEDM_IP_PROTOCOL_MQTT ? "MQTT" : "UNKNOWN"))));
      }
      break;

    case ucxhEDM_EVENT_CONNECT_IPv6:
      if (ucxhEDM_onConnectIpV6Received != NULL) {
        ucxhEDM_onConnectIpV6Received(&eventData->params.ipv6ConnectEvent);
      } else {
          udprintf(UCXH_PARSER_TRACE_EDM_PARSER, 2, "ucxhEDM_EVENT_CONNECT_IPv6 ...%s\n", "");
      }
      break;

    case ucxhEDM_EVENT_DISCONNECT:
      if (ucxhEDM_onDisconnectReceived != NULL) {
        ucxhEDM_onDisconnectReceived(&eventData->params.disconnectEvent);
      } else {
        udprintf(UCXH_PARSER_TRACE_EDM_PARSER, 2, "ucx187hEDM_EVENT_DISCONNECT: Channel %d\n", eventData->params.disconnectEvent.hChannel);
      }
      break;

    case ucxhEDM_EVENT_DATA:
      udprintf(UCXH_PARSER_TRACE_EDM_PARSER, 6, "ucxhEDM_EVENT_DATA: Channel %d length %u", eventData->params.dataEvent.hChannel, eventData->params.dataEvent.length);
      #if (UCXH_PARSER_TRACE_EDM_PARSER >= 7)
        udprintf(UCXH_PARSER_TRACE_EDM_PARSER, 7, ": '", "");
        ucxhPARSER_printBuffer(eventData->params.dataEvent.pData, eventData->params.dataEvent.length);
        udprintf(UCXH_PARSER_TRACE_EDM_PARSER, 7, "'\n", "");
      #else
        udprintf(UCXH_PARSER_TRACE_EDM_PARSER, 6, "%s\n", "");
      #endif
      if (ucxhPARSER_onDataReceived != NULL) {
        ucxhPARSER_onDataReceived((int)(eventData->params.dataEvent.hChannel), (uint8_t *)(eventData->params.dataEvent.pData), (size_t)eventData->params.dataEvent.length);
      }
      break;

    case ucxhEDM_EVENT_AT: {
      // EDM AT packet payloads are NOT NUL-termitted - but the data points
      // directly into the serial buffer, where the payload is followed
      // by the EOT, so it can safely be overwritten
      eventData->params.atEvent.pData[eventData->params.atEvent.length] = '\0';
      udprintf(UCXH_PARSER_TRACE_EDM_PARSER, 3, "ucxhEDM_EVENT_AT: Length %d", strlen(eventData->params.atEvent.pData));
      #if (UCXH_PARSER_TRACE_EDM_PARSER >= 4) 
        udprintf(UCXH_PARSER_TRACE_EDM_PARSER, 4, ": '", "");
        ucxhPARSER_printBuffer(eventData->params.atEvent.pData, eventData->params.atEvent.length);
        udprintf(UCXH_PARSER_TRACE_EDM_PARSER, 4, "'\n", "");
      #else
        udprintf(UCXH_PARSER_TRACE_EDM_PARSER, 3, "%s", "\n", "");
      #endif
      char *startOfLine = eventData->params.atEvent.pData;
      size_t maxLineLength = ucxhAT_RESPONSE_MAX_LEN + ucxtAT_MAX_TOKENS +1;
      while (startOfLine != NULL && startOfLine < eventData->params.atEvent.pData + eventData->params.atEvent.length) {
        // Return line excluding EOL indicator CR
        while (startOfLine[0] == LF) {
          startOfLine ++;
        }
        char *crPos = strchr(startOfLine, CR);
        size_t length = (eventData->params.atEvent.pData + eventData->params.atEvent.length) - startOfLine;
        if(crPos != NULL) {
            length = crPos - startOfLine;
        }

        if (length > maxLineLength-1) {
          udprintf(UCXH_PARSER_TRACE_EDM_PARSER, 1, "ucxhEDM_EVENT_AT line length %d too large, must be < %d '%s'\n", length, maxLineLength, startOfLine);
          length = maxLineLength-1;
          if (crPos != NULL) {
            crPos = startOfLine + length;
          }
        }

        if (length > 0) {
          startOfLine[length] = '\0';

          assert(strlen(startOfLine) == length);

          udprintf(UCXH_PARSER_TRACE_EDM_PARSER, 5, "ucxhEDM_EVENT_AT parsing line length %d '%s'\n", length, startOfLine);
          
          ucxhAT_response_t response = atParseResponse(startOfLine);
          if (response == ucxhAT_RESPONSE_OK || response == ucxhAT_RESPONSE_ERROR) {
            atResponse = response;
          }
        }
        if (crPos != NULL) {
          startOfLine = crPos+1;
        } else {
          startOfLine = NULL;
        }
      }
      break;
    }

    case ucxhEDM_EVENT_STARTUP:
      if (ucxhEDM_onStartupReceived != NULL) {
        ucxhEDM_onStartupReceived();
      } else {
        udprintf(UCXH_PARSER_TRACE_EDM_PARSER, 2, "ucxhEDM_EVENT_STARTUP%s\n", "");
      }
      break;

    case ucxhEDM_EVENT_INVALID:
    default:
      udprintf(UCXH_PARSER_TRACE_EDM_PARSER, 1, "ucxhEDM_EVENT_INVALID: Invalid EDM event %d\n", eventData->type);
      break;
  }
}

ucxhAT_response_t atEdmGetLastResponse(void)
{
  return atResponse;
}


static unsigned resetBufferSize = 0;
static uint8_t *resetBufferFrom = NULL;
static int (*readMore)(uint8_t*, size_t) = NULL;

static int atEdmResetBuffer(uint8_t* inData, size_t bufsize) {
  assert(inData != NULL);
  assert(bufsize >= ucxhEDM_MIN_SIZE);

  if (resetBufferFrom != NULL && resetBufferSize > 0) {
    udprintf(UCXH_PARSER_TRACE_EDM_LL_PARSER, 2, "atEdmResetBuffer: Keeping %d bytes\n", resetBufferSize);

    assert(resetBufferSize <= bufsize);
    memmove(inData, resetBufferFrom, resetBufferSize);
    memset(inData+resetBufferSize, 0x00, bufsize-resetBufferSize);
    readMore = NULL;
    resetBufferFrom = NULL;

    int bytesInBuffer = resetBufferSize;
    resetBufferSize = 0;
    return bytesInBuffer;
  }

  return 0;
}



/*
 * @retval ucxhEDM_OK Successful parsing
 * @retval ucxhEDM_ERROR_PARAM inData is NULL or size in invalid
 * @retval ucxhEDM_ERROR_INVALID Not a valid EDM packet
 * @retval ucxhEDM_ERROR_INCOMPELE Start of a valid EDM packet, but does not contain a full packet
 * @retval ucxhEDM_ERROR_SIZE EDM size header does not match actual data, should never happen
 * @retval ucxhEDM_ERROR_CORRUPTED Invalid value of data inside the EDM packet, should never happen
 */

int atEdmReadPacket(int (*defaultReadMore)(uint8_t*, size_t), uint8_t* inData, size_t bufSize, ucxhEDM_event_t* eventData)
{
  assert(defaultReadMore != NULL);
  assert(inData != NULL);
  assert(eventData != NULL);
  assert(bufSize >= ucxhEDM_MIN_SIZE);

  int bytesRead = 0;
  unsigned pos = 0;
  bool startOfPacketFound = false;

  if (readMore == NULL) {
    readMore = defaultReadMore;
  }

  do {
    // Make sure we're synchronized at ucxhEDM_SOH
    // The first invocation of readMore() may invoke atEdmResetBuffer if we had data remaining in the serial buffer 
    // If so, it will ensure the data from the last EDM packet is discarded, and the remaining data will start
    // at inData[0]. Subsequent invocations to readMore() will invoke the defaultReadMore().
    bytesRead = readMore(inData+pos, bufSize-pos); 
    readMore = defaultReadMore; 
    if (bytesRead > 0) {
      pos += bytesRead;
      udprintf(UCXH_PARSER_TRACE_EDM_LL_PARSER, 2, "atEdmReadPacket: Got %d from bytesRead\n", bytesRead);

      for (int startOfPacketPos = 0; startOfPacketPos < pos; startOfPacketPos++) {
        if (inData[startOfPacketPos] == ucxhEDM_SOH) {
          assert(bytesRead - startOfPacketPos >= 0);

          udprintf(UCXH_PARSER_TRACE_EDM_LL_PARSER, 2, "atEdmReadPacket: EDM SOH Synchronized at %u.\n", startOfPacketPos);
          // We're synchronized now at an ucxhEDM_SOH
          memmove(inData, inData+startOfPacketPos, pos-startOfPacketPos);
          bytesRead -= startOfPacketPos;
          pos -= startOfPacketPos;
          startOfPacketFound = true;
          udprintf(UCXH_PARSER_TRACE_EDM_LL_PARSER, 2, "atEdmReadPacket: We now have %d bytes: ", pos);
          #if (UCXH_PARSER_TRACE_EDM_LL_PARSER >= 3) 
            udprintf(UCXH_PARSER_TRACE_EDM_LL_PARSER, 3, ": '","");
            ucxhPARSER_printBuffer(inData, pos);
            udprintf(UCXH_PARSER_TRACE_EDM_LL_PARSER, 3, "'\n","");
          #else
            udprintf(UCXH_PARSER_TRACE_EDM_LL_PARSER, 2, "'\n","");
          #endif
          break;
        }
      }

      if (!startOfPacketFound) {
        udprintf(UCXH_PARSER_TRACE_EDM_LL_PARSER, 2, "atEdmReadPacket: SOH still not found, dropping data and restarting\n","");
        pos = 0; // Disregard  entire buffer, and start filling from 0 again...
      }
    } else if (bytesRead < 0) {
      udprintf(UCXH_PARSER_TRACE_EDM_LL_PARSER, 2, "atEdmReadPacket: EDM synchronization read error: %u.\n", -bytesRead); // TODO: Avoid spamming in case of consecutive read errors.
      return 0;
    }
  } while (!startOfPacketFound && bytesRead > 0);

  if (!startOfPacketFound) {
    return 0;
  }

  // Make sure we've got at least enough for an EDM header and empty packet
  assert(bytesRead > 0);
  while (pos < ucxhEDM_MIN_SIZE && bytesRead > 1) {
    bytesRead = readMore(inData+pos, bufSize-pos);
    if (bytesRead > 0) {
      pos += bytesRead;
    }
  }
  if (pos < ucxhEDM_MIN_SIZE) {
    return 0; 
  }


  // Now we know we have enough data for a potential EDM header and empty packet.
  int consumed = -EPROTO; // Just in case
  udprintf(UCXH_PARSER_TRACE_EDM_LL_PARSER, 2, "atEdmReadPacket:Potential EDM packet: We now have %d bytes:\n", pos);

  // try parsing until successful
  assert(pos >= ucxhEDM_MIN_SIZE);
  int result = edmParse(inData, pos, eventData, &consumed);
  assert(result != ucxhEDM_ERROR_PARAM); // We already ensured we call with correct parameters
  udprintf(UCXH_PARSER_TRACE_EDM_LL_PARSER, 2, "atEdmReadPacket: edmParse returned result=%d, consumed=%d\n", result, consumed);

  switch (result) {
    case ucxhEDM_OK:
      assert(consumed >= ucxhEDM_MIN_SIZE);
      udprintf(UCXH_PARSER_TRACE_EDM_LL_PARSER, 2, "EDM packet completed, pos=%d, consumed=%d, bytesRead=%d\n", pos, consumed, bytesRead);
      
      assert(bufSize >= pos);
      if (pos > consumed) {
        // There is data remaining in the serial in-buffer. Presumably, more EDM-packets.
        resetBufferFrom = inData+consumed;
        resetBufferSize =  pos-consumed;
        readMore = atEdmResetBuffer;
        // BUG: Will loose data in case EDM is left and there's more queued in the UART inbuffer (but EDM cannot be left as of now)
      } else {
        // Nothing remaining in serial in-buffer. Good! Next inovcation will invoke the default data getter.
        readMore = NULL;
        resetBufferFrom = NULL;
        resetBufferSize = 0;
      }
      break;

    case ucxhEDM_ERROR_INCOMPLETE:
      udprintf(UCXH_PARSER_TRACE_EDM_LL_PARSER, 2, "atEdmReadPacket: EDM packet incomplete\n","");
      return 0;

    case ucxhEDM_ERROR_SIZE: // FALLTHROUGH   // No ucxhEDM_EOT found at expected location - the received ucxhEDM_SOH was probably stray data
    case ucxhEDM_ERROR_CORRUPTED:             // The packet appeared valid, but the payload was not a valid EDM event or EDM AT response
      udprintf(UCXH_PARSER_TRACE_EDM_LL_PARSER, 2, "atEdmReadPacket: ucxhEDM_ERROR %d %s!\n", result, 
        (result == ucxhEDM_ERROR_SIZE ? " (Invalid size or missing ucxhEDM_EOT)" :
        (result == ucxhEDM_ERROR_CORRUPTED ? " (Invalid payload)": "")));
      // TODO: Discard and retry in a better way
      return -ENOMSG;

    case ucxhEDM_ERROR_INVALID: // FALLTHROUGH
    default:
      assert(result != ucxhEDM_ERROR_INVALID); // We've already ensured we call with SOH and enough bytes for a packet
      udprintf(UCXH_PARSER_TRACE_EDM_LL_PARSER, 2, "atEdmReadPacket: ucxhEDM_ERROR: Unknown result %d!\n", result);
      return -EPROTO;
  }

  return consumed;
}

