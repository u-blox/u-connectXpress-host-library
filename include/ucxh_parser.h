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
\file
*/


#ifndef _UCXH_AT_PARSER_H
#define _UCXH_AT_PARSER_H

#include <inttypes.h>
#include <stdbool.h> 
#include <time.h>

#include "ucxh_edm.h"
#include "ucxh_poll.h"
#include "ucxh_wait.h"
#include "ucxh_wifi.h"

/*! 
* \ingroup internal
*  @{
*/

/** Line delimiter for AT-commands in AT mode */
#define ucxhAT_LINE_DELIMITER "\r\n"
/** Line delimiter for AT-commands in Extended Data Node (EDM) */
#define ucxhEDM_LINE_DELIMITER "\r"

#ifndef ucxtAT_MAX_TOKENS
  /** The maximum numer of parameters allowed to be received for an echo or URC.
   * @def ucxtAT_MAX_TOKENS
   */
  #define ucxtAT_MAX_TOKENS 20
#endif


#ifndef ucxhAT_CMD_MAX_LEN
  /** Max length of an AT command.
   * No AT command string in this example exceeds 255 characters.
   * The maximum length of AT commands understood by the module,
   * depends on the module's firmware.
   */
  #define ucxhAT_CMD_MAX_LEN 255
#endif

#ifndef ucxhAT_RESPONSE_MAX_LEN
  /** Max length of a line containing an URC or AT response.
   * The maximum length of URCs and responses sent,
   * depends on the module's firmware.
   */
  #define ucxhAT_RESPONSE_MAX_LEN 255
#endif



/** 
 * Returns the next line from the serial port, excluding any leading LF, without any trailing CR or CR LF
 * @returns NULL if the serial buffer is empty.
 * @param [out] atMaxBufferSize Returns the total number of bytes in the serial buffer. May vary
 * from invocation to invocation, depending on how many lines that have been read into the buffer.
 * \bug Will treat a data in the serial buffer as a complete line, even if it doesn't end in CR or CR LF
 * \protected
 */
char *atReadLine(size_t *atMaxBufferSize);

/** Parse a line containing an AT OK/ERROR response, URC or raw data, and invokes the appropriate callback.
 * @param [in] line Line such as 'OK\<CR\>', 'ERROR\<CR\>', '+UUNU:0,12345\<CR\>', '"u-blox"\<CR\>'.
 * \protected
 */
ucxhAT_response_t atParseResponse(const char *line);

/** 
 * \protected
 */
ucxhAT_response_t atGetLastResponse(void);

/** 
 * \protected
 */
void ucxhPARSER_setOnCommandSentDone(int (*callback)(bool success, void *userdata), void *userdata);

/*! @} */


///////////////////////////////////////////////////////////////////////////////


/*! 
* \ingroup utility
*  @{
*/

/**
 * \def PRINTBUFFER_AS_HEX
 * If defined, ucxhPARSER_printBuffer() will output a comma-separated hex sequence instead of printable.
 */
 //#define PRINTBUFFER_AS_HEX

/** Print a binary buffer as characters, with some common characters (CR/LF/TAB) printed as unicode.
 * Unprintable characters are printed as unicode.
 * Suitable for debug printouts of buffers not NUL-terminated.
 * OR: If PRINTBUFFER_AS_HEX, all output is a comma-separated hex sequence.
 * Does not add any CR
 * @param buffer Binary buffer
 * @param length Number of bytes in buffer
 */
void ucxhPARSER_printBuffer(const uint8_t *buffer, size_t length);

/** Convert !isprint() character to unicode symbols.
 * @param c Latin-1 ISO 8851-15 character
 * @return Output string, which may be static if isprint(c) and thus may be destroyed upon next invocation
 */
const char *ucxhPARSER_ascii7ToUnicode(uint8_t c);

#ifndef MIN
#define MIN(a,b) ((a) <= (b) ? (a):(b))
#endif
#ifndef min
#define min(a,b) ((a) <= (b) ? (a):(b))
#endif
#ifndef MAX
#define MAX(a,b) ((a) >= (b) ? (a):(b))
#endif
#ifndef max
#define max(a,b) ((a) >= (b) ? (a):(b))
#endif


/** Copies the `src` NUL-terminated string to the `dst` array (which must not be a pointer to an array).
 * Any surrounding ""-characters will be removed.
 * The `ok` variable, will be set to TRUE, if successful.
 * If failed, `ok` will be set to FALSE, and `dst` will be unchanged.
 * Needs to be a macro, since sizeof(dst) is taken.
 * \sa STRCPY2ARRAYORFAIL()
 * \sa STRCPY2INT()
 */
#define STRCPY2ARRAY(dst, src, ok) \
do { \
    const char *_src = &src[0]; \
    size_t length = strlen(src); \
    if (_src[0] == '\"') { \
        _src++; \
        length--; \
    } \
    if (length < sizeof(dst)) { \
        strncpy(dst, _src, sizeof(dst)-1);  \
        dst[sizeof(dst)-1] = '\0'; \
        size_t length = strlen(dst); \
        if (length >= 1 && dst[length-1] == '\"') { \
            dst[length-1] = '\0'; \
        } \
        *(ok) = true; \
    } else { \
        *(ok) = false; \
    } \
} while(0)

/** Copies the `src` NUL-terminated string to the `dst` array (which must not be a pointer to an array).
 * If failed, `dst` will be unchanged, an error will be logged with `name` as the unparsable parameter, 
 * and the function caller will return ucxhURC_PARSE_RESULT_ERROR.
 * Needs to be a macro, since sizeof(dst) is taken.
 * This is syntax sugar.
 * \sa STRCPY2ARRAY()
 */
#define STRCPY2ARRAYORFAIL(dst, src) \
do { \
    bool _success = false; \
    STRCPY2ARRAY(dst, src, &_success); \
    if (!_success) { \
        udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (%s) Unparsable parameter '%s': '%s'\n", \
            ppParams[0], \
            ppParams[0], \
            #dst, \
            src); \
        return ucxhURC_PARSE_RESULT_ERROR; \
    } \
} while (0)

/** Copies the value in the `src` NUL-terminated string to the int32_t `dst` variable (which must not be a pointer).
 * The `ok` variable, will be set to TRUE, if successful.
 * If failed, `ok` will be set to FALSE, and `dst` will be unchanged.
 * \sa STRCPY2INTORFAIL()
 * \sa STRCPY2ARRAY()
 */
#define STRCPY2INT(dst, src, ok) \
do { \
    int32_t _dst = 0; \
    *(ok) = (bool) (sscanf(src, "%" SCNd32, &_dst) == 1); \
    if (*(ok)) { \
        dst = _dst; \
    } \
} while(0)

/** Copies the value in the `src` NUL-terminated string to the int32_t `dst` variable (which must not be a pointer).
 * If failed, `dst` will be unchanged, an error will be logged with `name` as the unparsable parameter, 
 * and the function caller will return ucxhURC_PARSE_RESULT_ERROR.
 * This is syntax sugar.
 * \sa STRCPY2INT()
 */
#define STRCPY2INTORFAIL(dst, src) \
do { \
    bool _success = false; \
    STRCPY2INT(dst, src, &_success); \
    if (!_success) { \
        udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (%s) Unparsable parameter '%s': '%s'\n", \
            ppParams[0], \
            ppParams[0], \
            #dst, \
            src); \
        return ucxhURC_PARSE_RESULT_ERROR; \
    } \
} while (0)


/** Check the validity of the parameters in the `ppParams` array.
 * Optionally, each parameter is printed.
 * * The number of parameters `numParams` must be at least `minParams`.
 *   If not, the function will return `ucxhURC_PARSE_RESULT_ERROR`.
 * * assert() will be invoked in case any of the `ppParams` are NULL,
 *   and a debug print will be output to `stdout`.
 */ 
#define ASSERTPARAMS(numParams, ppParams, minParams) do { \
    assert(numParams > 0); \
    assert(ppParams != NULL); \
    assert(ppParams[0] != NULL); \
    ucxhPARSER_checkParams(numParams, ppParams, __FILE__, __LINE__); \
    if (numParams < minParams) { \
        udprintf(UCXH_PARSER_TRACE_URC, 1, "%s Too few parameters (%d), expected at least %d\n", \
                    (ppParams[0] != NULL) ? ppParams[0] : "(unknown URC):", \
                    numParams, minParams); \
        return ucxhURC_PARSE_RESULT_ERROR;\
    } \
} while (0)

/** Optionally, each parameter is printed.
 * assert() will be invoked in case any pointer in `ppParams` is NULL, 
 * and a debug print will be output to `stdout`.
 */
void ucxhPARSER_checkParams(uint8_t numParams, const char *ppParams[], const char *file, uint32_t line);


/*! @} */


///////////////////////////////////////////////////////////////////////////////


/**
 *  \ingroup parsercallbacks
 *  @{
 */

/** Register a catch-all callback to invoke upon reception of a recognized URC.
 * The parser may have taken actions before the callback has been invoked, such as issuing 
 * additional AT-commands.
 * 
 * The callback is invoked independent of if the URC is received in AT mode or through the EDM AT channel.
 * 
 * When the incoming URC is not known, the callback registered using ucxhPARSER_setOnUnknownURCReceived() will be invoked instead.
 * @param [in] callback Callback to invoked upon reception of event.
 * @returns false if registration failed, otherwize true.
 * @retval callback The callback shall return 0 if sucessful, < 0 if failed during processing.
 * \sa ucxhPARSER_setOnOkReceived
 * \sa ucxhPARSER_setOnErrorReceived
 */
bool ucxhPARSER_setOnURCParsed(int (*callback)(uint8_t numParams, const char *ppParams[]));

/** Register callback to invoke upon reception of something recognized as an URC.
 * The parser may have taken actions before the callback has been invoked, such as issuing 
 * additional AT-commands.
 * 
 * The callback is invoked independent of if the unknown data is received in AT mode or through the EDM AT channel.
 * 
 * When the incoming URC is known, the callback registered using ucxhPARSER_setOnURCParsed() will be invoked instead.
 * @param [in] callback Callback to invoked upon reception unknown URC.
 * @param [in] line The actual line received
 * @returns false if registration failed, otherwize true.
 * @retval callback The callback shall return 0 if sucessful, < 0 if failed during processing.
 * \sa ucxhPARSER_setOnOkReceived
 * \sa ucxhPARSER_setOnErrorReceived
 */
bool ucxhPARSER_setOnUnknownURCReceived(int (*callback)(const char *line));

/** Register callback to invoke upon reception of an OK response to an AT command.
 * The parser may have taken actions before the callback has been invoked, such as issuing 
 * additional AT-commands.
 * 
 * The callback is invoked independent of if the unknown data is received in AT mode or through the EDM AT channel.
 * 
 * Depending on the AT command previously issued, there may have been URCs generated prior to, or after the
 * OK response as well. Such URCs will have caused the one of callback registered using ucxhPARSER_setOnURCParsed()
 * or ucxhPARSER_setOnUnknownURCReceived().
 * @param [in] callback Callback to invoked upon reception of OK.
 * @returns false if registration failed, otherwize true.
 * @retval callback The callback shall return 0 if sucessful, < 0 if failed during processing.
 * \sa ucxhPARSER_setOnErrorReceived
 */
bool ucxhPARSER_setOnOkReceived(int (*callback)(const char *lastcommand));

/** Register callback to invoke upon reception of an ERROR response to an AT command.
 * The callback is invoked independent of if the unknown data is received in AT mode or through the EDM AT channel.
 * 
 * @param [in] callback Callback to invoked upon reception of ERROR.
 * @returns false if registration failed, otherwize true.
 * @retval callback The callback shall return 0 if sucessful, < 0 if failed during processing.
 * \sa ucxhPARSER_setOnOkReceived
 */
bool ucxhPARSER_setOnErrorReceived(int (*callback)(const char *lastcommand));


/*! @} */


///////////////////////////////////////////////////////////////////////////////


/*! 
* \ingroup modulecontrol
*  @{
*/

/** 
 * Restarts the module
 */
void ucxhMODULE_restart(void);

/** 
 * Reverts the module to its factory default, and restarts it.
 * \busywaits
 * \polls ucxhPARSER_waitForOk()
 */
void ucxhMODULE_factoryReset(void);


/*! @} */


///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////

#endif
