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
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "ucxh_config.h"
#include "ucxh_library.h"
#include "ucxh_serial.h"
#include "ucxh_urc.h"


#include "ucxh_wifi.h"



bool wifiLinkConnected = false;
bool wifiChanged = false;

bool networkUp[ucxhAT_MAX_NETWORK_INTERFACES+1] = { false };
int interfaceId = ucxhAT_MAX_NETWORK_INTERFACES;
ucxhPARSER_urcNetworkStatusEvent_t networkStatus[ucxhAT_MAX_NETWORK_INTERFACES] = {0};
bool networkChanged = false;

#define ucxhAT_CMD_ACTIVATE_WIFI             "AT+UWSCA=0,3\r\n"
#define ucxhAT_CMD_DEACTIVATE_WIFI           "AT+UWSCA=0,4\r\n"
#define ucxhAT_CMD_STORE_WIFI                "AT+UWSCA=0,1\r\n"
#define ucxhAT_CMD_RESET_WIFI                "AT+UWSCA=0,0\r\n"

#define NO_IPV4_ADDRESS           "0.0.0.0"


static int (*ucxhMODULE_onNetworkStationUp)(ucxhMODULE_urcNetworkStationStatusEvent_t *pEvent);
static int (*ucxhMODULE_onNetworkStationDown)(ucxhMODULE_urcNetworkStationStatusEvent_t *pEvent);


//
//
//
static ucxhPARSER_wifiStationConfigurationId_t currentWifiStationConfigurationId = 0; 

bool ucxhMODULE_readWifiStationStatus(void) {
  ucxhPARSER_buildActionCmd("+UWSSTAT");
  udprintf(UCXH_PARSER_TRACE_WIFI, 3, "ucxhMODULE_readWifiStationStatus: WiFi Station Status queried\n", "");
  return ucxhPARSER_sendBuiltCommand();
}

bool ucxhCOMMAND_readNetworkStatus(ucxhPARSER_networkInterfaceId_t interface_id) {
  if (interface_id != ucxhURC_INTERFACE_ID_ALL) {
    ucxhPARSER_buildSetCmd("+UNSTAT");
    ucxhPARSER_addUnsignedInt(interface_id);
  } else {
    ucxhPARSER_buildActionCmd("+UNSTAT");
  }
  udprintf(UCXH_PARSER_TRACE_WIFI, 3, "ucxhCOMMAND_readNetworkStatus: Network Status queried\n", "");
  return ucxhPARSER_sendBuiltCommand();
}

bool ucxhCOMMAND_writeWifiStationConfiguration(const ucxhPARSER_wifiStationConfigurationParam_t *pParam)
{
  assert(pParam != NULL);
  bool res = true;

  res &= ucxhPARSER_buildSetCmd("+UWSC");
  res &= ucxhPARSER_addUnsignedInt(currentWifiStationConfigurationId);
  res &= ucxhPARSER_addUnsignedInt(pParam->param_tag);

  switch (pParam->param_tag) {
    case ucxhPARSER_WIFI_STATION_CONFIGURATION_ACTIVE_ON_STARTUP:            res &= ucxhPARSER_addBool(pParam->active_on_startup); break;
    case ucxhPARSER_WIFI_STATION_CONFIGURATION_SSID:                         res &= ucxhPARSER_addString(pParam->ssid); break;
    // 4 reserved
    case ucxhPARSER_WIFI_STATION_CONFIGURATION_AUTHENTICATION:               res &= ucxhPARSER_addUnsignedInt(pParam->authentication); break;
    // 6, 7 only for ODIN-W2
    case ucxhPARSER_WIFI_STATION_CONFIGURATION_PSK_PASSPHRASE:               res &= ucxhPARSER_addString(pParam->passphrase); break;
    case ucxhPARSER_WIFI_STATION_CONFIGURATION_PASSWORD:                     res &= ucxhPARSER_addString(pParam->password); break;
    // case ucxhPARSER_WIFI_STATION_CONFIGURATION_USER_NAME:
    // case ucxhPARSER_WIFI_STATION_CONFIGURATION_DOMAIN_NAME:
    // case ucxhPARSER_WIFI_STATION_CONFIGURATION_CLIENT_CERTIFICATE_NAME:
    // case ucxhPARSER_WIFI_STATION_CONFIGURATION_CLIENT_PRIVATE_KEY:
    // case ucxhPARSER_WIFI_STATION_CONFIGURATION_CA_CERTIFICATE_NAME:
    // case ucxhPARSER_WIFI_STATION_CONFIGURATION_VALIDATE_CA_CERTIFICATE:
    case ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_MODE:                    res &= ucxhPARSER_addUnsignedInt(pParam->ipv4_mode); break;
    case ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_ADDRESS:                 res &= ucxhPARSER_addIpV4Address(pParam->ipv4_address); break;
    case ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_SUBNET_MASK:             res &= ucxhPARSER_addIpV4Address(pParam->ipv4_subnet_mask); break;
    case ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_DEFAULT_GATEWAY:         res &= ucxhPARSER_addIpV4Address(pParam->ipv4_gateway); break;
    case ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_PRIMARY_DNS_SERVER:      res &= ucxhPARSER_addIpV4Address(pParam->primary_dns_server); break;
    case ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_SECONDARY_DNS_SERVER:    res &= ucxhPARSER_addIpV4Address(pParam->secondary_dns_server); break;
    // case ucxhPARSER_WIFI_STATION_CONFIGURATION_ADDRESS_CONFLICT_DETECTION:
    // case ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV6_MODE:
    // case ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV6_LINK_LOCAL_ADDRESS:
    case ucxhPARSER_WIFI_STATION_CONFIGURATION_LISTEN_INTERVAL:              res &= ucxhPARSER_addUnsignedInt(pParam->listen_interval); break;
    case ucxhPARSER_WIFI_STATION_CONFIGURATION_DTIM_ENABLED:                 res &= ucxhPARSER_addBool(pParam->dtim_enabled); break;

    default:
      udprintf(UCXH_PARSER_TRACE_AT, 1, "ucxhCOMMAND_writeWifiStationConfiguration: Unknown param_tag %d\n", pParam->param_tag);
      return false;
  }

  return ucxhPARSER_sendBuiltCommand();
}


//
//
//


#define PARAMSTRCPY(fun, type, dest, src, tag) \
do { \
  if (strlen(src) > 0) { \
    type cfg = { .param_tag = tag}; \
    strncpy(cfg.dest, src, sizeof(cfg.dest)); \
    cfg.dest[sizeof(cfg.dest)-1] = '\0'; \
    if(strcmp(cfg.dest, src) == 0) { \
      udprintf(UCXH_PARSER_TRACE_WIFI, 5, __FILE__ ":%u: " #src ": '%s' sent with (%u characters) for " #dest ", max %u characters expected\n", __LINE__, src, strlen(src), sizeof(cfg.dest)-1); \
      res &= fun(&cfg); \
    } else { \
      udprintf(UCXH_PARSER_TRACE_WIFI, 1, __FILE__ ":%u: Invalid " #src ": '%s' too long (%u characters) for " #dest ", max %u characters expected\n", __LINE__, src, strlen(src), sizeof(cfg.dest)-1); \
      res = false; \
    } \
  } \
} while (0)

#define PARAMINTCPY(fun, type, dest, src, tag) \
do { \
    type cfg = { .param_tag = tag}; \
    cfg.dest = src; \
    res &= fun(&cfg); \
} while (0)

#define PARAMOPTINTCPY(fun, type, dest, src, tag) \
do { \
    if (src != 0) { \
      type cfg = { .param_tag = tag}; \
      cfg.dest = src; \
      res &= fun(&cfg); \
    } \
} while (0)

#define PARAMBOOLCPY(fun, type, dest, src, tag, ontrue, onfalse) \
do { \
    if (src == ontrue || src == onfalse) {\
      type cfg = { .param_tag = tag}; \
      cfg.dest = (src == ontrue); \
      res &= fun(&cfg); \
    } \
} while (0)

bool ucxhMODULE_writeWifiStationConfiguration(const ucxhPARSER_wifiStationConfiguration_t *pConfiguration)
{
  assert(pConfiguration != NULL);
  bool res = true;
  
  #define PARAMSTRCPYwWSC(dest, src, tag) \
  do { \
    PARAMSTRCPY(ucxhCOMMAND_writeWifiStationConfiguration, ucxhPARSER_wifiStationConfigurationParam_t, dest, src, tag); \
  } while (0)

  #define PARAMINTCPYwWSC(dest, src, tag) \
  do { \
    PARAMINTCPY(ucxhCOMMAND_writeWifiStationConfiguration, ucxhPARSER_wifiStationConfigurationParam_t, dest, src, tag); \
  } while (0)

  #define PARAMOPTINTCPYwWSC(dest, src, tag) \
  do { \
    PARAMOPTINTCPY(ucxhCOMMAND_writeWifiStationConfiguration, ucxhPARSER_wifiStationConfigurationParam_t, dest, src, tag); \
  } while (0)

  #define PARAMBOOLCPYwWSC(dest, src, tag, ontrue, onfalse) \
  do { \
    PARAMBOOLCPY(ucxhCOMMAND_writeWifiStationConfiguration, ucxhPARSER_wifiStationConfigurationParam_t, dest, src, tag, ontrue, onfalse); \
  } while (0)


  PARAMBOOLCPYwWSC(active_on_startup,   pConfiguration->on_startup,           ucxhPARSER_WIFI_STATION_CONFIGURATION_ACTIVE_ON_STARTUP, ucxhPARSER_WIFI_STATION_CONFIGURATION_ON_STARTUP_ACTIVE, ucxhPARSER_WIFI_STATION_CONFIGURATION_ON_STARTUP_INACTIVE);
  PARAMSTRCPYwWSC(ssid,                 pConfiguration->ssid,                 ucxhPARSER_WIFI_STATION_CONFIGURATION_SSID);
  PARAMOPTINTCPYwWSC(authentication,    pConfiguration->authentication,       ucxhPARSER_WIFI_STATION_CONFIGURATION_AUTHENTICATION);
  PARAMSTRCPYwWSC(passphrase,           pConfiguration->psk,                  ucxhPARSER_WIFI_STATION_CONFIGURATION_PSK_PASSPHRASE);
  PARAMSTRCPYwWSC(passphrase,           pConfiguration->passphrase,           ucxhPARSER_WIFI_STATION_CONFIGURATION_PSK_PASSPHRASE);
  PARAMSTRCPYwWSC(password,             pConfiguration->password,             ucxhPARSER_WIFI_STATION_CONFIGURATION_PASSWORD);
  PARAMOPTINTCPYwWSC(ipv4_mode,         pConfiguration->ipv4_mode,            ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_MODE);
  PARAMSTRCPYwWSC(ipv4_address,         pConfiguration->ipv4_address,         ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_ADDRESS);
  PARAMSTRCPYwWSC(ipv4_subnet_mask,     pConfiguration->ipv4_subnet_mask,     ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_SUBNET_MASK);
  PARAMSTRCPYwWSC(ipv4_gateway,         pConfiguration->ipv4_gateway,         ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_DEFAULT_GATEWAY);
  PARAMSTRCPYwWSC(primary_dns_server,   pConfiguration->primary_dns_server,   ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_PRIMARY_DNS_SERVER);
  PARAMSTRCPYwWSC(secondary_dns_server, pConfiguration->secondary_dns_server, ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_SECONDARY_DNS_SERVER);
  PARAMBOOLCPYwWSC(dtim_enabled,        pConfiguration->dtim,                 ucxhPARSER_WIFI_STATION_CONFIGURATION_DTIM_ENABLED, ucxhPARSER_WIFI_STATION_CONFIGURATION_DTIM_ENABLE, ucxhPARSER_WIFI_STATION_CONFIGURATION_DTIM_DISABLE);
  PARAMINTCPYwWSC(listen_interval,      pConfiguration->listen_interval,      ucxhPARSER_WIFI_STATION_CONFIGURATION_LISTEN_INTERVAL);

  return res;
}


//
//
//


bool ucxhMODULE_activateWifiStation(void)
{
  ucxhPARSER_buildSetCmd("+UWSCA");
  ucxhPARSER_addUnsignedInt(currentWifiStationConfigurationId);
  ucxhPARSER_addUnsignedInt(3);
  udprintf(UCXH_PARSER_TRACE_WIFI, 3, "ucxhMODULE_activateWifiStation: Trying to activate WiFi Station configuration %d\n", currentWifiStationConfigurationId);

  return ucxhPARSER_sendBuiltCommand();
}

bool ucxhMODULE_deactivateWifiStation(void)
{
  ucxhPARSER_buildSetCmd("+UWSCA");
  ucxhPARSER_addUnsignedInt(currentWifiStationConfigurationId);
  ucxhPARSER_addUnsignedInt(4);

  udprintf(UCXH_PARSER_TRACE_WIFI, 3, "ucxhMODULE_deactivateWifiStation: Trying to deactivateStation configuration %d\n", currentWifiStationConfigurationId);

  return ucxhPARSER_sendBuiltCommand();
}

bool ucxhMODULE_resetWifiStationConfiguration(void)
{
  ucxhPARSER_buildSetCmd("+UWSCA");
  ucxhPARSER_addUnsignedInt(currentWifiStationConfigurationId);
  ucxhPARSER_addUnsignedInt(0);

  udprintf(UCXH_PARSER_TRACE_WIFI, 3, "ucxhMODULE_resetWifiStationConfiguration: Trying to reset Station configuration %d\n", currentWifiStationConfigurationId);

  return ucxhPARSER_sendBuiltCommand();
}

bool ucxhMODULE_storeWifiStationConfiguration(void)
{
  ucxhPARSER_buildSetCmd("+UWSCA");
  ucxhPARSER_addUnsignedInt(currentWifiStationConfigurationId);
  ucxhPARSER_addUnsignedInt(1);

  udprintf(UCXH_PARSER_TRACE_WIFI, 3, "ucxhMODULE_storeWifiStationConfiguration: Trying to store Station configuration %d\n", currentWifiStationConfigurationId);

  return ucxhPARSER_sendBuiltCommand();
}

#if 0
void ucxhMODULE_waitForWifiLink(void)
{
  bool gotData = false;

  do {
    gotData = ucxhPARSER_poll();
  } while (!wifiLinkConnected /* || !networkUp[interfaceId] */ /* || networkStatus[interfaceId].interface_type != INTERFACE_TYPE_WIFI_STATION  || gotData */); // BUG: Requires that the UUNU referes to the interface_id of the WiFi station
  
  udprintf(UCXH_PARSER_TRACE_WIFI, 2, "ucxhMODULE_waitForWifiLink: WiFi Link Connected!\n", "");
}
#endif

ucxhPARSER_networkInterfaceId_t getInterfaceId(ucxhPARSER_interfaceType_t interface_type) {
  for (int interface_id = 0; interface_id < ELEMENTS_OF(networkStatus); interface_id++) {
    if (networkStatus[interface_id].interface_type == interface_type) {
        return interface_id;
      }
  }

  return ucxhURC_INTERFACE_ID_NONE;
}


bool hasLocalIpV4Address(ucxhPARSER_interfaceType_t interface_type) {
  int interface_id = getInterfaceId(interface_type);
  if (interface_id < ELEMENTS_OF(networkStatus) &&
      ucxhPARSER_isValidIpV4Address(networkStatus[interface_id].ipv4_address) &&
      ucxhPARSER_isValidIpV4Address(networkStatus[interface_id].ipv4_gateway) &&
      ucxhPARSER_isValidIpV4Address(networkStatus[interface_id].ipv4_subnet_mask)) {
    return true;
  }
  
  return false;
}

bool isInterfaceUp(ucxhPARSER_interfaceType_t interface_type) {
  int interface_id = getInterfaceId(interface_type);
  if (interface_id < ELEMENTS_OF(networkStatus) &&
      networkStatus[interface_id].network_status != 0) {
    return true;
  }
  
  return false;
}

bool isWifiStationUp(void)
{
  return (isInterfaceUp(ucxhURC_INTERFACE_TYPE_WIFI_STATION) && hasLocalIpV4Address(ucxhURC_INTERFACE_TYPE_WIFI_STATION));
}

#if 0
void ucxhMODULE_waitForNetworkStation(void)
{
  while (!isWifiStationUp()) {
    printf("Keep waiting for more +UUNU or response to UNSTAT indicating WiFi Station connected...");
    networkChanged = false;
    do {
      ucxhPARSER_poll();
    } while (!networkChanged && !isWifiStationUp());

    if (networkChanged && !isWifiStationUp()) {
      // Got UUNU: or UUND:
      printf("Network has changed!\nChecking again ... ");
      networkChanged = false;
      ucxhCOMMAND_readNetworkStatus(ucxhURC_INTERFACE_ID_ALL);
    }
  }
  printf("WiFi Station is connected!\n");
}
#endif

static void generateNetworkStationUp(void)
{
    assert(ucxhMODULE_onNetworkStationUp != NULL);

    ucxhPARSER_urcNetworkStatusEvent_t event;
    ucxhPARSER_networkInterfaceId_t interface_id = getInterfaceId(ucxhURC_INTERFACE_TYPE_WIFI_STATION);

    udprintf(UCXH_PARSER_TRACE_WIFI, 2, "generateNetworkStationUp: WiFi Station is connected!: Invoking callback for onNetworkStationUp()\n", "");
    assert(interface_id < ELEMENTS_OF(networkStatus));
    memcpy(&event, &networkStatus[interface_id], sizeof(event));
    ucxhMODULE_onNetworkStationUp(&event);
}

static void generateNetworkStationDown(void)
{
    assert(ucxhMODULE_onNetworkStationDown != NULL);

    ucxhPARSER_urcNetworkStatusEvent_t event;
    ucxhPARSER_networkInterfaceId_t interface_id = getInterfaceId(ucxhURC_INTERFACE_TYPE_WIFI_STATION);

    udprintf(UCXH_PARSER_TRACE_WIFI, 4, "onNetworkStatusOk:  WiFi Station is disconnected!: Invoking callback for onNetworkStationDown()\n", "");
    assert(interface_id < ELEMENTS_OF(networkStatus));
    memcpy(&event, &networkStatus[interface_id], sizeof(event));
    ucxhMODULE_onNetworkStationDown(&event);
}

static int onNetworkStatusOk(void *_)
{
  if (isWifiStationUp()) {
    if (ucxhMODULE_onNetworkStationUp != NULL) {
      generateNetworkStationUp();
    } else {
      udprintf(UCXH_PARSER_TRACE_WIFI, 4, "onNetworkStatusOk: WiFi Station is connected!\n", "");
    }
  } else {
    if (ucxhMODULE_onNetworkStationDown != NULL) {
      generateNetworkStationDown();
    } else {
      udprintf(UCXH_PARSER_TRACE_WIFI, 2, "onNetworkStatusOk:  WiFi Station is disconnected!\n", "");
    }
  }

  return 0;
}

bool ucxhMODULE_prePollWifi(void)
{
  //while (!isWifiStationUp()) {
  //  printf("Keep waiting for more +UUNU or response to UNSTAT indicating WiFi Station connected...");
    networkChanged = false;
}

bool ucxhMODULE_postPollWifi(void)
{
  if (networkChanged) {
    if (!isWifiStationUp()) {
      // Got UUNU: or UUND:
      udprintf(UCXH_PARSER_TRACE_WIFI, 4, "ucxhMODULE_postPollWifi: Network has changed! - Checking again ... ", "");
      networkChanged = false;
      ucxhPARSER_internalSetOnUNSTATOk(onNetworkStatusOk);
      ucxhCOMMAND_readNetworkStatus(ucxhURC_INTERFACE_ID_ALL);
    } else {
      if (ucxhMODULE_onNetworkStationUp != NULL) {
        udprintf(UCXH_PARSER_TRACE_WIFI, 4, "ucxhMODULE_postPollWifi: Invoking generateNetworkStationUp()\n", "");
        generateNetworkStationUp();
        udprintf(UCXH_PARSER_TRACE_WIFI, 4, "ucxhMODULE_postPollWifi: generateNetworkStationUp() returned\n", "");
      } else {
         udprintf(UCXH_PARSER_TRACE_WIFI, 4, "ucxhMODULE_postPollWifi: WiFi Station is connected!\n", "");
      }
    }
  }

  return true;
}

bool ucxhPARSER_isIpV4Address(const char *s)
{
  int octet[4] = {-1, -1, -1, -1};
  char teststr[ucxhAT_ipV4AddrStr_MAX_LEN+1];
  
  sscanf(s, "%d.%d.%d.%d", &octet[0], &octet[1], &octet[2], &octet[3]);

  sprintf(teststr, "%u.%u.%u.%u", octet[0], octet[1], octet[2], octet[3]);
  if (strcmp(s, teststr) != 0) {
    return false;
  }
  
  if(octet[0] == 0 && octet[1] == 0 && octet[2] == 0 && octet[3] == 0) {
    return true;
  }
  if(octet[0] < 0 || octet[0] > 255 ||
     octet[1] < 0 || octet[1] > 255 ||
     octet[2] < 0 || octet[2] > 255 ||
     octet[3] < 0 || octet[3] > 255) {
    return false;
  }
  
  return true;
}

bool ucxhPARSER_isValidIpV4Address(const char *s)
{

  bool res = ucxhPARSER_isIpV4Address(s);

  if (res && strcmp(s, "0.0.0.0") == 0) {
    return false;
  }

  return true;
}

bool ucxhPARSER_isIpV6Address(const char *s)
{
  // TODO: Implement ucxhPARSER_isIpV6Address
  return true;
}

bool ucxhPARSER_isValidIpV6Address(const char *s)
{
  // TODO: Implement ucxhPARSER_isValidIpV6Address
  return true;
}

bool ucxhMODULE_setOnNetworkStationUp(int (*callback)(ucxhMODULE_urcNetworkStationStatusEvent_t *pEvent))
{
  ucxhMODULE_onNetworkStationUp = callback;
  return true;
}


bool ucxhMODULE_setOnNetworkStationDown(int (*callback)(ucxhMODULE_urcNetworkStationStatusEvent_t *pEvent))
{
  ucxhMODULE_onNetworkStationDown = callback;
  return true;
}

