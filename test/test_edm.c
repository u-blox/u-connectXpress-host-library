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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ucxh_edm.h"

/** 
 * \file test_edm.c
 * \test test_edm.c Unit test: Low-level test of EDM packet builder.
 * \brief Unit test: Low-level test of EDM packet builder.s
 *  
## Introduction
 
## Typical response

### Description


*/

#if (defined(_WIN32) || defined (_WIN64) || defined(__gnu_linux__) || defined(__CYGWIN__) )
#define COMMAND_LINE_SUPPORT
#endif

#define CHECK_INT(x, y) do { \
  if (x != y) { \
    printf("Line %d failed, got %d, expected %d\n", __LINE__, (int)x, (int)y); \
    goto done; \
  } \
} while (0)

#define CHECK_ARRAY(x, y) do { \
  if (memcmp(x, y, sizeof(y))) { \
    printf("Line %d failed\n", __LINE__); \
    goto done; \
  } \
} while (0)

char* getCmdLineArg(char* pValue, const char* pArg, unsigned short maxValueLength);

char edmEventStartup[] = {0xAA,0x00,0x02,0x00,0x71,0x55};
char edmEventConnectBt[] = {0xAA,0x00,0x0D,0x00,0x11,0x03,0x01,0x0E,0x11,0xBB,0x33,0x44,0x55,0x66,0x01,0x66,0x55};
char edmBtAddress[] = {0x11,0xBB,0x33,0x44,0x55,0x66};
char edmEventConnectIPv4[] = {0x00,0xAA,0x00,0x11,0x00,0x11,0x05,0x02,0x00,0xC0,0xA8,0x00,0x02,0x13,0x88,0xC0,0xA8,0x00,0x01,0x0F,0xA0,0x55};
char edmIPv4LocalAddress[] = {0xC0,0xA8,0x00,0x01};
char edmIPv4RemoteAddress[] = {0xC0,0xA8,0x00,0x02};
char edmEventConnectIPv6[] = {0xAA,0x00,0x29,0x00,0x11,0x05,0x03,0x00,0xFE,0x80,
                              0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                              0x00,0x00,0x00,0x02,0x13,0x88,0xFE,0x80,0x00,0x00,
                              0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                              0x00,0x01,0x0F,0xA0,0x55};
char edmIPv6LocalAddress[] = {0xFE,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                              0x00,0x00,0x00,0x00,0x00,0x01};
char edmIPv6RemoteAddress[] = {0xFE,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                               0x00,0x00,0x00,0x00,0x00,0x02};
char edmEventDisconnect[] = {0xAA, 0x00, 0x03, 0x00, 0x21, 0x03, 0x55};
char edmEventData[] = {0xAA,0x00,0x05,0x00,0x31,0x03,0x12,0x34,0x55};
char edmTestData[] = {0x12,0x34};
char edmEventAt[] = {0xAA,0x00,0x0E,0x00,0x41,0x2B,0x55,0x55,0x44,0x50,0x44,0x3A,0x33,0x2C,0x31,0x0D,0x0A,0x55};
char edmTestAt[] = {0x2B,0x55,0x55,0x44,0x50,0x44,0x3A,0x33,0x2C,0x31,0x0D,0x0A};
char edmAtRequest[] = {0x41,0x54,ucxhEDM_AT_REQUEST_EXECUTE};
char edmAtRequestPacket[] = {0xAA,0x00,0x05,0x00,0x44,0x41,0x54,0x0D,0x55};
char edmDataCommand[] = {0x12,0x34};
char edmDataCommandPacket[] = {0xAA,0x00,0x05,0x00,0x36,0x03,0x12,0x34,0x55};
char edmDataCommandHead[] = {0xAA,0x01,0x2F,0x00,0x36,0x03};
char edmRequestHead[] = {0xAA,0x01,0x2E,0x00,0x44};
char edmAtString[] = "AT\r";

int main()
{
#ifdef COMMAND_LINE_SUPPORT
    int returncode = EXIT_FAILURE;
#endif
    ucxhEDM_event_t data;
    unsigned int consumed;
    char* request = malloc(sizeof(edmAtRequest) + ucxhEDM_REQUEST_OVERHEAD);
    char* strRequest = malloc(strlen(edmAtString) + ucxhEDM_REQUEST_OVERHEAD);
    char *txData = malloc(sizeof(edmDataCommand) + ucxhEDM_DATA_OVERHEAD);
    char* headRequest = malloc(ucxhEDM_REQUEST_HEAD_SIZE);
    char* headDataCommand = malloc(ucxhEDM_DATA_HEAD_SIZE);

    CHECK_INT(edmParse((char*)&edmEventStartup[0], sizeof(edmEventStartup), &data, &consumed), ucxhEDM_OK);
    CHECK_INT(consumed, sizeof(edmEventStartup));
    CHECK_INT(data.type, ucxhEDM_EVENT_STARTUP);

    memset(&data, 0, sizeof(data));

    CHECK_INT(edmParse((char*)&edmEventConnectBt[0], sizeof(edmEventConnectBt), &data, &consumed), ucxhEDM_OK);
    CHECK_INT(consumed, sizeof(edmEventConnectBt));
    CHECK_INT(data.type, ucxhEDM_EVENT_CONNECT_BT);
    CHECK_INT(data.params.btConnectEvent.hChannel, 3);
    CHECK_INT(data.params.btConnectEvent.profile, ucxhEDM_BT_PROFILE_SPS);
    CHECK_INT(data.params.btConnectEvent.framesize, 358);
    CHECK_ARRAY(data.params.btConnectEvent.address, edmBtAddress);

    memset(&data, 0, sizeof(data));

    CHECK_INT(edmParse((char*)&edmEventConnectIPv4[0], sizeof(edmEventConnectIPv4), &data, &consumed), ucxhEDM_OK);
    CHECK_INT(consumed, sizeof(edmEventConnectIPv4));
    CHECK_INT(data.type, ucxhEDM_EVENT_CONNECT_IPv4);
    CHECK_INT(data.params.ipv4ConnectEvent.hChannel, 5);
    CHECK_INT(data.params.ipv4ConnectEvent.localPort, 4000);
    CHECK_INT(data.params.ipv4ConnectEvent.protocol, ucxhEDM_IP_PROTOCOL_TCP);
    CHECK_INT(data.params.ipv4ConnectEvent.remotePort, 5000);
    CHECK_ARRAY(data.params.ipv4ConnectEvent.localAddress, edmIPv4LocalAddress);
    CHECK_ARRAY(data.params.ipv4ConnectEvent.remoteAddress, edmIPv4RemoteAddress);

    memset(&data, 0, sizeof(data));

    CHECK_INT(edmParse((char*)&edmEventConnectIPv6[0], sizeof(edmEventConnectIPv6), &data, &consumed), ucxhEDM_OK);
    CHECK_INT(consumed, sizeof(edmEventConnectIPv6));
    CHECK_INT(data.type, ucxhEDM_EVENT_CONNECT_IPv6);
    CHECK_INT(data.params.ipv6ConnectEvent.hChannel, 5);
    CHECK_INT(data.params.ipv6ConnectEvent.localPort, 4000);
    CHECK_INT(data.params.ipv6ConnectEvent.protocol, ucxhEDM_IP_PROTOCOL_TCP);
    CHECK_INT(data.params.ipv6ConnectEvent.remotePort, 5000);
    CHECK_ARRAY(data.params.ipv6ConnectEvent.localAddress, edmIPv6LocalAddress);
    CHECK_ARRAY(data.params.ipv6ConnectEvent.remoteAddress, edmIPv6RemoteAddress);

    memset(&data, 0, sizeof(data));

    CHECK_INT(edmParse((char*)&edmEventDisconnect[0], sizeof(edmEventDisconnect), &data, &consumed), ucxhEDM_OK);
    CHECK_INT(consumed, sizeof(edmEventDisconnect));
    CHECK_INT(data.type, ucxhEDM_EVENT_DISCONNECT);
    CHECK_INT(data.params.disconnectEvent.hChannel, 3);

    memset(&data, 0, sizeof(data));

    CHECK_INT(edmParse((char*)&edmEventData[0], sizeof(edmEventData), &data, &consumed), ucxhEDM_OK);
    CHECK_INT(consumed, sizeof(edmEventData));
    CHECK_INT(data.type, ucxhEDM_EVENT_DATA);
    CHECK_INT(data.params.dataEvent.hChannel, 3);
    CHECK_INT(data.params.dataEvent.length, 2);
    CHECK_ARRAY(data.params.dataEvent.pData, edmTestData);

    memset(&data, 0, sizeof(data));

    CHECK_INT(edmParse((char*)&edmEventAt[0], sizeof(edmEventAt), &data, &consumed), ucxhEDM_OK);
    CHECK_INT(consumed, sizeof(edmEventAt));
    CHECK_INT(data.type, ucxhEDM_EVENT_AT);
    CHECK_INT(data.params.atEvent.length, 12);
    CHECK_ARRAY(data.params.atEvent.pData, edmTestAt);

    memset(&data, 0, sizeof(data));

    CHECK_INT(edmRequest(edmAtRequest, sizeof(edmAtRequest), request), sizeof(edmAtRequestPacket));
    CHECK_ARRAY(request, edmAtRequestPacket);

    edmData(3, edmDataCommand, sizeof(edmDataCommand), txData);
    CHECK_ARRAY(txData, edmDataCommandPacket);

    edmZeroCopyHeadRequest(300, headRequest);
    CHECK_ARRAY(headRequest, edmRequestHead);

    edmZeroCopyHeadData(3, 300, headDataCommand);
    CHECK_ARRAY(headDataCommand, edmDataCommandHead);

    CHECK_INT(edmRequest(edmAtString, strlen(edmAtString), strRequest), sizeof(edmAtRequestPacket));
    CHECK_ARRAY(strRequest, edmAtRequestPacket);

    printf("All tests passed\n"); 
#ifdef COMMAND_LINE_SUPPORT
    returncode = EXIT_SUCCESS;
#endif
done:
    free(request);
    free(strRequest);
    free(txData);
    free(headRequest);
    free(headDataCommand);

#ifdef COMMAND_LINE_SUPPORT
    return returncode;
#else
    while (1);
#endif    
}
