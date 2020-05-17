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


#ifndef _UCXH_AT_EDM_PARSER_H
#define _UCXH_AT_EDM_PARSER_H

#include <time.h>

#include "ucxh_parser.h"
#include "ucxh_poll.h"
#include "ucxh_edm.h"


/**
 * \protected
 */
#define MAX_AT_CMD_OVER_EDM_SIZE (ucxhAT_CMD_MAX_LEN + ucxhEDM_REQUEST_OVERHEAD)

/** Buffer size for incoming EDM events
 * Can typically be much smaller, typically MTU+ucxhEDM_MAX_OVERHEAD bytes
 * For IP, the MTU size is ucxhEDM_MTU_IP_MAX_SIZE bytes
 * For Bluetooth, the MTU size is ucxhEDM_MTU_BT_MAX_SIZE bytes
 */
#define ucxhEDM_BUFFER_SIZE (ucxhEDM_MAX_SIZE+ucxhEDM_MAX_OVERHEAD)


///////////////////////////////////////////////////////////////////////////////


/** Transmit an AT command to the module in EDM mode.
 * \attention This function requires the module to be in EDM mode. 
 * If not, invoke ucxhMODULE_enterExtendedDataMode() first.
 * \note Avoid using this method directly. Use ucxhPARSER_sendAtCommand() instead, since the latter
 * will use AT mode in case EDM is not activated.
 * \protected
 * \ingroup edmsend
 * @param [in] cmd Command to transmit, e.g. "ATI9\r"
 * \sa ucxhAT_sendCommands()
 * \sa ucxhEDM_sendCommand()
 */
void ucxhEDM_sendCommand(const char *cmd);


/** Send binary data over an EDM data channel.
 * The channel is obtained from the various connection events.
 * Register an appropriate callback to obtain the channel.
 * @param [in] channel The EDM data channel to use. The channel is obtained from the ucxhEDM_setOnConnectXXX() callbacks previously registered.
 * @param [in] data Buffer containing the binary data to send.
 * @param [in] length Number of bytes.
 * \ingroup edmsend
 * \sa ucxhEDM_setOnConnectIpV4Received()
 * \sa ucxhEDM_setOnConnectIpV6Received()
 * \sa ucxhEDM_setOnConnectBTReceived()
 */
void ucxhEDM_sendData(unsigned short channel, const uint8_t *data, size_t length);



///////////////////////////////////////////////////////////////////////////////


/*! 
* \ingroup edmcallbacks
*  @{
*/

/** Register callback to invoke upon reception of the EDM Start Event from the module.
 * Depending on the configuration, this event may be sent to inform the host that the module is started.
 * @param [in] callback Callback to invoked upon reception of event.
 * @returns false if registration failed, otherwize true.
 * @retval callback The callback shall return 0 if sucessful, < 0 if failed during processing.
 */
bool ucxhEDM_setOnStartupReceived(int (*callback)(void));

/** Register callback to invoke upon reception of the EDM Connect Event IPv4 from the module.
 * The channel is used as the identifier of the connection and it is unique for each active connection.
 * The callback receives an ucxhEDM_connectionEventIPv4_t as eventData, in which 
 * the protocol field is used to identify the IP protocol that is associated with the connection.
 *  + 0: TCP
 *  + 1: UDP
 * 
 * When the connection is closed or disconnected from the remote peer,
 * the callback registered with ucxhEDM_setOnDisconnectReceived() is invoked with this connection's
 * channel as parameter in the ucxhEDM_disconnectEvent_t .
 * @param [in] callback Callback to invoked upon reception of event.
 * @param [in] eventData EDM Connect Event IPv4 passed to the callback.
 * @returns false if registration failed, otherwize true.
 * @retval callback The callback shall return 0 if sucessful, < 0 if failed during processing.
 */
bool ucxhEDM_setOnConnectIpV4Received(int (*callback)(const ucxhEDM_connectionEventIPv4_t *eventData));

/** Register callback to invoke upon reception of the EDM Connect Event IPv6 from the module.
 * The channel is used as the identifier of the connection and it is unique for each active connection.
 * The callback receives an ucxhEDM_connectionEventIPv6_t as eventData, in which 
 * the protocol field is used to identify the IP protocol that is associated with the connection.
 *  + 0: TCP
 *  + 1: UDP
 * 
 * When the connection is closed or disconnected from the remote peer,
 * the callback registered with ucxhEDM_setOnDisconnectReceived() is invoked with this connection's
 * channel as parameter in the ucxhEDM_disconnectEvent_t .
 * @param [in] callback Callback to invoked upon reception of event.
 * @param [in] eventData EDM Connect Event IPv6 passed to the callback.
 * @returns false if registration failed, otherwize true.
 * @retval callback The callback shall return 0 if sucessful, < 0 if failed during processing.
 */
bool ucxhEDM_setOnConnectIpV6Received(int (*callback)(const ucxhEDM_connectionEventIPv6_t *eventData));

/** Register callback to invoke upon reception of the EDM Connect Event Bluetooth from the module.
 * The channel is used as the identifier of the connection and it is unique for each active connection.
 * The callback receives an ucxhEDM_connectionEventBT_t as eventData, in which 
 * the profile is used to identify what Bluetooth profile is associated with the connection.
 * 
 *  + SPP = 0
 *  + DUN = 1
 *  + Serial Port Service Bluetooth Low Energy = 14
 * 
 * The Bluetooth address is the address of the remote device that is now connected and the frame
 * size is the maximum data size allowed to be transmitted in a data command or data event packet.
 * When the connection is closed or disconnected from the remote peer,
 * the callback registered with ucxhEDM_setOnDisconnectReceived() is invoked with this connection's
 * channel as parameter in the ucxhEDM_disconnectEvent_t .
 * @param [in] callback Callback to invoked upon reception of event.
 * @param [in] eventData EDM Connect Event Bluetooth passed to the callback.
 * @returns false if registration failed, otherwize true.
 * @retval callback The callback shall return 0 if sucessful, < 0 if failed during processing.
 */
bool ucxhEDM_setOnConnectBTReceived(int (*callback)(const ucxhEDM_connectionEventBT_t *eventData));

/** Register callback to invoke upon reception of the EDM Disconnect Event from the module.
 * When a previously set up connection is terminated, the module sends the disconnect event.
 * @param [in] callback Callback to invoked upon reception of event.
 * @param [in] eventData EDM Disconnect Event passed to the callback.
 * @returns false if registration failed, otherwize true.
 * @retval callback The callback shall return 0 if sucessful, < 0 if failed during processing.
 */
bool ucxhEDM_setOnDisconnectReceived(int (*callback)(const ucxhEDM_disconnectEvent_t *eventData));


/*! @} */


///////////////////////////////////////////////////////////////////////////////


/*! 
* \ingroup internal
*  @{
*/


/** Invokes the appropriate callback depending on the event type, or invokes the AT parser for AT Response.
 * Register the callbacks to invoke, using the appropriate function.
 * \sa ucxhEDM_setOnStartupReceived()
 * \sa ucxhEDM_setOnConnectIpV4Received()
 * \sa ucxhEDM_setOnConnectIpV6Received()
 * \sa ucxhEDM_setOnConnectBTReceived()
 * \sa ucxhEDM_setOnDisconnectReceived()
 * \protected
 */
 void atEdmParsePayloadEvent(const ucxhEDM_event_t* eventData, size_t atMaxBufferSize);

/** Attempt to generate an ucxhEDM_event_t from an incoming packet
 * If the inData buffer does not contain a complete packet, readMore() will be invoked.
 * \busywaits
 * \polls readMore()
 * \protected
 * @param [in] readMore callback invoked if the inData buffer does not contain a complete packet
 * @param [in] inData buffer containing an incoming (complete or partial) EDM packet to parse
 * @param [in] bufSize number of valid bytes in the inData buffer
 * @param [out] eventData The event to generate.
 * \returns number of bytes consumed in the inbuffer when an event has been generated, 0 is more data is required, < 0 in case of error
 */
int atEdmReadPacket(int (*readMore)(uint8_t*, size_t), uint8_t* inData, size_t bufSize, ucxhEDM_event_t* eventData);

/**
 * \protected
 */
ucxhAT_response_t atEdmGetLastResponse(void);


/*! @} */


///////////////////////////////////////////////////////////////////////////////


#endif
