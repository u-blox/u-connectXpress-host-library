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


#ifndef _CB_STATUS_H_
#define _CB_STATUS_H_

#include "cb_comdefs.h"

/*! 
 *  @{
 * \libfile Internal defines.
 * \libdefs
*/


/*===========================================================================
 * DEFINES
 *=========================================================================*/

#define OK(status) (status == cbSTATUS_OK)
#define BUSY(status) (status == cbSTATUS_BUSY)
#define ERR(status) (status == cbSTATUS_ERROR)

/*===========================================================================
 * TYPES
 *=========================================================================*/

 typedef enum
 {
    cbSTATUS_OK,
    cbSTATUS_ERROR,
    cbSTATUS_BUSY,
    cbSTATUS_RECEIVE_DATA_MODE,
    cbSTATUS_TIMEOUT
 
 } cbRTSL_Status;


/*! @} */

#endif /* _CB_STATUS_H_ */

