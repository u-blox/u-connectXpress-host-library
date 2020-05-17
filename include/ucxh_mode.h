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


#ifndef _UCXH_AT_MODE_H
#define _UCXH_AT_MODE_H

/*! 
* \ingroup modulecontrol
*  @{
*/

typedef enum {
    ucxhAT_MODE_UNKNOWN,
    ucxhAT_MODE_DATA,
    ucxhAT_MODE_AT,
    ucxhAT_MODE_EDM
} ucxhAT_mode_t;

bool ucxhMODULE_enterAtMode(void);
bool ucxhMODULE_enterDataMode(void);

bool ucxhMODULE_enterExtendedDataMode(void);

/*! @} */

/*! 
*  @{
* \protected
*/

/** Since it is possible to execute AT commands as part of the extended data
 * mode protocol, it is not necessary to enter command mode when in the
 * extended data mode.
 * 
 * @bug: It is not possible to leave EDM in many firmware versions.
 */ 
bool atLeaveExtendedDataMode(void);

ucxhAT_mode_t atGetCurrentMode(void);
/*! @} */


#endif