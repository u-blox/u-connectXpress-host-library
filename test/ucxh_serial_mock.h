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

#include "errno.h"

typedef enum ucxhSERIAL_writeResult_t {
  ucxhSERIAL_WRITE_RESULT_ERROR = -ENOTSUP,
  ucxhSERIAL_WRITE_RESULT_NONE = 0,
  ucxhSERIAL_WRITE_RESULT_ONE = 1,
  ucxhSERIAL_WRITE_RESULT_ALMOST_ALL = 2,
  ucxhSERIAL_WRITE_RESULT_ALL = 4   // Or whatever fit in the writebuffer
} ucxhSERIAL_writeResult_t;

void ucxhSERIAL_setWriteResult(ucxhSERIAL_writeResult_t result);

/**
 * \brief Define the mocked data to return for ucxhSERIAL_read()
 * \param [in] numChunks Number of chunks in the chunks array
 * \param [in] chunks sequence of chunks of strings (excl NUL) to return for each invocation to ucxhSERIAL_read()
 * \param [in] readResultNoChunks returncode to return if ucxhSERIAL_read() arrives after all chunks has been returned. >0 implies to restart again from the first chunk.
 */
#define ucxhSERIAL_setReadResult(numChunks, chunks, readResultNoChunks) _ucxhSERIAL_setReadResult(numChunks, chunks, readResultNoChunks, __FILE__, __LINE__) 
void _ucxhSERIAL_setReadResult(uint32_t numChunks, const char *chunks[], int readResultNoChunks, const char *file, uint32_t line);

/** Returns true if the desired binary data is first in the write FIFO as result of invokcations of ucxhSERIAL_write()
 */
bool ucxhSERIAL_wasWritten(const char *buffer, size_t length);

/** Returns true if the desired string (excl. NUL) is first in the write FIFO as result of invokcations of ucxhSERIAL_write()
 */
bool ucxhSERIAL_wasStringWritten(const char *s);
void ucxhSERIAL_setWriteBuffer(size_t sz);

#define ucxhSERIAL_setReadResult(numChunks, chunks, readResultNoChunks) _ucxhSERIAL_setReadResult(numChunks, chunks, readResultNoChunks, __FILE__, __LINE__)


/** Trace level of mocked serial port:
 * 0: No trace output
 * 1: Display errors during output to mocked serial port
 * 2: Display sizes of input and output to mocked serial port
 * 3: Also display content of input and output to mocked serial port
 * \ingroup tracingconf
 */
#define UCXH_SERIAL_TRACE_MOCK 1
