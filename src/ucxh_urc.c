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


#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ucxh_config.h"
#include "ucxh_library.h"

#include "ucxh_urc.h"

TRIVIAL_EVENT(GMM,  ModelIdentification,             ARRAY, model)
TRIVIAL_EVENT(GMI,  ManufacturerIdentification,      ARRAY, manufacturer)
TRIVIAL_EVENT(GMR,  SoftwareVersionIdentification,   ARRAY, software_version)
TRIVIAL_EVENT(GSN,  SerialNumber,                    ARRAY, serial_number)
TRIVIAL_EVENT(I0,   TypeCodeIdentification,          ARRAY, type_code)
TRIVIAL_EVENT(I10,  McuIdentification,               ARRAY, mcu_identification)
TRIVIAL_EVENT(UDCP, ConnectPeer,                     INT,   peer_id)
EMPTY_EVENT(STARTUP, Startup)

//
//  For non-trivial events, we need to write a little more code...
//

ucxhURC_parseResult_t onI9_createEvent(uint8_t numParams, const char *ppParams[], ucxhPARSER_urcIdentificationInformationEvent_t *pEvent)
{
    ucxhURC_parseResult_t result = ucxhURC_PARSE_RESULT_OK;
    ucxhPARSER_urcIdentificationInformationEvent_t event = {0};

    assert(pEvent != NULL);
    ASSERTPARAMS(numParams, ppParams, 3);
    
    STRCPY2ARRAYORFAIL(event.application_version,   ppParams[1]);
    STRCPY2ARRAYORFAIL(event.unique_identifier,     ppParams[2]);
    
    memcpy(pEvent, &event, sizeof(*pEvent));
    return ucxhURC_PARSE_RESULT_OK;
}

ucxhURC_parseResult_t onUMLA_createEvent(uint8_t numParams, const char *ppParams[], ucxhPARSER_urcLocalAddressEvent_t *pEvent)
{
    ucxhURC_parseResult_t result = ucxhURC_PARSE_RESULT_OK;
    ucxhPARSER_urcLocalAddressEvent_t event = {0};

    assert(pEvent != NULL);
    ASSERTPARAMS(numParams, ppParams, 2);
    
    STRCPY2ARRAYORFAIL(event.mac_address,           ppParams[1]);
    STRCPY2INTORFAIL  (event.interface_id,          ucxhPARSER_getLastCommandParam());
     
    memcpy(pEvent, &event, sizeof(*pEvent));
    return ucxhURC_PARSE_RESULT_OK;
}


bool ucxhCOMMAND_readLocalAddress(ucxhPARSER_hardwareInterfaceId_t interface_id) {
    ucxhPARSER_buildSetCmd("+UMLA");
    ucxhPARSER_addUnsignedInt(interface_id);
    return ucxhPARSER_sendBuiltCommand();
}


bool ucxhCOMMAND_readTypeCodeIdentification(void)
{
    ucxhPARSER_buildActionCmd("I0");
    return ucxhPARSER_sendBuiltCommand();
}


bool ucxhCOMMAND_readIdentificationInformation(void)
{
    ucxhPARSER_buildActionCmd("I9");
    return ucxhPARSER_sendBuiltCommand();
}


bool ucxhCOMMAND_readMcuIdentification(void)
{
    ucxhPARSER_buildActionCmd("I10");
    return ucxhPARSER_sendBuiltCommand();
}


bool ucxhCOMMAND_readManufacturerIdentification(void)
{
    ucxhPARSER_buildActionCmd("+GMI");
    return ucxhPARSER_sendBuiltCommand();
}


bool ucxhCOMMAND_readModelIdentification(void)
{
    ucxhPARSER_buildActionCmd("+GMM");
    return ucxhPARSER_sendBuiltCommand();
}


bool ucxhCOMMAND_readSoftwareVersionIdentification(void)
{
    ucxhPARSER_buildActionCmd("+GMR");
    return ucxhPARSER_sendBuiltCommand();
}


bool ucxhCOMMAND_readSerialNumber(void)
{
    ucxhPARSER_buildActionCmd("+GSN");
    return ucxhPARSER_sendBuiltCommand();
}

static char queryString[ucxhAT_CMD_MAX_LEN+1];
static bool addQueryString(const char *query, const char *value)
{
    static bool isFirst = true;
    static size_t queryStringLength = 0;

    if (query == NULL && value == NULL) {
        isFirst = true;
        queryStringLength = 0;
        udprintf(UCXH_PARSER_TRACE_AT, 5, "addQueryString: Invoked with (null,null): Resetting\n", "");
        return true;
    }

    assert(query != NULL);

    size_t query_length = strlen(query);
    size_t value_length = 0;
    if (value != NULL) {
        value_length = 1 + strlen(value);
    }

    size_t query_value_length = query_length + value_length;
    size_t resulting_length = queryStringLength + query_value_length;
    if (value != NULL) {
        resulting_length += 1;
    } 
    udprintf(UCXH_PARSER_TRACE_AT, 5, "addQueryString: queryStringLength: %d, query_length: %d, value_length: %d, query_value_length: %d resulting_length: %d, sizeof(queryString): %d\n",
        queryStringLength, query_length, value_length, query_value_length, resulting_length, sizeof(queryString));
    if (resulting_length > sizeof(queryString)-1) {
        udprintf(UCXH_PARSER_TRACE_AT, 1, "addQueryString: URL too long (%d chars, max allowed %d chars): url '%s' (length %d) and query: '%s%s%s' (length %d)\n",
            resulting_length, sizeof(queryString)-1,
            queryString,
            queryStringLength,
            query, (value != NULL ? "=" : ""), (value != NULL ? value : ""),
            query_value_length
        );

        isFirst = true;
        queryString[0] = '\0';
        queryStringLength = 0;

        return false;
    }

    if (value == NULL && queryStringLength == 0) {
        udprintf(UCXH_PARSER_TRACE_AT, 5, "addQueryString: Starting query: '%s'\n", query);

        strncpy(queryString, query, sizeof(queryString) -1);
        queryString[sizeof(queryString)-1] = '\0';
        queryStringLength += resulting_length;
    } else {
        udprintf(UCXH_PARSER_TRACE_AT, 5, "addQueryString: Adding query: '%s' and value '%s'\n", query, value);

        size_t added = snprintf(queryString+queryStringLength, sizeof(queryString) - queryStringLength -1, 
            "%s%s%s%s",
            (isFirst ? "?" : "&"), query, (value != NULL ? "=" : ""), (value != NULL ? value : ""));
        queryString[sizeof(queryString)-1] = '\0';

        udprintf(UCXH_PARSER_TRACE_AT, 5, "addQueryString: Added string and value with %d chars, expected %d chars\n", added, 1+query_value_length);
        assert(added == 1+query_value_length);
        queryStringLength += added;
        isFirst = false;
    }

    udprintf(UCXH_PARSER_TRACE_AT, 5, "addQueryString: New queryString: '%s'\n", queryString);

    return true;
}

static bool addQueryUnsignedInt(const char *query, uint32_t value)
{
    char buffer[10+1];
    sprintf(buffer, "%u", value);
    return addQueryString(query, buffer);
}

static const char *getUrlQueryString(void)
{
    return queryString;
}

bool createUrlQueryString(ucxhPARSER_peerURL_t *pUrl)
{
    static char buffer[ucxhAT_CMD_MAX_LEN+1]; // Static since large
    bool success = true;
    size_t length = 0;

    assert(pUrl != NULL);

    success &= addQueryString(NULL, NULL);

    switch (pUrl->scheme) {
        case ucxhPARSER_PEER_URL_SCHEME_TCP: {
            const char *pre_domain = "";
            const char *post_domain = "";

            if (strchr(pUrl->tcp.domain, ':') != NULL) {
                pre_domain = "[";
                post_domain = "]";
            }

            length += snprintf(buffer, sizeof(buffer)-length-1, "tcp://%s%s%s:%u/", pre_domain, pUrl->tcp.domain, post_domain, pUrl->tcp.remote_port);
            buffer[sizeof(buffer)-1] = '\n';
            if (length >= sizeof(buffer)-1) {
                udprintf(UCXH_PARSER_TRACE_AT, 1, "createUrlQueryString: URL too long (%d chars, max allowed %d chars: '%s'", length, sizeof(buffer), buffer);
                return false;
            }

            success &= addQueryString(buffer, NULL);

            // Create reasonable keepAlive values in case one or two of them is missing
            uint32_t idle_ms     = pUrl->tcp.query.keep_alive.idle_ms;
            uint32_t interval_ms = pUrl->tcp.query.keep_alive.interval_ms;
            uint32_t count       = pUrl->tcp.query.keep_alive.count;

            if (idle_ms > 0) {
                if (interval_ms == 0) { interval_ms = idle_ms / 2; }
                if (count == 0) { count = interval_ms / 10; }
            }
            if (interval_ms > 0) {
                if (idle_ms == 0) {idle_ms = interval_ms * 2; };
                if (count == 0) { count = interval_ms / 10; }
            }
            if (count > 0) {
                if (interval_ms == 0) { interval_ms = count * 10; }
                if (idle_ms == 0) {idle_ms = interval_ms * 2; };
            }
            if (count > 0 && interval_ms > 0 && idle_ms > 0) {
                snprintf(buffer, sizeof(buffer)-1, "%u+%u+%u", idle_ms, interval_ms, count);
                buffer[sizeof(buffer)-1] = '\0';
            } else {
                buffer[0] = '\0';
            }

            if (pUrl->tcp.query.local_port != 0)      {success &= addQueryUnsignedInt("local_port",   pUrl->tcp.query.local_port); }
            if (pUrl->tcp.query.flush_tx)             {success &= addQueryUnsignedInt("flush_tx",     1); }
            if (buffer[0] != '\0')                    {success &= addQueryString     ("keepAlive",    buffer); } 
            if (pUrl->tcp.query.ca[0] != '\0')        {success &= addQueryString     ("ca",           pUrl->tcp.query.ca); }
            if (pUrl->tcp.query.cert[0] != '\0')      {success &= addQueryString     ("cert",         pUrl->tcp.query.cert); }
            if (pUrl->tcp.query.priv_key[0] != '\0')  {success &= addQueryString     ("privKey",      pUrl->tcp.query.priv_key); }
            if (pUrl->tcp.query.encr != 0)            {success &= addQueryUnsignedInt("encr",         pUrl->tcp.query.encr); }
            
            break;
        }

        default:
            udprintf(UCXH_PARSER_TRACE_AT, 1, "createUrlQueryString: Invalid url.scheme %d!\n", pUrl->scheme);
            return false;
    }

    if(!success) {
        udprintf(UCXH_PARSER_TRACE_AT, 1, "createUrlQueryString: Unable to create URL!\n", "");
        return false;
    }

    return true;
}

bool ucxhCOMMAND_writeDefaultRemotePeer(ucxhPARSER_defaultRemotePeerParam_t *pParam)
{
    size_t charsWritten = 0;
    size_t length = 0;
    bool success = true;

    assert(pParam != NULL);

    success &= createUrlQueryString(&pParam->url);

    uint32_t connect_scheme = 0x0;
    if (pParam->connect_scheme.always_connected) { connect_scheme |= (0x1 << 1); }   // Always connected bit number: 1
    if (pParam->connect_scheme.external_connect) { connect_scheme |= (0x1 << 2); }   // External connect bit number: 2

    if (connect_scheme == 0x0) {
        udprintf(UCXH_PARSER_TRACE_AT, 1, "createUrlQueryString: Invalid url.connect_scheme %d, must have at least one bit!\n", connect_scheme);
        return false;
    }

    if(!success) {
        udprintf(UCXH_PARSER_TRACE_AT, 1, "ucxhCOMMAND_writeDefaultRemotePeer: Unable to create URL!\n", "");
        return false;
    }

    ucxhPARSER_buildSetCmd("+UDDRP");
    ucxhPARSER_addUnsignedInt(pParam->peer_id);
    ucxhPARSER_addString(getUrlQueryString());
    ucxhPARSER_addUnsignedInt(connect_scheme);

    return ucxhPARSER_sendBuiltCommand();
}

bool ucxhCOMMAND_connectPeer(ucxhPARSER_peerURL_t *pUrl)
{
    bool success = true;

    assert(pUrl != NULL);

    success &= createUrlQueryString(pUrl);

    if(!success) {
        udprintf(UCXH_PARSER_TRACE_AT, 1, "ucxhCOMMAND_connectPeer: Unable to create URL!\n", "");
        return false;
    }

    ucxhPARSER_buildSetCmd("+UDCP");
    ucxhPARSER_addString(getUrlQueryString());

    return ucxhPARSER_sendBuiltCommand();
}


ucxhURC_parseResult_t onUWSC_createEvent(uint8_t numParams, const char *ppParams[], ucxhPARSER_urcWifiStationConfigurationEvent_t *pEvent)
{
    // TODO: Implement!
    return ucxhURC_PARSE_RESULT_ERROR;
}


ucxhURC_parseResult_t onUNSTAT_createEvent(uint8_t numParams, const char *ppParams[], ucxhPARSER_urcNetworkStatusEvent_t *pEvent)
{
    ucxhURC_parseResult_t result = ucxhURC_PARSE_RESULT_OK;
    bool success = false;
    static int last_interface_id = -1;
    int interface_id = -1;

    assert(pEvent != NULL);
    
    if (numParams > 0 || ppParams != NULL) {
        const char *ipv6_address = NULL;
        int status_id = -1;

        ASSERTPARAMS(numParams, ppParams, 4);
        
        const char *interface_id_s        = ppParams[1];
        const char *status_id_s           = ppParams[2];
        const char *status_val            = ppParams[3];
        const char *ipv6_status_state     = ppParams[4];

        udprintf(UCXH_PARSER_TRACE_URC, 2, "%s (Network Status) "
            "interface_id='%s', status_id='%s', status_val='%s', ipv6_status_state='%s'\n", 
            ppParams[0],
            interface_id_s, status_id_s, status_val, numParams >= 5 ? ipv6_status_state: "");

        ucxhPARSER_urcNetworkStatusEvent_t *network_status = NULL;
        STRCPY2INT(interface_id, interface_id_s,  &success);
        if (success) {
            STRCPY2INT(status_id,    status_id_s,     &success);
        }
        if (success && interface_id >= 0 && status_id >= 0 && interface_id < ELEMENTS_OF(networkStatus)) {
            if (interface_id == last_interface_id || last_interface_id == -1) {
                result = ucxhURC_PARSE_RESULT_NEED_MORE;
                last_interface_id = interface_id;
            }
            network_status = &networkStatus[interface_id];
            network_status->interface_id = interface_id;
            success = false;
            switch (status_id) {
                case 0:   STRCPY2ARRAY(network_status->interface_hardware_address, status_val, &success); break;
                case 1:   STRCPY2INT(  network_status->network_status,             status_val, &success); break;
                case 2:   STRCPY2INT(  network_status->interface_type,             status_val, &success); break;
                case 101: STRCPY2ARRAY(network_status->ipv4_address,               status_val, &success); break;
                case 102: STRCPY2ARRAY(network_status->ipv4_subnet_mask,           status_val, &success); break;
                case 103: STRCPY2ARRAY(network_status->ipv4_gateway,               status_val, &success); break;
                case 104: STRCPY2ARRAY(network_status->primary_dns_server,         status_val, &success); break;
                case 105: STRCPY2ARRAY(network_status->secondary_dns_server,       status_val, &success); break;
                case 201: STRCPY2ARRAY(network_status->ipv6_link_local_address,    status_val, &success); break;
                case 210: // FALLTHROUGH
                case 211: // FALLTHROUGH
                case 212: {
                    int index = status_id - 210;
                    STRCPY2ARRAY(network_status->ipv6_address[index].ipv6_address,      status_val, &success);
                    if (success) {
                        STRCPY2INT(  network_status->ipv6_address[index].ipv6_status_state, ipv6_status_state, &success);
                    }
                    break;
                }
                default: 
                    udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (Network Status) Undefined status_id %s on Interface Id %s\n",
                        ppParams[0],
                        status_id_s, interface_id_s);
                    success = true; // Probably newer firmware
                break;
            }

            if(!success) {
                udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (Network Status) Unparsable parameter '%s' for Status Id %s on Interface Id %s\n",
                        ppParams[0],
                        status_val, status_id_s, interface_id_s);
            }
        } else {
            udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (Network Status) Undefined Interface Id %s with status_id %s\n",
                ppParams[0],
                interface_id_s, status_id_s);
        }
    } else {
        result == ucxhURC_PARSE_RESULT_OK;
        if (last_interface_id != -1) {
            success = true;
        }
    }
        
    if (!success) {
        result = ucxhURC_PARSE_RESULT_ERROR;
    }

    if (result == ucxhURC_PARSE_RESULT_OK) {
        assert(last_interface_id != -1);
        memcpy(pEvent, &networkStatus[last_interface_id], sizeof(*pEvent));
    }
    last_interface_id = interface_id;

  	return result;
}


ucxhURC_parseResult_t onUUDPC_createEvent(uint8_t numParams, const char *ppParams[], ucxhPARSER_urcPeerConnectedEvent_t *pEvent)
{
    ucxhPARSER_urcPeerConnectedEvent_t event = {0};

    assert(pEvent != NULL);
    ASSERTPARAMS(numParams, ppParams, 5);

    bool success = false;
    const char *peer_handle_s = ppParams[1];
    const char *type_s        = ppParams[2];

    STRCPY2INT(event.peer_handle, peer_handle_s, &success);
    if (!success) {
        udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (Peer Connected) Unparsable parameter 'Peer Handle' %s\n",
              ppParams[0],
              peer_handle_s);
        return ucxhURC_PARSE_RESULT_ERROR;
    }

    STRCPY2INT(event.type, type_s, &success);
    if (!success) {
        udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (Peer Connected) Unparsable parameter 'Type' %s\n",
              ppParams[0],
              type_s);
        return ucxhURC_PARSE_RESULT_ERROR;
    }

    switch (event.type) {
        case ucxhURC_PEER_TYPE_IPV4:
        case ucxhURC_PEER_TYPE_IPV6:
            if (numParams < 8) {
                udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (Peer Connected) Too few parameters for 'Type' %s\n",
                    ppParams[0],
                    type_s);
                return ucxhURC_PARSE_RESULT_ERROR;
            }

            ASSERTPARAMS(numParams, ppParams, 8);

            const char *protocol_s          = ppParams[3];
            const char *local_address_s     = ppParams[4];
            const char *local_port_s        = ppParams[5];
            const char *remote_address_s    = ppParams[6];
            const char *remote_port_s       = ppParams[7];

            udprintf(UCXH_PARSER_TRACE_URC, 2, "%s (Peer Connected IP%s) "
                "protocol='%s', local_address_s='%s', local_port='%s', remote_address='%s', remote_port='%s'\n", 
                ppParams[0],
                (event.type == ucxhURC_PEER_TYPE_IPV4 ? "V4" : "V6"),
                protocol_s, local_address_s, local_port_s, remote_address_s, remote_port_s);

            uint16_t local_port;
            uint16_t remote_port;
            uint16_t protocol;

            STRCPY2INT(protocol, protocol_s, &success);
            if (!success) {
                udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (Peer Connected) Unparsable parameter 'IP Protocol' %s\n",
                    ppParams[0],
                    protocol_s);
                return ucxhURC_PARSE_RESULT_ERROR;
            }

            STRCPY2INT(local_port, local_port_s, &success);
            if (!success) {
                udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (Peer Connected) Unparsable parameter 'Local IP Port' %s\n",
                    ppParams[0],
                    local_port_s);
                return ucxhURC_PARSE_RESULT_ERROR;
            }

            STRCPY2INT(remote_port, remote_port_s, &success);
            if (!success) {
                udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (Peer Connected) Unparsable parameter 'Remote IP Port' %s\n",
                    ppParams[0],
                    remote_port_s);
                return ucxhURC_PARSE_RESULT_ERROR;
            }

            if (event.type == ucxhURC_PEER_TYPE_IPV4) {
                event.ipv4_peer.local_port = local_port;
                event.ipv4_peer.remote_port = remote_port;
                event.ipv4_peer.protocol = protocol;

                STRCPY2ARRAY(event.ipv4_peer.local_address, local_address_s, &success);
                if (!success || !ucxhPARSER_isIpV4Address(local_address_s)) {
                    udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (Peer Connected) Unparsable parameter 'Local IPV4 Address' %s\n",
                        ppParams[0],
                        remote_address_s);
                    return ucxhURC_PARSE_RESULT_ERROR;
                }

                STRCPY2ARRAY(event.ipv4_peer.remote_address, remote_address_s, &success);
                if (!success || !ucxhPARSER_isValidIpV4Address(remote_address_s)) {
                    udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (Peer Connected) Unparsable parameter 'Remote IPV4 Address' %s\n",
                        ppParams[0],
                        remote_address_s);
                    return ucxhURC_PARSE_RESULT_ERROR;
                }

            } else {
                event.ipv6_peer.local_port = local_port;
                event.ipv6_peer.remote_port = remote_port;
                event.ipv6_peer.protocol = protocol;

                STRCPY2ARRAY(event.ipv4_peer.local_address, local_address_s, &success);
                if (!success || !ucxhPARSER_isIpV6Address(local_address_s)) {
                    udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (Peer Connected) Unparsable parameter 'Local IPV4 Address' %s\n",
                        ppParams[0],
                        local_address_s);
                    return ucxhURC_PARSE_RESULT_ERROR;
                }

                STRCPY2ARRAY(event.ipv4_peer.remote_address, remote_address_s, &success);
                if (!success || !ucxhPARSER_isValidIpV6Address(remote_address_s)) {
                    udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (Peer Connected) Unparsable parameter 'Remote IPV4 Address' %s\n",
                        ppParams[0],
                        remote_address_s);
                    return ucxhURC_PARSE_RESULT_ERROR;
                }
            }
            break;
        
        case ucxhURC_PEER_TYPE_BLUETOOTH:
        default:
            udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (Peer Connected) Unsupported Type %u\n",
                        ppParams[0],
                        event.type);
            // TODO: Bluetooth peer
            return ucxhURC_PARSE_RESULT_ERROR;
    }


    assert(success);
    memcpy(pEvent, &event, sizeof(*pEvent));

    return ucxhURC_PARSE_RESULT_OK;
}


ucxhURC_parseResult_t onUUDPD_createEvent(uint8_t numParams, const char *ppParams[], ucxhPARSER_urcPeerDisconnectedEvent_t *pEvent)
{
    ucxhPARSER_urcPeerDisconnectedEvent_t event = {0};
    bool success = false;

    assert(pEvent != NULL);
    ASSERTPARAMS(numParams, ppParams, 2);
    const char *peer_handle_s = ppParams[1];
    

    udprintf(UCXH_PARSER_TRACE_URC, 2, "%s (Peer Disconnected) peer_handle='%s'\n",
            ppParams[0],
            peer_handle_s);

    STRCPY2INT(event.peer_handle, peer_handle_s, &success);
    if(!success) {
        udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (Peer Disconnected) Unparsable parameter 'Peer handle' %s\n",
            ppParams[0],
            peer_handle_s);
        return ucxhURC_PARSE_RESULT_ERROR;
    }

    assert(success);
    memcpy(pEvent, &event, sizeof(*pEvent));
    return ucxhURC_PARSE_RESULT_OK;
}


ucxhURC_parseResult_t onUUND_createEvent(uint8_t numParams, const char *ppParams[], ucxhPARSER_urcNetworkDownEvent_t *pEvent)
{
    ucxhPARSER_urcNetworkDownEvent_t event = {0};
    bool success = false;

    assert(pEvent != NULL);
    ASSERTPARAMS(numParams, ppParams, 2);
    
    const char *interface_id_s = ppParams[1];
    STRCPY2INT(event.interface_id, interface_id_s, &success);
    if (!success) {
        udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (Network Down) Unparsable parameter 'Interface Id' %s\n",
              ppParams[0],
              interface_id_s);
        return ucxhURC_PARSE_RESULT_ERROR;
    }
    
    udprintf(UCXH_PARSER_TRACE_URC, 2, "%s (Network Down) interface_id=%d\n", 
            ppParams[0],
            event.interface_id);

    networkUp[event.interface_id] = false;
    networkChanged = true;

    assert(success);
    memcpy(pEvent, &event, sizeof(event));
    return ucxhURC_PARSE_RESULT_OK;
}


ucxhURC_parseResult_t onUUNU_createEvent(uint8_t numParams, const char *ppParams[], ucxhPARSER_urcNetworkUpEvent_t *pEvent)
{
    ucxhPARSER_urcNetworkUpEvent_t event = {0};
    bool success = false;

    assert(pEvent != NULL);
    ASSERTPARAMS(numParams, ppParams, 2);
    
    const char *interface_id_s = ppParams[1];
    STRCPY2INT(event.interface_id, interface_id_s, &success);
    if (!success) {
        udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (Network Up) Unparsable parameter 'Interface Id' %s\n",
              ppParams[0],
              interface_id_s);
        return ucxhURC_PARSE_RESULT_ERROR;
    }
    udprintf(UCXH_PARSER_TRACE_URC, 2, "%s (Network Up) interface_id=%d\n",
            ppParams[0],
            event.interface_id);

    networkUp[event.interface_id] = true;
    networkChanged = true;

    assert(success);
    memcpy(pEvent, &event, sizeof(event));
    return ucxhURC_PARSE_RESULT_OK;
}


ucxhURC_parseResult_t onUUWLD_createEvent(uint8_t numParams, const char *ppParams[], ucxhPARSER_urcWifiLinkDisconnectedEvent_t *pEvent)
{
    ucxhPARSER_urcWifiLinkDisconnectedEvent_t event = {0};
    bool success = false;

    assert(pEvent != NULL);
    ASSERTPARAMS(numParams, ppParams, 2);
    
    const char *connection_id_s = ppParams[1];

    STRCPY2INT(event.connection_id, connection_id_s, &success);
    if (!success) {
        udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (WiFi Link Disonnected) Unparsable parameter 'Connection Id' %s\n",
              ppParams[0],
              connection_id_s);
        return ucxhURC_PARSE_RESULT_ERROR;
    }

    udprintf(UCXH_PARSER_TRACE_URC, 2, "%s (WiFi Link Disonnected) Connection Id %d\n",
            ppParams[0],
            event.connection_id);

    wifiChanged = true;
    wifiLinkConnected = false;

    assert(success);
    memcpy(pEvent, &event, sizeof(event));
    return ucxhURC_PARSE_RESULT_OK;
}


ucxhURC_parseResult_t onUUWLE_createEvent(uint8_t numParams, const char *ppParams[], ucxhPARSER_urcWifiLinkConnectedEvent_t *pEvent)
{
    ucxhPARSER_urcWifiLinkConnectedEvent_t event = {0};
    bool success = false;

    assert(pEvent != NULL);
    ASSERTPARAMS(numParams, ppParams, 2);

    const char *connection_id_s = ppParams[1];
    const char *bssid = ppParams[2];
    const char *channel_s = ppParams[3];

    STRCPY2INT(event.connection_id, connection_id_s, &success);
    if (!success) {
        udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (WiFi Link Connected) Unparsable parameter 'Connection Id' %s\n",
              ppParams[0],
              connection_id_s);
        return ucxhURC_PARSE_RESULT_ERROR;
    }

    STRCPY2INT(event.channel, channel_s, &success);
    if (!success) {
        udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (WiFi Link Connected) Unparsable parameter 'Channel' %s\n",
              ppParams[0],
              channel_s);
        return ucxhURC_PARSE_RESULT_ERROR;
    }

    STRCPY2ARRAY(event.bssid, bssid, &success);
    if (!success) {
        udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (WiFi Link Connected) Unparsable parameter 'BSSID' %s\n",
              ppParams[0],
              bssid);
        return ucxhURC_PARSE_RESULT_ERROR;
    }

    udprintf(UCXH_PARSER_TRACE_URC, 2, "%s (WiFi Link Connected) Connection Id %d BSSID %s Channel %d\n",
            ppParams[0],
            event.connection_id, event.bssid, event.channel);

    wifiChanged = true;
    wifiLinkConnected = true;

    assert(success);
    memcpy(pEvent, &event, sizeof(event));
    return ucxhURC_PARSE_RESULT_OK;
}


ucxhURC_parseResult_t onUWSSTAT_createEvent(uint8_t numParams, const char *ppParams[], ucxhPARSER_urcWifiStationStatusEvent_t *pEvent)
{
    // UWSSTAT doesn't provide all information prior to the OK, and we also cannot know which will arrive 
    // in the currently available firmwares without checking the module and firmware version, since 
    // not all status_id's are implemented in all modules.
    // However, the way this callback and event is designed, it might be possible in the future to gather and return a single event.
    static ucxhPARSER_urcWifiStationStatusEvent_t event = {};
    bool success = false;

    ASSERTPARAMS(numParams, ppParams, 3);

    const char *status_id_s = ppParams[1];
    const char *status_val  = ppParams[2];

    STRCPY2INT(event.status_id, status_id_s, &success);
    if (success) {
        udprintf(UCXH_PARSER_TRACE_URC, 2, "%s (WiFi Station Status) %d '%s'\n",
                ppParams[0],
                event.status_id, status_val);

        switch (event.status_id) {
            case ucxhURC_WIFI_STATION_STATUS_SSID:
                STRCPY2ARRAY(event.ssid, status_val, &success);
                break;

            case ucxhURC_WIFI_STATION_STATUS_BSSID:
                STRCPY2ARRAY(event.bssid, status_val, &success);
                break;

            case ucxhURC_WIFI_STATION_STATUS_CHANNEL:
                STRCPY2INT(event.channel, status_val, &success);
                break;

            case ucxhURC_WIFI_STATION_STATUS_STATUS:
                STRCPY2INT(event.status, status_val, &success);
                break;
                
            case ucxhURC_WIFI_STATION_STATUS_RSSI:
                STRCPY2INT(event.rssi, status_val, &success);
                break;
                
            case ucxhURC_WIFI_STATION_STATUS_REGION:
                STRCPY2INT(event.region, status_val, &success);
                break;

  //        case ucxhURC_WIFI_STATION_STATUS_MOBILITY_DOMAIN: // TODO: Bluetooth
            default:
                udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (Wifi Station Status) Undefined status_id %u\n",
                    ppParams[0],
                    event.status_id);
                success = false; // Probably newer firmware
        }

        if (!success) {
            udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (Wifi Station Status) Unparsable parameter %u %s\n",
                ppParams[0],
                event.status_id,
                status_val);

        }
    } else {
        udprintf(UCXH_PARSER_TRACE_URC, 1, "%s (Wifi Station Status) Unparsable status_id '%s'\n",
            ppParams[0],
            status_id_s);
    }

    if (success) {
        memcpy(pEvent, &event, sizeof(event));
    }

    return (success ? ucxhURC_PARSE_RESULT_OK : ucxhURC_PARSE_RESULT_ERROR);
}
