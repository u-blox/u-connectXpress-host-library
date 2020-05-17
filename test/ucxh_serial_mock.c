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

#include "ucxh_config.h"

#include "ucxh_serial_mock.h"
#include "ucxh_parser.h"

#include "ucxh_serial.h"

static ucxhSERIAL_writeResult_t writeResult = ucxhSERIAL_WRITE_RESULT_ALL;

void ucxhSERIAL_setWriteResult(ucxhSERIAL_writeResult_t result) 
{
  writeResult = result;
}

#define MAX_WRITTEN_BYTES 65536

uint8_t writtenBytes[MAX_WRITTEN_BYTES];
size_t writtenBytesWritePos = 0;
size_t writtenBytesReadPos = MAX_WRITTEN_BYTES;
size_t writtenBytesSize = MAX_WRITTEN_BYTES;

bool ucxhSERIAL_wasBinaryWritten(const char *buffer, size_t length)
{
  size_t i = 0;
  bool res = true;
  while ((writtenBytesReadPos % writtenBytesSize) != writtenBytesWritePos && i < length) {
    writtenBytesReadPos %= writtenBytesSize;
    if (writtenBytes[writtenBytesReadPos++] != buffer[i++]) {      
      res = false;
      break;
    }
  }

  if (writtenBytesReadPos == writtenBytesWritePos) {
    writtenBytesWritePos = 0;
    writtenBytesReadPos = writtenBytesSize;
  }

  return res;
}

void ucxhSERIAL_setWriteBuffer(size_t sz)
{
  assert (sz < MAX_WRITTEN_BYTES);
  writtenBytesWritePos = 0;
  writtenBytesReadPos = sz;
  writtenBytesSize = sz;
}

bool ucxhSERIAL_wasStringWritten(const char *s)
{
  return ucxhSERIAL_wasBinaryWritten(s, strlen(s));
}

int ucxhSERIAL_write(const uint8_t *buffer, size_t length)
{
  if(buffer == NULL || length == 0) {
    return 0;
  }

  int res = -42;

  switch (writeResult)  {
    ucxhSERIAL_WRITE_RESULT_ERROR:        res = -ENOTSUP; break;
    ucxhSERIAL_WRITE_RESULT_NONE:         res = 0; break;
    ucxhSERIAL_WRITE_RESULT_ONE:          res = 1; break;
    ucxhSERIAL_WRITE_RESULT_ALMOST_ALL:   res = length-1; break;
    ucxhSERIAL_WRITE_RESULT_ALL:          res = length; break;
    default :  res = length; break;
  }

  if (writtenBytesReadPos == writtenBytesWritePos) {
    res = 0;
  }

  int i = 0;
  if (res > 0) {
    while (writtenBytesReadPos != (writtenBytesWritePos % writtenBytesSize) && i < res) {
      writtenBytesWritePos %= writtenBytesSize;
      writtenBytes[writtenBytesWritePos++] = buffer[i++];
    }
  } else {
    i = res;
  }

    udprintf(UCXH_SERIAL_TRACE_MOCK, 2, "(mocked) ucxhSERIAL_write request to write %d bytes, returning %d since writeResult=%d and res=%d", length, i, writeResult, res);
  #if (UCXH_SERIAL_TRACE_MOCK>=3)
    udprintf(UCXH_SERIAL_TRACE_MOCK, 3, ": '", "");
    if (i > 0) {
      ucxhPARSER_printBuffer(buffer, i);
    }
    udprintf(UCXH_SERIAL_TRACE_MOCK, 3, "'\n", "");
  #else
    udprintf(UCXH_SERIAL_TRACE_MOCK, 2, "\n", "");
  #endif

  return res;
}

static const char **ucxhSERIAL_readResultChunks = NULL;
static uint32_t ucxhSERIAL_readResultNumChunks = 0;
static int ucxhSERIAL_readResultNoChunks = 0;
static int ucxhSERIAL_readResultCurrentChunk = 0;
static int ucxhSERIAL_readResultChunksStartPos = 0;
static const char *ucxhSERIAL_readResultFile = __FILE__;
static uint32_t ucxhSERIAL_readResultLine = 0;

void _ucxhSERIAL_setReadResult(uint32_t numChunks, const char *chunks[], int readResultNoChunks, const char *file, uint32_t line)
{
  ucxhSERIAL_readResultNumChunks = numChunks;
  ucxhSERIAL_readResultChunks = chunks;
  ucxhSERIAL_readResultNoChunks = readResultNoChunks;
  ucxhSERIAL_readResultCurrentChunk = 0;
  ucxhSERIAL_readResultChunksStartPos = 0;
  ucxhSERIAL_readResultFile = file;
  ucxhSERIAL_readResultLine = line;
}


int ucxhSERIAL_read(uint8_t *outbuffer, size_t bufsize)
{
  if (outbuffer == NULL || bufsize < 1) {
    return -ENOMEM;
  }
  static uint32_t printed0s = 0;

  assert (ucxhSERIAL_readResultChunks != NULL);
  assert (ucxhSERIAL_readResultNumChunks != 0);
  
  if (ucxhSERIAL_readResultCurrentChunk == ucxhSERIAL_readResultNumChunks) {
    if (ucxhSERIAL_readResultNoChunks > 0) {
      ucxhSERIAL_readResultCurrentChunk = 0;
      ucxhSERIAL_readResultChunksStartPos = 0;
    } else {
      if (printed0s == 0) {
        udprintf(UCXH_SERIAL_TRACE_MOCK, 2, "(mocked) ucxhSERIAL_read: returning %d\n", ucxhSERIAL_readResultNoChunks);
      }
      printed0s++;
      return ucxhSERIAL_readResultNoChunks;
    }
  }

  if (printed0s > 1) {
    udprintf(UCXH_SERIAL_TRACE_MOCK, 2, "(mocked) ucxhSERIAL_read: Totally %u invocations skipped\n", printed0s);
    printed0s = 0;
  }

  if (ucxhSERIAL_readResultChunks[ucxhSERIAL_readResultCurrentChunk] == NULL) {
    ucxhSERIAL_readResultCurrentChunk++;
    ucxhSERIAL_readResultChunksStartPos = 0;
    udprintf(UCXH_SERIAL_TRACE_MOCK, 2, "(mocked) ucxhSERIAL_read: returning 0\n", "");
    return 0;
  }

  const uint8_t *chunk = ucxhSERIAL_readResultChunks[ucxhSERIAL_readResultCurrentChunk];
  assert(ucxhSERIAL_readResultChunksStartPos <= strlen(chunk));
  const uint8_t *src = &chunk[ucxhSERIAL_readResultChunksStartPos];
  size_t len = strlen(src);
  if(len > bufsize) {
    len = bufsize;
    ucxhSERIAL_readResultChunksStartPos = len;
  } else {
    ucxhSERIAL_readResultChunksStartPos = 0;
    ucxhSERIAL_readResultCurrentChunk++;
  }
  memcpy(outbuffer, src, len);
    udprintf(UCXH_SERIAL_TRACE_MOCK, 2, "(mocked) ucxhSERIAL_read: returning %d bytes", len);
  #if (UCXH_SERIAL_TRACE_MOCK>=3)
    udprintf(UCXH_SERIAL_TRACE_MOCK, 3, ": '", "");
    ucxhPARSER_printBuffer(outbuffer, len);
    udprintf(UCXH_SERIAL_TRACE_MOCK, 3, "'\n", "");
  #else
    udprintf(UCXH_SERIAL_TRACE_MOCK, 2, "\n", "");
  #endif

  return len;
}

bool ucxhSERIAL_open(void)
{
  ucxhSERIAL_readResultCurrentChunk = 0;
  ucxhSERIAL_readResultChunksStartPos = 0;

  return true;
}

void ucxhSERIAL_close(void)
{
    // NOTHING
}


int ucxhSERIAL_print(const char *s)
{
  size_t strLength = strlen(s);
  int bytesWritten = ucxhSERIAL_write(s, strLength);
  return bytesWritten;
}
