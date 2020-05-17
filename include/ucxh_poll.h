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


#ifndef _UCXH_AT_POLL_H
#define _UCXH_AT_POLL_H

#include <stdbool.h> 
#include <time.h>

/*! 
* \ingroup receiving
*  @{
*/


/** Polls the ports for incoming data and invokes the appropriate callbacks.
 * \warning Callbacks may NOT invoke this method, recursion might happen!
 *  @return true if something was received.
 */
bool ucxhAT_poll(void);

/** Polls the ports for incoming data and invokes the appropriate callbacks.
 * \warning Callbacks may NOT invoke this method, recursion might happen!
 *  @return true if something was received.
 */
bool ucxhEDM_poll(void);

/** Polls the ports for incoming data and invokes the appropriate callbacks.
 * \warning Callbacks may NOT invoke this method, recursion might happen!
 *  @return true if something was received.
 */
bool ucxhPARSER_poll(void);


/*! @} */

/** Register callback to invoke upon reception of data.
 * The callback is invoked independent of if the data is received in DATA mode or through the an EDM Data channel
 * 
 * @param [in] callback Callback to invoked upon reception of data.
 * @param [in] channel The EDM Data channel, or -1 if data was received in DATA mode.
 * @returns false if registration failed, otherwize true.
 * @retval callback The callback shall return 0 if sucessful, < 0 if failed during processing.
 */
bool ucxhPARSER_setOnDataReceived(int (*callback)(int channel, const uint8_t *buffer, size_t size));

extern int (*ucxhPARSER_onDataReceived)(int channel, const uint8_t *data, size_t size);


#endif