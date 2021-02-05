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

/** MQTT client which the MQTT-based examples will use
 * \note This client is non-functional. Replace this client with your own client.
 */
#define MQTT_CLIENT "device:259e42c3-23bc-efab-764d-242596e42c93"

/** MQTT user which the MQTT-based examples will use
 * \note This user is non-functional. Replace this user with your own user.
 */
#define MQTT_USER "29FCABF540B55DC2CE7B"

/** MQTT password which the MQTT-based examples will use.
 * \note This password is non-functional. Replace this password with your own password.
 */
#define MQTT_PASSWORD "B53383F24A6008520DCF4BD22E6B2FA6B4173B36"





/** IP number which the WiFi examples will connect to
 */
#define REMOTE_PEER_IP "192.168.2.1"

/** Port number which the Wi-Fi examples will connect to
 */
#define REMOTE_PEER_PORT 12345



/** IP number which the MQTT-based examples will connect to
 */
#define MQTT_REMOTE_PEER_IP "mqtt.thingstream.io"

/** Port number which the MQTT-based examples will connect to
 */
#define MQTT_REMOTE_PEER_PORT 8883

/** MQTT Publish topic which the MQTT-based examples will use
 */
#define MQTT_PUBLISH "thingstream-subscribe"

/** MQTT password which the MQTT-based examples will use
 */
#define MQTT_SUBSCRIBE "thingstream-publish"

/** Max number of scan results the json file should contain which the ex_wifi_pos example
 *  will use to avoid sending too large amount of data.
 */
#define MAX_WIFI_SCAN_RESSULTS 8


/** @} */

#endif