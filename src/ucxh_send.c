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
#include <stdio.h>
#include <string.h>

#include "cb_comdefs.h"

#include "ucxh_config.h"
#include "ucxh_library.h"
#include "ucxh_serial.h"

#include "ucxh_send.h"

#define MAX_COMMAND_LEN 31
#define MAX_COMMAND_PARAM_LEN 11

static char lastCommand[MAX_COMMAND_LEN+1];
static char lastCommandParam[MAX_COMMAND_PARAM_LEN+1];


const char *ucxhPARSER_getLastCommand(void) 
{
  return lastCommand;
}

const char *ucxhPARSER_getLastCommandParam(void) 
{
  return lastCommandParam;
}

static void storeLastCommand(const char *str) 
{
  if (strstr(str, "AT") == str) {
    str += 2;
    const char *eq_pos = strchr(str, '=');
    const char *qm_pos = strchr(str, '?');

    memset(lastCommand, 0x00, sizeof(lastCommand));

    if        (eq_pos == NULL && qm_pos == NULL) { strncpy(lastCommand, str, sizeof(lastCommand)-1);
    } else if (eq_pos != NULL && qm_pos == NULL) { memcpy (lastCommand, str, cb_MIN(sizeof(lastCommand)-1, (eq_pos-str)));
    } else if (eq_pos == NULL && qm_pos != NULL) { memcpy (lastCommand, str, cb_MIN(sizeof(lastCommand)-1, (qm_pos-str)));
    } else if (eq_pos < qm_pos)                  { memcpy (lastCommand, str, cb_MIN(sizeof(lastCommand)-1, (eq_pos-str)));
    } else                                       { memcpy (lastCommand, str, cb_MIN(sizeof(lastCommand)-1, (qm_pos-str)));
    }

    lastCommand[sizeof(lastCommand)-1] = '\0';

    size_t length = strlen(lastCommand);
    while (length > 0 && (lastCommand[length-1] == '\r' || lastCommand[length-1] == '\n')) {
      length --;
    }
    lastCommand[length] = '\0';

    if (eq_pos != NULL) {
      // Prepare parser for the fact that some commands may not generate enough information in their responses to 
      // determine the actual value returned.
      const char *digitpos = eq_pos+1;
      int i = 0;
      while (isdigit(digitpos[i]) && i < sizeof(lastCommandParam)-1) {
        lastCommandParam[i] = digitpos[i];
        i++;
      }
      if(digitpos[i] == '\r' || digitpos[i] == '\n' || digitpos[i] == ',' || digitpos[i] == '\0') {
        lastCommandParam[i] = '\0';
      } else {
        // First parameter is not a number, just ignore.
        lastCommandParam[0] = '\0';
      }
    }
  }
}

/** 
 * \busywaits
 */
static void send(size_t length, const uint8_t *data) 
{
  int bytesWritten = 0;
  
  do {
    int res = ucxhSERIAL_write(data, length);
    if (res >= 0) {
      bytesWritten += res;
    } else if (bytesWritten > 0) {
      udprintf(UCXH_PARSER_TRACE_AT, 1, "send: ucxhSERIAL_write() returned %d\n", res);
      assert(0);  // TODO: What?
    } else {
      // Loop until its possible to send again
    }
  } while (bytesWritten < length);  
}

static void atSendCommandNow(const char *str) 
{
  size_t strLength = strlen(str);
  while (strLength > 0 && (str[strLength-1] == CR || str[strLength-1] == LF)) {
    strLength--;
  };

  udprintf(UCXH_PARSER_TRACE_AT, 2, "ucxhAT_sendCommand: Transmitting '%s", "");
  #if (UCXH_PARSER_TRACE_AT >= 2)
    ucxhPARSER_printBuffer(str, strLength);
  #endif
  udprintf(UCXH_PARSER_TRACE_AT, 2, "%s%s'\n", ucxhPARSER_ascii7ToUnicode(CR), ucxhPARSER_ascii7ToUnicode(LF));
  send(strLength, str);

  const char EOL[] = {CR, LF};
  send(2, EOL);
}

static void edmSendCommandNow(const char *cmd)
{
  uint8_t edmHead[ucxhEDM_REQUEST_HEAD_SIZE] = {0};

  assert(cmd != NULL);

  size_t cmdLength = strlen(cmd);
  while (cmdLength > 0 && (cmd[cmdLength-1] == CR || cmd[cmdLength-1] == LF)) {
    cmdLength--;
  };
  
  #if (UCXH_PARSER_TRACE_AT >= 2)
    udprintf(UCXH_PARSER_TRACE_AT, 2, "ucxhEDM_sendCommand: Transmitting '%s", "");
    ucxhPARSER_printBuffer(cmd, cmdLength);
    udprintf(UCXH_PARSER_TRACE_AT, 2, "%s' ...\n", "");
  #endif

  assert(cmdLength <= ucxhEDM_BUFFER_SIZE);

  size_t headSize = edmZeroCopyHeadRequest(cmdLength+1, edmHead);
  assert(headSize >= 0 && headSize <= ucxhEDM_REQUEST_HEAD_SIZE);
  send(headSize, edmHead);
  
  send(cmdLength, cmd);

  uint8_t edmTail[1+ucxhEDM_TAIL_SIZE] = {CR};

  size_t tailSize = edmZeroCopyTail(edmTail+1);
  assert(tailSize >= 0 && tailSize <= ucxhEDM_TAIL_SIZE);
  send(tailSize+1, edmTail);
}


void ucxhEDM_sendData(unsigned short channel, const uint8_t *data, size_t size)
{
  uint8_t edmHead[ucxhEDM_DATA_HEAD_SIZE] = {0};

  assert(data != NULL);

  udprintf(UCXH_PARSER_TRACE_SEND_DATA, 2, "ucxhEDM_sendData: Transmitting %d bytes", size);
  #if (UCXH_PARSER_TRACE_SEND_DATA == 2)
    udprintf(UCXH_PARSER_TRACE_SEND_DATA, 2, "%s\n", "");
  #endif
  #if (UCXH_PARSER_TRACE_SEND_DATA >= 3)
    udprintf(UCXH_PARSER_TRACE_SEND_DATA, 3, ":'%s", "");
    ucxhPARSER_printBuffer(data, size);
    udprintf(UCXH_PARSER_TRACE_SEND_DATA, 3, "%s' ...\n", "");
  #endif

  assert(size <= ucxhEDM_BUFFER_SIZE);

  int headSize = edmZeroCopyHeadData(channel, size, edmHead);
  assert(headSize >= 0 && headSize <= ucxhEDM_DATA_HEAD_SIZE);
  send(headSize, edmHead);

  send(size, data);

  uint8_t edmTail[ucxhEDM_TAIL_SIZE] = {0x0};

  size_t tailSize = edmZeroCopyTail(edmTail);
  assert(tailSize >= 0 && tailSize <= ucxhEDM_TAIL_SIZE);
  send(tailSize, edmTail);
}

void ucxhDATA_sendData(const uint8_t *data, size_t size)
{
  send(size, data);
}



//
//
//


static void sendCommandWithCallbackNow(const char *cmd, int (*callback)(bool success, void *userdata), void *userdata)
{
  storeLastCommand(cmd);
  ucxhPARSER_setOnCommandSentDone(callback, userdata);
  if(atGetCurrentMode() == ucxhAT_MODE_EDM) {
    edmSendCommandNow(cmd);
  } else {
    atSendCommandNow(cmd);
  }
}


typedef struct ucxhPARSER_enqueuedCommand_t {
  char *command;
  struct ucxhPARSER_enqueuedCommand_t *next;
  // void (*onDone)(const char *command); // TODO:
} ucxhPARSER_enqueuedCommand_t;

static ucxhPARSER_enqueuedCommand_t *atCommandsInProgress = NULL;


#define ucxhPARSER_allocString(s) (s != NULL ? malloc(strlen(s)+1) : NULL)
#define ucxhPARSER_freeString(s) free((void*)s)


#define allocEnqueuedCommand() malloc(sizeof(ucxhPARSER_enqueuedCommand_t))

static void freeEnqueuedCommand(void *v)
{
  ucxhPARSER_enqueuedCommand_t *list = (ucxhPARSER_enqueuedCommand_t *)v;
  udprintf(UCXH_PARSER_TRACE_AT, 4, "freeEnqueuedCommand() invoked for '", "");
  #if (UCXH_PARSER_TRACE_AT >= 4)
      ucxhPARSER_printBuffer(list->command, strlen(list->command));
   #endif
  udprintf(UCXH_PARSER_TRACE_AT, 4, "'\n", "");
  ucxhPARSER_freeString(list->command);
  list->command = NULL;
  list->next = NULL;
  
  free(v);
}

static int onSendEnqueuedCommandDone(bool success, void *userdata)
{
    ucxhPARSER_enqueuedCommand_t *list = userdata;

    assert(userdata != NULL);
    assert(list != NULL);
    assert(list->command != NULL);
    assert(atCommandsInProgress == list);

    udprintf(UCXH_PARSER_TRACE_AT, 3, "onSendEnqueuedCommandDone: %s: for '", success ? "OK" : "ERRROR");
    #if (UCXH_PARSER_TRACE_AT >= 3)
        ucxhPARSER_printBuffer(list->command, strlen(list->command));
    #endif
    udprintf(UCXH_PARSER_TRACE_AT, 3, "´\n", "");

    ucxhPARSER_enqueuedCommand_t *self = list;
    atCommandsInProgress = list->next;
    freeEnqueuedCommand(self);
    if (atCommandsInProgress != NULL) {
      list = atCommandsInProgress;
      udprintf(UCXH_PARSER_TRACE_AT, 3, "onSendEnqueuedCommandDone: But more commands exist, invoking sendCommandWithCallbackNow for  '", "");
      #if (UCXH_PARSER_TRACE_AT >= 3)
        ucxhPARSER_printBuffer(list->command, strlen(list->command));
      #endif
      udprintf(UCXH_PARSER_TRACE_AT, 3, "'\n", "");
      sendCommandWithCallbackNow(list->command, onSendEnqueuedCommandDone, (void *)list);
    } else {
      udprintf(UCXH_PARSER_TRACE_AT, 3, "onSendEnqueuedCommandDone: And that was the last list!\n", "");
    }

    return true;
}

static void enqueueCommand(const char *cmd, const char *file, uint32_t line) 
{
  assert(cmd != NULL);
  udprintf(UCXH_PARSER_TRACE_AT, 3, "enqueueCommand: from %s:%u: '", file, line);
  #if (UCXH_PARSER_TRACE_AT >= 3)
    ucxhPARSER_printBuffer(cmd, strlen(cmd));
  #endif
  udprintf(UCXH_PARSER_TRACE_AT, 3, "'\n", "");

  ucxhPARSER_enqueuedCommand_t *newCommand = allocEnqueuedCommand();
  assert(newCommand != NULL);

  newCommand->next = NULL;
  newCommand->command = ucxhPARSER_allocString(cmd);
  assert(newCommand->command != NULL);
  
  strcpy(newCommand->command, cmd);

  if (atCommandsInProgress == NULL) {
    atCommandsInProgress = newCommand;

    udprintf(UCXH_PARSER_TRACE_AT, 3, "enqueueCommand: Nothing in progress, invoking sendCommandWithCallbackNow\n", "");
    sendCommandWithCallbackNow(newCommand->command, onSendEnqueuedCommandDone, (void *)atCommandsInProgress);
  } else {
    ucxhPARSER_enqueuedCommand_t *inProgress = atCommandsInProgress;
    ucxhPARSER_enqueuedCommand_t **nextPtr = &atCommandsInProgress;
    udprintf(UCXH_PARSER_TRACE_AT, 3, "enqueueCommand: Need to enqueue\n","");
    do {
        assert(inProgress->command != NULL);
        assert(inProgress->command[0] != '\0');

        udprintf(UCXH_PARSER_TRACE_AT, 3, "  after '", "");
        #if (UCXH_PARSER_TRACE_AT >= 3)
          ucxhPARSER_printBuffer(inProgress->command, strlen(inProgress->command));
        #endif
        udprintf(UCXH_PARSER_TRACE_AT, 3, "'\n", "");

        nextPtr = &inProgress->next;
        inProgress = inProgress->next;
    } while (inProgress != NULL);
    assert(nextPtr != NULL);
    assert(*nextPtr == NULL);
    *nextPtr = newCommand;
  }
}

//
//
//

void ucxhPARSER_sendAtCommands(const char *atList[], size_t elements)
{
  assert(atList != NULL);
  assert(elements != 0);

  for (int i = 0; i < elements; i++) {
    enqueueCommand(atList[i], __FILE__, __LINE__);
  }
}

void ucxhAT_sendAtCommands(const char *atList[], size_t elements)
{
  ucxhPARSER_sendAtCommands(atList, elements);
}

void ucxhEDM_sendAtCommands(const char *atList[], size_t elements)
{
  ucxhPARSER_sendAtCommands(atList, elements);
}


void ucxhAT_sendCommand(const char *cmd) 
{
  enqueueCommand(cmd, __FILE__, __LINE__);
}

void ucxhEDM_sendCommand(const char *cmd) 
{
  enqueueCommand(cmd, __FILE__, __LINE__);
}


void _ucxhPARSER_sendCommand(const char *cmd, const char *file, uint32_t line) 
{
  enqueueCommand(cmd, file, line);
}


//
//
//

#define ucxhAT_MAX_CMD_LEN 255
char atCommand[ucxhAT_MAX_CMD_LEN+1];
size_t atCommandPos;
bool atCommandValid;
uint8_t numAtCommandParams;

static bool ucxhAT_makeParam(size_t characters)
{
  if (atCommandValid && 1 + atCommandPos + characters < ucxhAT_MAX_CMD_LEN) {
    if (numAtCommandParams > 0) {
      atCommand[atCommandPos++] = ',';
      atCommand[atCommandPos] = '\0';
    }
    numAtCommandParams++;
    return true;
  } else {
    udprintf(UCXH_PARSER_TRACE_AT, 1, "ucxhAT_makeParam: Command too long: '%s'\n", atCommand);
    atCommandPos = 0;
    atCommand[atCommandPos] = '\0';
    atCommandValid = false;
    return false;
  }
}

bool ucxhPARSER_buildSetCmd(const char *cmd)
{
  assert(cmd != NULL);

  memset(atCommand, 0x00, sizeof(atCommand));

  size_t len = strlen(cmd);
  strcpy(atCommand, "AT");
  strncpy(atCommand+2, cmd, sizeof(atCommand)-2);
  atCommand[sizeof(atCommand)-1] = '\0';
  numAtCommandParams = 0;

  if (len < sizeof(atCommand)-2) {
    atCommandPos = len+2;
    atCommand[atCommandPos++] = '=';
    atCommand[atCommandPos] = '\0';
    atCommandValid = true;

    return true;
  } else {
    udprintf(UCXH_PARSER_TRACE_AT, 1, "ucxhPARSER_buildSetCmd: Command too long: '%s'\n", cmd);
    atCommandValid = false;
    atCommandPos = 0;
    atCommand[atCommandPos] = '\0';

    return false;
  }
}

bool ucxhPARSER_buildActionCmd(const char *cmd)
{
  assert(cmd != NULL);

  memset(atCommand, 0x00, sizeof(atCommand));

  size_t len = strlen(cmd);
  strcpy(atCommand, "AT");
  strncpy(&atCommand[2], cmd, sizeof(atCommand)-2);
  atCommand[sizeof(atCommand)-1] = '\0';
  numAtCommandParams = 0;

  if (len < sizeof(atCommand)-1) {
    atCommandPos = len + 2;
    atCommand[atCommandPos] = '\0';
    atCommandValid = true;

    return true;
  } else {
    udprintf(UCXH_PARSER_TRACE_AT, 1, "ucxhPARSER_buildSetCmd: Command too long: '%s'\n", cmd);
    atCommandValid = false;
    atCommandPos = 0;
    atCommand[atCommandPos] = '\0';

    return false;
  }
}

bool ucxhPARSER_addBool(bool value)
{
  if (!ucxhAT_makeParam(1)) {
    udprintf(UCXH_PARSER_TRACE_AT, 1, "ucxhPARSER_addBool: Unable to make space for %d bytes for value %d\n", 1, value);
    return false;
  }
  
  atCommandPos += sprintf(atCommand+atCommandPos, "%u", (value == false ? 0 : 1));
  return true;
}

bool ucxhPARSER_addSignedInt(int32_t value)
{
  if (!ucxhAT_makeParam(10+1)) {
    udprintf(UCXH_PARSER_TRACE_AT, 1, "ucxhPARSER_addSignedInt: Unable to make space for %d bytes for value %d\n", 10+1, value);
    return false;
  }

  atCommandPos += sprintf(atCommand+atCommandPos, "%d", value);
  return true;
}

bool ucxhPARSER_addUnsignedInt(uint32_t value)
{
  if (!ucxhAT_makeParam(10)) {
    udprintf(UCXH_PARSER_TRACE_AT, 1, "ucxhPARSER_addSignedInt: Unable to make space for %d bytes for value %d\n", 10, value);
    return false;
  }

  atCommandPos += sprintf(atCommand+atCommandPos, "%u", value);
  return true;
}

bool ucxhPARSER_addString(const char *s)
{
  size_t len = 0;
  if (s != NULL) {
    len = strlen(s);
  }
  if (!ucxhAT_makeParam(len+2)) {
    udprintf(UCXH_PARSER_TRACE_AT, 1, "ucxhPARSER_addSignedInt: Unable to make space for %d bytes for string '%s'\n", len+2, s);
    return false;
  }
  
  atCommand[atCommandPos++] = '"';
  if (len > 0) {
    strcpy(atCommand+atCommandPos, s);
  }
  atCommandPos += len;
  atCommand[atCommandPos++] = '"';
  atCommand[atCommandPos] = '\0';
  return true;
}

bool ucxhPARSER_addByteArray(size_t size, const uint8_t *data)
{
  if (!ucxhAT_makeParam(size*2)) {
    udprintf(UCXH_PARSER_TRACE_AT, 1, "ucxhPARSER_addSignedInt: Unable to make space for a ByteArray of %d bytes\n", size*2+1);
    return false;
  }

  if (size > 0) {
    assert(data != NULL);
  }
  for (size_t i = 0; i < size; i++) {
    sprintf(atCommand+atCommandPos, "%02X", data[i]);
    atCommandPos += 2;
  }

  return true;
}

bool ucxhPARSER_addIpV4Address(const char *s)
{
  if (!ucxhAT_makeParam(ucxhAT_ipV4AddrStr_MAX_LEN+1)) {
    udprintf(UCXH_PARSER_TRACE_AT, 1, "ucxhPARSER_addIpV4Address: Unable to make space for an IP v4 address of %d bytes\n", ucxhAT_ipV4AddrStr_MAX_LEN+1, s);
    return false;
  }

  size_t len = strlen(s);

  if (!ucxhPARSER_isIpV4Address(s)) {
    udprintf(UCXH_PARSER_TRACE_AT, 1, "ucxhPARSER_addIpV4Address: '%s' is not an IP v4 Address\n", s);
    atCommandValid = false;
    return false;
  }

  strcpy(atCommand+atCommandPos, s);
  atCommandPos += len;

  return true;
}

bool _ucxhPARSER_sendBuiltCommand(const char *file, uint32_t line) 
{
  if (atCommandValid) {
    _ucxhPARSER_sendCommand(atCommand, file, line);
    atCommandPos = 0;
    atCommandValid = false;
    return true;
  } else {
    udprintf(UCXH_PARSER_TRACE_AT, 1, "ucxhPARSER_sendBuiltCommand: Invalid command '%s'ignored\n", atCommand);
  }
  return false;
}
