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


#ifndef _UCXH_AT_EDM_H
#define _UCXH_AT_EDM_H

#include <unistd.h>

/** Return value for successful execution of a function */
#define ucxhEDM_OK                  0
/** Return value for general failure during execution of a function, not covered by any other return value */
#define ucxhEDM_ERROR               -1

/** Return value for function parameter errors. */
#define ucxhEDM_ERROR_PARAM         -2

/** Return value for invalid header. */
#define ucxhEDM_ERROR_INVALID       -3

/** Return value for incomplete packet. */
#define ucxhEDM_ERROR_INCOMPLETE    -4

/** Return value for invalid size. */
#define ucxhEDM_ERROR_SIZE          -5

/** Return value for corrupted packet. */
#define ucxhEDM_ERROR_CORRUPTED     -6

#define ucxhEDM_BT_ADDRESS_SIZE   6
#define ucxhEDM_IPv4_ADDRESS_SIZE 4
#define ucxhEDM_IPv6_ADDRESS_SIZE 16

/** Overhead in bytes for an AT Request packet */
#define ucxhEDM_REQUEST_OVERHEAD    6

/** Overhead in bytes for data packet */
#define ucxhEDM_DATA_OVERHEAD       7

#define ucxhEDM_REQUEST_HEAD_SIZE   5
#define ucxhEDM_DATA_HEAD_SIZE      6
#define ucxhEDM_TAIL_SIZE           1

/** Largest possible overhead for an EDM packet
 * */
#define ucxhEDM_MAX_OVERHEAD        7

#define ucxhEDM_AT_REQUEST_EXECUTE  0x0D

/** Maximum size of an EDM payload
 */
#define ucxhEDM_MAX_SIZE            0xFFC

/** Minimum size of an EDM packet with no payload
 */
#define ucxhEDM_MIN_SIZE            4

/** Maximum MTU size for an IP packet
 */
#define ucxhEDM_MTU_IP_MAX_SIZE     635

#define ucxhEDM_SOH 0xAA
#define ucxhEDM_EOT 0x55

typedef enum {
    ucxhEDM_BT_PROFILE_SPP,
    ucxhEDM_BT_PROFILE_DUN,
    ucxhEDM_BT_PROFILE_SPS,
    ucxhEDM_BT_PROFILE_INVALID,
} ucxhEDM_btProfile_t;

typedef enum {
    ucxhEDM_IP_PROTOCOL_TCP,
    ucxhEDM_IP_PROTOCOL_UDP,
    ucxhEDM_IP_PROTOCOL_MQTT,
    ucxhEDM_IP_PROTOCOL_INVALID,
} ucxhEDM_ipProtocol_t;

typedef enum {
    ucxhEDM_EVENT_CONNECT_BT,
    ucxhEDM_EVENT_CONNECT_IPv4,
    ucxhEDM_EVENT_CONNECT_IPv6,
    ucxhEDM_EVENT_DISCONNECT,
    ucxhEDM_EVENT_DATA,
    ucxhEDM_EVENT_AT,
    ucxhEDM_EVENT_STARTUP,
    ucxhEDM_EVENT_INVALID,
} ucxhEDM_eventType_t;

typedef unsigned short ucxhEDM_channel_t;

/** Event data for an EDM Connect Event Bluetooth
 */
typedef struct ucxhEDM_connectionEventBT_t {
    ucxhEDM_channel_t hChannel;
    ucxhEDM_btProfile_t profile;
    uint8_t address[ucxhEDM_BT_ADDRESS_SIZE];
    unsigned short framesize;
} ucxhEDM_connectionEventBT_t;

/** Event data for an EDM Connect Event IPv4
 */
typedef struct ucxhEDM_connectionEventIPv4_t {
    ucxhEDM_channel_t hChannel;
    ucxhEDM_ipProtocol_t protocol;
    uint8_t remoteAddress[ucxhEDM_IPv4_ADDRESS_SIZE];
    unsigned short remotePort;
    uint8_t localAddress[ucxhEDM_IPv4_ADDRESS_SIZE];
    unsigned short localPort;
} ucxhEDM_connectionEventIPv4_t;

/** Event data for an EDM Connect Event IPv4
 */
typedef struct ucxhEDM_connectionEventIPv6_t {
    ucxhEDM_channel_t hChannel;
    ucxhEDM_ipProtocol_t protocol;
    uint8_t remoteAddress[ucxhEDM_IPv6_ADDRESS_SIZE];
    uint16_t remotePort;
    uint8_t localAddress[ucxhEDM_IPv6_ADDRESS_SIZE];
    uint16_t localPort;
} ucxhEDM_connectionEventIPv6_t;

/** Event data for an EDM Disconnect Event
 */
typedef struct ucxhEDM_disconnectEvent_t {
    ucxhEDM_channel_t hChannel;
} ucxhEDM_disconnectEvent_t;

/** Event data for an EDM Data Event
 */
typedef struct ucxhEDM_dataEvent_t {
    ucxhEDM_channel_t hChannel;
    unsigned short length;
    /** Points directly into the pInData passed to edmParse() */
    char* pData;
} ucxhEDM_dataEvent_t;

/** Event data for an AT request 
 */
typedef struct ucxhEDM_atEvent_t {
    unsigned short length;
    /** Points directly into the pInData passed to edmParse() */
    char* pData;
} ucxhEDM_atEvent_t;


/** An incoming EDM event, parsed into a C-friendly struct.
 */
 typedef struct ucxhEDM_event_t {
    ucxhEDM_eventType_t type;
    union {
        ucxhEDM_connectionEventBT_t btConnectEvent;
        ucxhEDM_connectionEventIPv4_t ipv4ConnectEvent;
        ucxhEDM_connectionEventIPv6_t ipv6ConnectEvent;
        ucxhEDM_disconnectEvent_t disconnectEvent;
        ucxhEDM_dataEvent_t dataEvent;
        ucxhEDM_atEvent_t atEvent;
    } params;
} ucxhEDM_event_t;

/**
 *
 * @brief Function for parsing binary EDM data into the event structure
 *
 * A complete EDM binary packet must be present in the buffer.
 * To guarantee that a packet can be fit in the buffer,
 * the buffer must be ucxhEDM_MAX_OVERHEAD + ucxhEDM_MAX_SIZE bytes.
 * The first byte must contain the ucxhEDM_SOH, and the last byte of the packet must be ucxhEDM_EOT
 * 
 * @note  Only the first found EDM packet is parsed. So (after handling the event
 *        data output) rerun the function until all data is consumed.<br>
 *        Special case ucxhEDM_ERROR_INCOMPLETE in which case all data has probably not
 *        been received yet.
 *
 * @param[in] pInData Pointer to the binary data to parse
 * @param[in] size Size of the binary data. Must be ucxhEDM_MIN_SIZE bytes or larger.
 * @param[out] pEventData Output structure, needs to be allocated by caller before call.
 *             Only valid if result code is ucxhEDM_OK.
 * @param[out] pConsumed The number of bytes of the input that has been parsed. See function
               description on how to handle.
 *
 * @post If output type is ucxhEDM_EVENT_DATA or ucxhEDM_EVENT_AT the data/command pointer in pEventData
 *       points to memory in the original inData. So inData most still be in scoop when pEventData
 *       is used.
 *
 * @retval ucxhEDM_OK Successful parsing
 * @retval ucxhEDM_ERROR_PARAM inData is NULL or size in invalid
 * @retval ucxhEDM_ERROR_INVALID Not a valid EDM packet
 * @retval ucxhEDM_ERROR_INCOMPLETE Start of a valid EDM packet, but does not contain a full packet
 * @retval ucxhEDM_ERROR_SIZE EDM size header does not match actual data, should never happen
 * @retval ucxhEDM_ERROR_CORRUPTED Invalid value of data inside the EDM packet, should never happen
 */
int edmParse(char* pInData, unsigned int size, ucxhEDM_event_t* pEventData, unsigned int* pConsumed);

/**
 *
 * @brief Function packing an AT command request into an EDM packet
 *
 * @note This function will include a memcpy of size bytes, if there is desire to avoid
 *       this for performance and/or memory reasons use the edmZeroCopy functions instead.
 *
 * @param[in] pAtString Pointer to the AT Command, must end with CR (0x0D).
 * @param[in] atStringSize Size of the AT Command not including terminating character. Must be less
 *             than ucxhEDM_MAX_SIZE.
 * @param[out] pEdmPacket Pointer to a memory where the EDM packet is created. This need to be
 *             an allocated memory area of AT request size + ucxhEDM_REQUEST_OVERHEAD.
 *
 * @retval positive Number of bytes used in the packet memory. Typically size + ucxhEDM_REQUEST_OVERHEAD.
 * @retval ucxhEDM_ERROR_PARAM Input pointer and null or size is to large.
 */
int edmRequest(const char* pAtString, int atStringSize, char* pEdmPacket);

/**
 *
 * @brief Function packing data into an EDM packet
 *
 * @note This function will include a memcpy of size bytes, if there is desire to avoid
 *       this for performance and/or memory reasons use the edmZeroCopy functions instead.
 *
 * @param[in] hChannel The EDM channel the data should be sent on.
 * @param[in] pInData Pointer to the data.
 * @param[in] inDataSize Size of data. Must be less than ucxhEDM_MAX_SIZE.
 * @param[out] pEdmPacket Pointer to a memory where the EDM packet is created. This need to be
 *             an allocated memory area of data size + ucxhEDM_DATA_OVERHEAD
 *
 * @retval positive Number of bytes used in the packet memory. Typically size + ucxhEDM_DATA_OVERHEAD
 * @retval ucxhEDM_ERROR_PARAM Input pointer and null or size is to large.
 */
int edmData(ucxhEDM_channel_t hChannel, const char* pInData, int inDataSize, char* pEdmPacket);

/**
 *
 * @brief Creates an EDM AT request packet header
 *
 * @details This is a way to avoid a memcpy and can e.g. be useful if the packet is sent
 *          over a stream based channel the accepts bytes/chunks. The module will assemble
 *          the input and execute when a full EDM packet is received.<br>
 *          Valid EDM packet: head + AT request + tail.
 *
 * @param[in] atStringSize Size of the AT request.
 * @param[out] pHeadData Pointer to a memory where the EDM packet is created. This need to be
 *             an allocated memory area of ucxhEDM_REQUEST_OVERHEAD.
 *
 * @retval positive Number of bytes used in the head memory. 
 * @retval ucxhEDM_ERROR_PARAM Input pointer and null or size is to large.
 */
int edmZeroCopyHeadRequest(int atStringSize, char* pHeadData);

/**
 *
 * @brief Creates an EDM data packet header
 *
 * @details This is a way to avoid a memcpy and can e.g. be useful if the packet is sent
 *          over a stream based channel the accepts bytes/chunks. The module will assemble
 *          the input and execute when a full EDM packet is received.<br>
 *          Valid EDM packet: head + data + tail.
 *
 * @param[in] hChannel The EDM channel the data should be sent on.
 * @param[in] inDataSize Size of the data.
 * @param[out] pHeadData Pointer to a memory where the EDM packet is created. This need to be
 *             an allocated memory area of ucxhEDM_DATA_OVERHEAD.
 *
 * @retval positive Number of bytes used in the head memory. 
 * @retval ucxhEDM_ERROR_PARAM Input pointer and null or size is to large.
 */
int edmZeroCopyHeadData(ucxhEDM_channel_t hChannel, int inDataSize, char* pHeadData);

/**
 *
 * @brief Creates an EDM data packet tail. Valid for both AT request and data.
 *
 * @details See edmZeroCopyHeadData/edmZeroCopyHeadRequest
 *
 * @param[pTail] tail Pointer to a memory where the EDM packet is created. This need to be
 *             an allocated memory area of ucxhEDM_TAIL_OVERHEAD.
 *
 * @retval positive Number of bytes used in the tail memory. 
 * @retval ucxhEDM_ERROR_PARAM Input pointer is null.
 */
int edmZeroCopyTail(char* pTail);

#endif