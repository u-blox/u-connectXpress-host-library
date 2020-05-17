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


#ifndef _UCXH_EX_CONFIG_H
#define _UCXH_EX_CONFIG_H

/** 
 * \defgroup exampleconf Example-specific configuration
 * \ingroup configuration
 * \brief Modify these configurations to run the examples and tests.
 * @{
 */



/** SSID of your AP */
#define AP_SSID     "myssid"

/** Password of your WiFi network */
#define AP_PASSWORD "mypassword"

/** IP number which the examples will connect to
 */
#define REMOTE_PEER_IP "192.168.2.1"

/** Port number which the examples will connect to
 */
#define REMOTE_PEER_PORT 12345


/** @} */

#endif