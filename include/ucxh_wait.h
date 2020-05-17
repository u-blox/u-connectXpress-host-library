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


#ifndef _UCXH_AT_WAIT_H
#define _UCXH_AT_WAIT_H

#ifndef ucxhAT_INBUF_SIZE
  /** Default buffer size
   * @def ucxhAT_INBUF_SIZE
   */
  #define ucxhAT_INBUF_SIZE  2048
#endif  

/** Default timeout to OK response
 */
#define ucxhAT_DEFAULT_AT_TIMEOUT 2

typedef enum {
  ucxhAT_RESPONSE_OK = 0,
  ucxhAT_RESPONSE_ERROR = 1,
  ucxhAT_RESPONSE_STARTUP = 2,

  ucxhAT_RESPONSE_PARSED = 3,
  ucxhAT_RESPONSE_UNKNOWN = 4
} ucxhAT_response_t;

/*! 
* \ingroup busywaits
*  @{
*/

/** Wait and handle data, URC or AT OK/ERROR responses in AT or DATA mode for a specific time.
 * Callbacks will be invoked as required.
 * Use ucxhPARSER_wait() instead. 
 * \protected
 * \busywaits
 * \polls ucxhAT_poll()
 * @param numSeconds number of seconds to wait.
 * @return true if something was received during waiting.
 */
bool ucxhAT_wait(int32_t numSeconds);

/** Wait and handle data, URC or AT OK/ERROR responses in Extended Data Mode (EDM) for a specific time.
 * Callbacks will be invoked as required.
 * Use ucxhPARSER_wait() instead. 
 * \protected
 * \busywaits
 * \polls ucxhEDM_poll()
 * @param numSeconds number of seconds to wait.
 * @return true if something was received during waiting.
 */
bool ucxhEDM_wait(int32_t numSeconds);

/** Wait and handle data, URC or AT OK/ERROR responses for a specific time.
 * Callbacks will be invoked as required.
 * This is the preferred function to wait for data, URC and AT OK/ERROR responses.
 * \busywaits
 * \polls ucxhPARSER_poll()
 * @param numSeconds number of seconds to wait.
 * @return true if something was received during waiting.
 */
bool ucxhPARSER_wait(int32_t numSeconds);
ucxhAT_response_t ucxhAT_waitForOk(int32_t numSeconds);
ucxhAT_response_t ucxhEDM_waitForOk(int32_t numSeconds);

/** Wait for OK up to a timeout.
 * Callbacks will be invoked as required.
 * This is the preferred function to wait for OK.
 * This function aborts once OK has been received.
 * If OK is not detected within the desired timeout, this function returns false.
 * \busywaits
 * \polls ucxhPARSER_poll()
 * @param numSeconds number of seconds to wait.
 * @return true if something was received during waiting.
 */
ucxhAT_response_t ucxhPARSER_waitForOk(int32_t numSeconds);

/*! @} */

#endif
