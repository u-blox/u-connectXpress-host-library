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


#include <stdlib.h>
#include <string.h>

#include "ucxh_edm.h"

#define ucxhEDM_MIN_SIZE                4
#define ucxhEDM_HEAD_SIZE               1
#define ucxhEDM_TAIL_SIZE               1
#define ucxhEDM_SIZE_SIZE               2 //!< Size of the Size field in an EDM packet.
#define ucxhEDM_HEAD                    0xAA
#define ucxhEDM_TAIL                    0x55
#define ucxhEDM_SIZE_FILTER             0x0F

#define ucxhEDM_TYPE_CONNECT_EVENT      0x11
#define ucxhEDM_TYPE_DISCONNECT_EVENT   0x21
#define ucxhEDM_TYPE_DATA_EVENT         0x31
#define ucxhEDM_TYPE_DATA_COMMAND       0x36
#define ucxhEDM_TYPE_AT_REQUEST         0x44
#define ucxhEDM_TYPE_AT_RESPONSE        0x45
#define ucxhEDM_TYPE_AT_EVENT           0x41
#define ucxhEDM_TYPE_RESEND_CONNECT     0x56
#define ucxhEDM_TYPE_START_EVENT        0x71

#define ucxhEDM_CONNECTION_TYPE_BT      0x01
#define ucxhEDM_CONNECTION_TYPE_IPv4    0x02
#define ucxhEDM_CONNECTION_TYPE_IPv6    0x03

#ifndef NULL
#define NULL ((void *) 0)
#endif

int validEdmPacket(char* pData, int size);

static ucxhEDM_eventType_t findEventType(char evt, char connectionType)
{
    ucxhEDM_eventType_t type;

    switch (evt) {
        case ucxhEDM_TYPE_CONNECT_EVENT:
            if (connectionType == ucxhEDM_CONNECTION_TYPE_BT) {
                type = ucxhEDM_EVENT_CONNECT_BT;
            } else if (connectionType == ucxhEDM_CONNECTION_TYPE_IPv4) {
                type = ucxhEDM_EVENT_CONNECT_IPv4;
            } else if (connectionType == ucxhEDM_CONNECTION_TYPE_IPv6) {
                type = ucxhEDM_EVENT_CONNECT_IPv6;
            } else {
                type = ucxhEDM_EVENT_INVALID;
            }
            break;
        case ucxhEDM_TYPE_DISCONNECT_EVENT:
            type = ucxhEDM_EVENT_DISCONNECT;
            break;
        case ucxhEDM_TYPE_DATA_EVENT:
            type = ucxhEDM_EVENT_DATA;
            break;
        case ucxhEDM_TYPE_AT_RESPONSE:
        case ucxhEDM_TYPE_AT_EVENT:
            type = ucxhEDM_EVENT_AT;
            break;
        case ucxhEDM_TYPE_START_EVENT:
            type = ucxhEDM_EVENT_STARTUP;
            break;
        default:
            type = ucxhEDM_EVENT_INVALID;
            break;
    }

    return type;
}

ucxhEDM_btProfile_t getBtProfile(char value)
{
    ucxhEDM_btProfile_t profile;

    switch (value) {
        case 0:
            profile = ucxhEDM_BT_PROFILE_SPP;
            break;
        case 1:
            profile = ucxhEDM_BT_PROFILE_DUN;
            break;
        case 14:
            profile = ucxhEDM_BT_PROFILE_SPS;
                break;
        default:
            profile = ucxhEDM_BT_PROFILE_INVALID;
            break;
    }

    return profile;
}

ucxhEDM_ipProtocol_t getIpProtocol(char value)
{
    ucxhEDM_ipProtocol_t protocol;

    switch (value) {
        case 0x00:
            protocol = ucxhEDM_IP_PROTOCOL_TCP;
            break;
        case 0x01:
            protocol = ucxhEDM_IP_PROTOCOL_UDP;
            break;
        default:
            protocol = ucxhEDM_IP_PROTOCOL_INVALID;
            break;
    }

    return protocol;
}

int edmParse(char* pInData, unsigned int size, ucxhEDM_event_t* pEventData, unsigned int* pConsumed)
{
    int error = ucxhEDM_OK;
    unsigned int payload_size;
    unsigned int used = 0;
    char* pPackedHead = pInData;

    if (pPackedHead == NULL || size < ucxhEDM_MIN_SIZE) {
        *pConsumed = 0;
        return ucxhEDM_ERROR_PARAM;
    }

    for (used = 0; used < size; used++) {
        if (*(pPackedHead + used) == (char)ucxhEDM_HEAD) {
            pPackedHead += used;
            break;
        }
    }

    if (used == size) {
        *pConsumed = used;
        return ucxhEDM_ERROR_INVALID;
    }

    payload_size = ((*(pPackedHead + 1) & ucxhEDM_SIZE_FILTER) << 8) + (*(pPackedHead + 2) & 0xFF);

    if ((size - used) < (ucxhEDM_HEAD_SIZE + ucxhEDM_SIZE_SIZE + payload_size + ucxhEDM_TAIL_SIZE)) {
        *pConsumed = used;
        return ucxhEDM_ERROR_INCOMPLETE;
    }

    if (*(pPackedHead + payload_size + ucxhEDM_HEAD_SIZE + ucxhEDM_SIZE_SIZE) != (char)ucxhEDM_TAIL) {
        *pConsumed = used + ucxhEDM_HEAD_SIZE + ucxhEDM_SIZE_SIZE + payload_size + ucxhEDM_TAIL_SIZE;
        return ucxhEDM_ERROR_SIZE;
    }

    pEventData->type = findEventType(*(pPackedHead + 4), *(pPackedHead + 6));

    switch (pEventData->type) {
        case ucxhEDM_EVENT_CONNECT_BT:
        {
            ucxhEDM_btProfile_t profile = getBtProfile(*(pPackedHead + 7));
            if (profile == ucxhEDM_BT_PROFILE_INVALID) {
                error = ucxhEDM_ERROR_CORRUPTED;
                pEventData->type = ucxhEDM_EVENT_INVALID;
            } else {
                pEventData->params.btConnectEvent.profile = profile;
                pEventData->params.btConnectEvent.hChannel = (*(pPackedHead + 5) & 0xFF);
                pEventData->params.btConnectEvent.framesize = ((*(pPackedHead + 14) & 0xFF) << 8) + (*(pPackedHead + 15) & 0xFF);
                memcpy(&pEventData->params.btConnectEvent.address[0], pPackedHead + 8, ucxhEDM_BT_ADDRESS_SIZE);
            }
            used += 17;
            break;
        }
        case ucxhEDM_EVENT_CONNECT_IPv4:
            pEventData->params.ipv4ConnectEvent.hChannel = (*(pPackedHead + 5) & 0xFF);
            pEventData->params.ipv4ConnectEvent.protocol = getIpProtocol(*(pPackedHead + 7));
            memcpy(&pEventData->params.ipv4ConnectEvent.remoteAddress[0], pPackedHead + 8, ucxhEDM_IPv4_ADDRESS_SIZE);
            pEventData->params.ipv4ConnectEvent.remotePort = ((*(pPackedHead + 12) & 0xFF) << 8) + (*(pPackedHead + 13) & 0xFF);
            memcpy(&pEventData->params.ipv4ConnectEvent.localAddress[0], pPackedHead + 14, ucxhEDM_IPv4_ADDRESS_SIZE);
            pEventData->params.ipv4ConnectEvent.localPort = ((*(pPackedHead + 18) & 0xFF) << 8) + (*(pPackedHead + 19) & 0xFF);
            used += 21;
            break;
        case ucxhEDM_EVENT_CONNECT_IPv6:
            pEventData->params.ipv6ConnectEvent.hChannel = (*(pPackedHead + 5) & 0xFF);
            pEventData->params.ipv6ConnectEvent.protocol = getIpProtocol(*(pPackedHead + 7));
            memcpy(&pEventData->params.ipv6ConnectEvent.remoteAddress[0], pPackedHead + 8, ucxhEDM_IPv6_ADDRESS_SIZE);
            pEventData->params.ipv6ConnectEvent.remotePort = ((*(pPackedHead + 24) & 0xFF) << 8) + (*(pPackedHead + 25) & 0xFF);
            memcpy(&pEventData->params.ipv6ConnectEvent.localAddress[0], pPackedHead + 26, ucxhEDM_IPv6_ADDRESS_SIZE);
            pEventData->params.ipv6ConnectEvent.localPort = ((*(pPackedHead + 42) & 0xFF) << 8) + (*(pPackedHead + 43) & 0xFF);
            used += 45;
            break;
        case ucxhEDM_EVENT_DISCONNECT:
            pEventData->params.disconnectEvent.hChannel = (*(pPackedHead + 5) & 0xFF);
            used += 7;
            break;
        case ucxhEDM_EVENT_DATA:
            pEventData->params.dataEvent.hChannel = *(pPackedHead + 5);
            pEventData->params.dataEvent.pData = pPackedHead + 6;
            payload_size = ((*(pPackedHead + 1) & ucxhEDM_SIZE_FILTER) << 8) + *(pPackedHead + 2);
            pEventData->params.dataEvent.length = payload_size - 3;
            used += payload_size + 4;
            break;
        case ucxhEDM_EVENT_AT:
            pEventData->params.atEvent.pData = pPackedHead + 5;
            payload_size = ((*(pPackedHead + 1) & ucxhEDM_SIZE_FILTER) << 8) + *(pPackedHead + 2);
            pEventData->params.atEvent.length = payload_size - 2;
            used += payload_size + 4;
            break;
        case ucxhEDM_EVENT_STARTUP:
            used += 6;
            break;
        case ucxhEDM_EVENT_INVALID:
        default:
            used += ucxhEDM_HEAD_SIZE + ucxhEDM_SIZE_SIZE + payload_size + ucxhEDM_TAIL_SIZE;
            error = ucxhEDM_ERROR_CORRUPTED;
            break;
    }

    *pConsumed = used;

    return error;
}

int edmZeroCopyHeadData(ucxhEDM_channel_t hChannel, int size, char* pHead)
{
    if (pHead == NULL || size > ucxhEDM_MAX_SIZE) {
        return ucxhEDM_ERROR_PARAM;
    }

    int edmSize = size + 3;

    *pHead = ucxhEDM_HEAD;
    *(pHead + 1) = edmSize >> 8;
    *(pHead + 2) = edmSize & 0xFF;
    *(pHead + 3) = 0x00;
    *(pHead + 4) = ucxhEDM_TYPE_DATA_COMMAND;
    *(pHead + 5) = hChannel;

    return 6;
}

int edmData(ucxhEDM_channel_t hChannel, const char* pData, int dataSize, char* pEdmPacket)
{
    if (pEdmPacket == NULL || pData == NULL || dataSize > ucxhEDM_MAX_SIZE) {
        return ucxhEDM_ERROR_PARAM;
    }

    edmZeroCopyHeadData(hChannel, dataSize, pEdmPacket);
    memcpy((pEdmPacket + 6), pData, dataSize);
    *(pEdmPacket + dataSize + 6) = ucxhEDM_TAIL;

    return ucxhEDM_OK;
}

int edmZeroCopyHeadRequest(int size, char* pHead)
{
    if (pHead == NULL || size > ucxhEDM_MAX_SIZE) {
        return ucxhEDM_ERROR_PARAM;
    }

    int edmSize = size + 2;

    *pHead = ucxhEDM_HEAD;
    *(pHead + 1) = edmSize >> 8;
    *(pHead + 2) = edmSize & 0xFF;
    *(pHead + 3) = 0x00;
    *(pHead + 4) = ucxhEDM_TYPE_AT_REQUEST;

    return 5;
}

int edmRequest(const char* pAtString, int atStringSize, char* pEdmPacket)
{
    if (pEdmPacket == NULL || pAtString == NULL || atStringSize > ucxhEDM_MAX_SIZE) {
        return ucxhEDM_ERROR_PARAM;
    }

    edmZeroCopyHeadRequest(atStringSize, pEdmPacket);
    memcpy((pEdmPacket + 5), pAtString, atStringSize);
    *(pEdmPacket + atStringSize + 5) = ucxhEDM_TAIL;

    return atStringSize + 5 +1;
}

int edmZeroCopyTail(char* pTail)
{
    if (pTail == NULL) {
        return ucxhEDM_ERROR_PARAM;
    }

    *pTail = ucxhEDM_TAIL;

    return 1;
}

int validEdmPacket(char* pData, int size)
{
    int payload_size;

    if (pData == NULL || size < ucxhEDM_MIN_SIZE) {
        return ucxhEDM_ERROR_PARAM;
    }

    if (*pData != (char)ucxhEDM_HEAD) {
        return ucxhEDM_ERROR_PARAM;
    }

    payload_size = ((*(pData + 1) & ucxhEDM_SIZE_FILTER) << 8) + *(pData + 2);

    if (*(pData + payload_size + ucxhEDM_HEAD_SIZE + ucxhEDM_SIZE_SIZE) != (char)ucxhEDM_TAIL) {
        return ucxhEDM_ERROR_INCOMPLETE;
    }

    return ucxhEDM_OK;
}
