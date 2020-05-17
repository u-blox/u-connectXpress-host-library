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

#include "cb_tokenizer.h"
#include "ucxh_config.h"
#include "ucxh_library.h"
#include "ucxh_serial.h"

#include "ucxh_parser.h"



const char *ucxhAT_RESPONSE[] = {
  "OK",
  "ERROR",
  "STARTUP",
  "PARSED",
  "UNKNOWN"
};

const char *ucxhPARSER_ascii7ToUnicode(uint8_t c) {
  if (isprint(c)) {
    static uint8_t buf[5];
    sprintf(buf, "%c", c);
    return buf;
  } else {
    static const char *unicodestr[] = {
      "␀", // U+2400 
      "␁", // U+2401 (SOH)
      "␂", // U+2402 (STX)
      "␃", // U+2403 (ETX)
      "␄", // U+2404 (EOT)
      "␅", // U+2405 (ENQ)
      "␆", // U+2406 (ACK)
      "⍾", //        (BEL)
      "←", // U+2190 (BS)
      "⇥", // U+24A6 (TAB)
      "↓", // U+2193 (LF)
      "␋", // U+240B
      "␌", // U+240C
      "↵", // U+21B2 (CR)
      "␎", // U+240E
      "␏", // U+240F
      "␐", // U+2410 (DLE)
      "␑", // U+2411 (DC1/XON)
      "␒", // U+2412
      "␓", // U+2413 (DC1/XOFF)
      "␔", // U+2414
      "␕", // U+2415 (NAK)
      "␖", // U+2416 (SYN)
      "␗", // U+2417 (ETB)
      "␘", // U+2418 (CAN)
      "␙", // U+2419
      "␚", // U+241A
      "␛", // U+241B (ESC)
      "␜", // U+241C
      "␝", // U+241D
      "␞", // U+241E
      "␟", // U+241F
    };

    if (c < ELEMENTS_OF(unicodestr)) {
      return unicodestr[c];
    } else if (c == '\x7F') {
      return "↛"; // U+219B (DEL)
    }
  }
  return "�"; // U+FFFD (Default for unprintable)
}

void ucxhPARSER_printBuffer(const uint8_t *buffer, size_t length)
{
  for (int i = 0; i < length; i++) {
    printf("%s", ucxhPARSER_ascii7ToUnicode(buffer[i]));
  }
}


void ucxhPARSER_checkParams(uint8_t numParams, const char *ppParams[], const char *file, uint32_t line)
{
    for (int i = 0; i < numParams ; i++) {
        #if (UCXH_PARSER_TRACE_URC >= 3)
            if (ppParams[i] != NULL) {
                udprintf(UCXH_PARSER_TRACE_URC, 3, "%s", ppParams[i]);
                if (i > 0) {
                    if (i < numParams-1) {
                        udprintf(UCXH_PARSER_TRACE_URC, 3, ", ", "");
                    } else {
                        udprintf(UCXH_PARSER_TRACE_URC, 3, "\n", "");
                    }
                }
            }
        #endif
        if(ppParams[i] == NULL) {
          udprintf(UCXH_PARSER_TRACE_URC, 0, "%s:%u: ppParams[%u] is NULL!\n", file, line, i);
        }
        assert(ppParams[i] != NULL);
    }
}

char *atReadLine(size_t *atMaxBufferSize)
{
  static char *token = NULL;
  static char inputLineBuffer[ucxhAT_INBUF_SIZE+1] = {'\0'};

  assert(atMaxBufferSize != NULL);

  *atMaxBufferSize = sizeof(inputLineBuffer);
  
  if (token == NULL) {
    int bytesRead = ucxhSERIAL_read(inputLineBuffer, sizeof(inputLineBuffer)-1);
    
    if(bytesRead == 0) {
      // Nothing in incoming buffer
      token = NULL;
      return NULL;
    } else if (bytesRead > 0) {
      // Find first EOL, if present
      
      inputLineBuffer[bytesRead] = '\0';
      token = strtok(inputLineBuffer, ucxhAT_LINE_DELIMITER);
      if (token == NULL) {
        // Treat the entire inputLineBuffer as a complete line
        udprintf(UCXH_PARSER_TRACE_AT_PARSER, 4, "atReadLine: No EOL found\n", "");
        
        token = inputLineBuffer;
      }
    }
  }
  
  char *line = token;

  // Return token excluding EOL indicator CR, excluding any leading LF or space
  size_t movedBytes = 0;
  while (line[0] == LF || line[0] == ' ') {
    line ++;
    movedBytes ++;
  }
  if (movedBytes > 0) {
    size_t lineLength = strlen(line);

    memmove(token, line, lineLength+1);
    memset(token + lineLength+1, 0x00, movedBytes);
    *atMaxBufferSize = lineLength+1 + movedBytes;
  }

  token = strtok (NULL, ucxhAT_LINE_DELIMITER);
  if (token == NULL) {
    // This was the last line in the inputLineBuffer, thus
    // the remaining part of the buffer is free for use.
    size_t offsetInbuffer = line - inputLineBuffer;
    *atMaxBufferSize = sizeof(inputLineBuffer) - offsetInbuffer;
  }
  
  return line;
}


static int (*ucxhPARSER_onURCParsed)(uint8_t numParams, const char *ppParams[]);
bool ucxhPARSER_setOnURCParsed(int (*callback)(uint8_t numParams, const char *ppParams[]))
{
  ucxhPARSER_onURCParsed = callback;
  return true;
}


static int (*ucxhPARSER_onUnknownResponseReceived)(const char *urc);
bool ucxhPARSER_setOnUnknownURCReceived(int (*callback)(const char *))
{
  ucxhPARSER_onUnknownResponseReceived = callback;
  return true;
}


static int (*ucxhPARSER_onOkReceived)(const char *lastcommand);
bool ucxhPARSER_setOnOkReceived(int (*callback)(const char *lastcommand))
{
  ucxhPARSER_onOkReceived = callback;
  return true;
}


static int (*ucxhPARSER_onErrorReceived)(const char *lastcommand);
bool ucxhPARSER_setOnErrorReceived(int (*callback)(const char *lastcommand))
{
  ucxhPARSER_onErrorReceived = callback;
  return true;
}

// static definition of function pointers for external callbacks, named uxchPARSER_onIdentifier(ucxhPARSER_urc##Identifier_t *pEvent)
#define _XMACRO(_1, _2, Identifier, _4, _5) static int (*ucxhPARSER_on##Identifier)(ucxhPARSER_urc##Identifier##Event_t *);

  LIST_OF_PLUS_URC
#undef _XMACRO

// static definition booleans indicating if a specific onXXX() and subsequent callback shall be invoked before OK is returned.
#define _XMACRO(_1, _2, Identifier, _4, _5) static bool onOkFinalize##Identifier;

  LIST_OF_PLUS_URC
#undef _XMACRO


// global definition of ucxhPARSER_setOnIdentifier() 
#define _XMACRO(_1, _2, Identifier, _4, _5) \
  bool ucxhPARSER_setOn##Identifier(int (*callback)(ucxhPARSER_urc##Identifier##Event_t *pEvent)) \
  { \
    if (ucxhPARSER_on##Identifier != NULL && callback != NULL) { return false; } \
    ucxhPARSER_on##Identifier = callback; \
    return true; \
  }

  LIST_OF_PLUS_URC
#undef _XMACRO



// static definition of function pointers for internal OK/Error callbacks, named uxchPARSER_internalOnURCOk(void) and uxchPARSER_internalOnURCError(void)
#define _XMACRO(URCname, _2, _3, _4, _5) \
  static int (*ucxhPARSER_internalOn##URCname##Ok)(void *_); \
  static int (*ucxhPARSER_internalOn##URCname##Error)(void *_); \
 
  LIST_OF_PLUS_URC
#undef _XMACRO
 
// global definition of ucxhPARSER_internalSetOnURCNameOk() 
#define _XMACRO(URCname, _2, _3, _4, URCcallbackOnOk) \
  bool ucxhPARSER_internalSetOn##URCname##Ok(int (*callback)(void *_)) \
  { \
    if (URCcallbackOnOk == NULL || (ucxhPARSER_internalOn##URCname##Ok != NULL && callback != NULL)) { return false; } \
    ucxhPARSER_internalOn##URCname##Ok = callback; \
    return true; \
  }
 
  LIST_OF_PLUS_URC
#undef _XMACRO

// global definition of ucxhPARSER_setOnURCNameError() 
// TODO: Invoke ucxhPARSER_setOnURCNameError() if callback is defined.
#define _XMACRO(URCname, _2, _3, _4, URCcallbackOnOk) \
  bool ucxhPARSER_internalSetOn##URCname##Error(int (*callback)(void *_)) \
  { \
    if (URCcallbackOnOk == NULL || (ucxhPARSER_internalOn##URCname##Error != NULL && callback != NULL)) { return false; } \
    ucxhPARSER_internalOn##URCname##Error = callback; \
    return true; \
  }
 
  LIST_OF_PLUS_URC
#undef _XMACRO

static int (*commandSentDoneCallback)(bool success, void *userdata) = NULL;
static void *commandSentDoneUserdata = NULL;

void ucxhPARSER_setOnCommandSentDone(int (*callback)(bool success, void *userdata), void *userdata)
{
  assert(commandSentDoneCallback == NULL);
  commandSentDoneCallback = callback;
  commandSentDoneUserdata = userdata;
}

static ucxhAT_response_t atResponse = ucxhAT_RESPONSE_UNKNOWN;

ucxhAT_response_t atParseResponse(const char *line)
{
  assert(line != NULL);
  char *tokens[ucxtAT_MAX_TOKENS] = {0};
  uint8_t numTokens = 0;

  if (strcmp(line, "OK") == 0) {
      const char *lastCommand = ucxhPARSER_getLastCommand();

      // The reason for implementing the selection of parsing function using X-MACROs, is
      // to avoid void * pointers (in case of a simple lookup-table), too many enums
      // and copy-paste errors.
      
     /** Invokes onUUU_createEvent() if finalizing is required as defined in ucxh_urc.h,
      * and optionally also the ucxhPARSER_onXXX() callback if allowed as defined in ucxh_urc.h.
      * and optionally also the internal ucxhPARSER_onXXXOk() callback if registered
      */
      #define _XMACRO(URCname, URCfullName, URCidentifier, URCneedMore, URCcallbackOnOk) \
      do { \
        if (URCneedMore == ucxhURC_MUST_FINALIZE && onOkFinalize## URCidentifier == true) { /*  Hopefully, this gets optimized out by the compiler if not needed, otherwize it'll just be dead code. */ \
          ucxhPARSER_urc##URCidentifier##Event_t urcEvent; \
          ucxhURC_parseResult_t result = on ## URCname ## _createEvent(0, NULL, &urcEvent); \
          if (result == ucxhURC_PARSE_RESULT_OK) { \
            if (ucxhPARSER_on##URCidentifier != NULL) { \
              udprintf(UCXH_PARSER_TRACE_AT_PARSER, 3, "ucxh_parser: OK to be handled by client callback for +%s: (%s)\n", #URCname, #URCfullName); \
              /* If you want to post the event to another task, this is one of the two lines to modify: */ \
              int callback_result = ucxhPARSER_on##URCidentifier(&urcEvent); \
              udprintf(UCXH_PARSER_TRACE_AT_PARSER, 4, "ucxh_parser: Callback returned %d for +%s: (%s)\n", callback_result, #URCname, #URCfullName); \
            } else { \
              udprintf(UCXH_PARSER_TRACE_AT_PARSER, 2, "ucxh_parser: OK finalized but no callback for +%s: (%s)\n", #URCname, #URCfullName); \
            } \
            onOkFinalize## URCidentifier = false; \
          } else if (result == ucxhURC_PARSE_RESULT_NEED_MORE) { \
            udprintf(UCXH_PARSER_TRACE_AT_PARSER, 1, "ucxh_parser: OK tried to finalize, but on%s() still needs more (%s)\n", #URCname, #URCfullName); \
            onOkFinalize## URCidentifier  = true; \
          } else { \
            udprintf(UCXH_PARSER_TRACE_AT_PARSER, 1, "ucxh_parser: OK tried to finalize, but on" "() %s: FAILED (%s)\n", #URCname, #URCfullName); \
          } \
          break; \
        } \
      } while(0); \
      \
      do { \
        if (URCcallbackOnOk != NULL && strcmp(lastCommand, #URCname) == 0) { /** Hopefully, this gets optimized out by the compiler if not needed, otherwize it'll just be dead code. */ \
          if (ucxhPARSER_internalOn##URCname##Ok != NULL) { \
            udprintf(UCXH_PARSER_TRACE_AT_PARSER, 3, "ucxh_parser: OK to be handled by (internal callback) for +%s: (%s)\n", #URCname, #URCfullName); \
            int callback_result = ucxhPARSER_internalOn##URCname##Ok(NULL); \
            udprintf(UCXH_PARSER_TRACE_AT_PARSER, 4, "ucxh_parser: Callback returned %d for +%s: (%s)\n", callback_result, #URCname, #URCfullName); \
          } else { \
            udprintf(UCXH_PARSER_TRACE_AT_PARSER, 2, "ucxh_parser: OK (internal callback) FAILED for +%s: (%s)\n", #URCname, #URCfullName); \
          } \
        } \
      } while (0);
      
        LIST_OF_PLUS_URC
      #undef _XMACRO

      atResponse = ucxhAT_RESPONSE_OK;
  } else if (strcmp(line, "ERROR") == 0) {
      atResponse = ucxhAT_RESPONSE_ERROR;
  } else {
    atResponse = ucxhAT_RESPONSE_UNKNOWN;
    static char tokenizableLine[ucxhAT_RESPONSE_MAX_LEN + ucxtAT_MAX_TOKENS +1] = {0}; // Static since it might be large.
    size_t lineLength = strlen(line);
    char *dest = &tokenizableLine[0];
    size_t commandLength = 0;

    if (line[0] == '"' || isdigit(line[0])) {
      // Some command responses, most commonly legacy hayes such as ATI, AT+GMM, do not begin with +URC:; So just add them to simplify parsing

      const char *lastCommand = ucxhPARSER_getLastCommand();
      commandLength = strlen(lastCommand);
      const char *plus = "";
      size_t plusLength = 0;
      if(lastCommand[0] != '+') {
        // Typically ATIx => Treat as if it is called AT+Ix
        plus = "+";
        plusLength = 1;
      } 
      if (plusLength + commandLength + 1 + lineLength <= ucxhAT_RESPONSE_MAX_LEN) {
        udprintf(UCXH_PARSER_TRACE_AT_PARSER, 2, "ucxh_parser: Prepending '%s%s:' to '%s'\n", plus, lastCommand, line); \
        if (plusLength > 0) {
          tokenizableLine[0] = '+';
        }
        strncpy(tokenizableLine+plusLength, lastCommand, sizeof(tokenizableLine)-plusLength);
        tokenizableLine[plusLength + commandLength] = ':';
        tokenizableLine[sizeof(tokenizableLine)-1] = '\0';
        dest = &tokenizableLine[plusLength+commandLength+1];
        commandLength += (plusLength+1);
      } else {
          udprintf(UCXH_PARSER_TRACE_AT_PARSER, 2, "ucxh_parser: Line too long to generate %s%s: for '%s'\n", plus, lastCommand, line); \
      }
    }

    memcpy(dest, line, lineLength);
    memset(dest+lineLength, 0x00, ucxhAT_RESPONSE_MAX_LEN + ucxtAT_MAX_TOKENS-lineLength-commandLength +1);

    do {
      cbRTSL_Status status = cb_Tokenize(tokenizableLine, sizeof(tokenizableLine), ucxtAT_MAX_TOKENS, tokens, &numTokens);
      #if (UCXH_PARSER_TRACE_AT_PARSER >= 4)
        for (int i = 0; i < numTokens; i++) {
          udprintf(UCXH_PARSER_TRACE_AT_PARSER, 4, "token[%d]='%s'\n", i, tokens[i]);
        }
      #endif
      if (numTokens < 1 || status != cbSTATUS_OK || numTokens > ucxtAT_MAX_TOKENS) {
        atResponse = ucxhAT_RESPONSE_UNKNOWN;
        break;
      }

      // The reason for implementing the selection of parsing function using X-MACROs, is
      // to avoid void * pointers (in case of a simple lookup-table), too many enums
      // and copy-paste errors.

     /** Invokes 
      * onUUU_createEvent()
      * and optionally also the ucxhPARSER_onXXX() callback if event was created successfully, and a callback has been registered.
      */
      #define _XMACRO(URCname, URCfullName, URCidentifier, URCneedMore, _5) \
        if (strcmp(tokens[0], "+" #URCname ":") == 0 || (URCneedMore == ucxhURC_NO_COLON && strcmp(tokens[0], "+" #URCname) == 0)) { \
          ucxhPARSER_urc##URCidentifier##Event_t urcEvent; \
          ucxhURC_parseResult_t result = on ## URCname ## _createEvent(numTokens, (const char **)&tokens[0], &urcEvent); \
          udprintf(UCXH_PARSER_TRACE_AT_PARSER, 4, "ucxh_parser: %s createEvent returned %d (%s)\n", tokens[0], result, #URCfullName); \
          if (result == ucxhURC_PARSE_RESULT_OK) { \
            atResponse =  ucxhAT_RESPONSE_PARSED; \
            if (ucxhPARSER_on##URCidentifier != NULL) { \
              udprintf(UCXH_PARSER_TRACE_AT_PARSER, 3, "ucxh_parser: %s to be handled by callback: (%s)\n", tokens[0], #URCfullName); \
              /* If you want to post the event to another task, this is one of the two lines to modify: */ \
              int callback_result = ucxhPARSER_on##URCidentifier(&urcEvent); \
              udprintf(UCXH_PARSER_TRACE_AT_PARSER, 4, "ucxh_parser: %s Callback returned %d: (%s)\n", tokens[0], callback_result, #URCfullName); \
            } else { \
              udprintf(UCXH_PARSER_TRACE_AT_PARSER, 2, "ucxh_parser: %s parsed but no callback: (%s)\n", tokens[0], #URCfullName); \
            } \
            if (URCneedMore == ucxhURC_MUST_FINALIZE) { \
              onOkFinalize## URCidentifier = false; \
            } \
          } else if (result == ucxhURC_PARSE_RESULT_NEED_MORE) { \
            udprintf(UCXH_PARSER_TRACE_AT_PARSER, 3, "ucxh_parser: on" #URCname "() %s: Need more before invoking callback\n", ""); \
            atResponse =  ucxhAT_RESPONSE_PARSED; \
            assert(URCneedMore == ucxhURC_MUST_FINALIZE); \
            onOkFinalize## URCidentifier  = true; \
          } else { \
            udprintf(UCXH_PARSER_TRACE_AT_PARSER, 1, "ucxh_parser: on" #URCname "() %s: Syntax error in URC\n", "FAILED"); \
            atResponse = ucxhAT_RESPONSE_UNKNOWN; \
          } \
          break; \
        }

        LIST_OF_PLUS_URC
      #undef _XMACRO
    } while (0);
  }

  switch (atResponse) {
    case ucxhAT_RESPONSE_PARSED:
      if (ucxhPARSER_onURCParsed != NULL) {
        ucxhPARSER_onURCParsed(numTokens, (const char **)&tokens[0]);
      }
      break;

    case ucxhAT_RESPONSE_OK:
      if (ucxhPARSER_onOkReceived != NULL) {
        ucxhPARSER_onOkReceived(ucxhPARSER_getLastCommand());
      }
      break;

    case ucxhAT_RESPONSE_ERROR:
      if (ucxhPARSER_onErrorReceived != NULL) {
        ucxhPARSER_onErrorReceived(ucxhPARSER_getLastCommand());
      }
      break;

    case ucxhAT_RESPONSE_UNKNOWN:
      if (ucxhPARSER_onUnknownResponseReceived != NULL) {
        ucxhPARSER_onUnknownResponseReceived(line);
      } else {
        #if (UCXH_PARSER_TRACE_AT_PARSER > 1)
          if (ucxhPARSER_onUnknownResponseReceived == NULL) {
            udprintf(UCXH_PARSER_TRACE_AT_PARSER, 2, "ucxh_parser: Got unparsed response '%s'\n", line);
          }
        #endif
      }
      break;

    default:
      assert(0);
  }

  if (atResponse == ucxhAT_RESPONSE_OK || atResponse == ucxhAT_RESPONSE_ERROR) {
    // Invoke callback registered by the parser's enqueuer for this command, which
    // will initiate the sending of the next command, if any.
    if (commandSentDoneCallback != NULL) {
      udprintf(UCXH_PARSER_TRACE_AT_PARSER, 3, "ucxh_parser: Invoking commandSentDoneCallback()...\n", "");
      int (*callback)(bool success, void *userdata) = commandSentDoneCallback;
      void *userdata = commandSentDoneUserdata;
      commandSentDoneCallback = NULL;
      commandSentDoneUserdata = NULL;
      int callback_result = callback((atResponse == ucxhAT_RESPONSE_OK), userdata);
      udprintf(UCXH_PARSER_TRACE_AT_PARSER, 4, "ucxh_parser: commandSentDoneCallback() returned %d...\n", callback_result);
    }
  }

  return atResponse;
}


ucxhAT_response_t atGetLastResponse(void)
{
  return atResponse;
}

void ucxhMODULE_restart(void)
{
  ucxhPARSER_sendCommand("AT+CPWROFF\r");
}

void ucxhMODULE_factoryReset(void)
{
  ucxhPARSER_sendCommand("AT+UFACTORY\r");
  ucxhPARSER_waitForOk(-1);
  ucxhPARSER_sendCommand("AT+CPWROFF\r");
}
