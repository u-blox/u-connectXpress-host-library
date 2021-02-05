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
#include <time.h>
#include <unistd.h>

#include "ucxh_library.h"
#include "ucxh_serial.h"

#include "ex_config.h"

/** 
 * \file ex_http_get.c
 * \example ex_http_get Using EDM, configure WiFi as a WiFi Station, connect to a http:// server, request GET
 * \brief  Using EDM, configure WiFi as a WiFi Station, connect to a http:// server, request GET
 
## Introduction
 
This example is a complete example, which shows how to

 * Start the Serial Port
 * Configure the module to autoconnect to a server
 * Enter EDM
 * Wait for connection to remote server
 * Send and receive data over EDM

Once configured, the example is driven by events, which are
handled by the callbacks onConnectIpV4(), onData() and onDisconnect(). 

A number of other callbacks, are registered as well, but these are only done 
to show how AT URCs and responses are reported as callbacks, and
that some that require more intelligence, such as onNetworkStationUp() which
indicates that the station has received its IP adress, also are reported as callbacks.

Full source in `examples/ex_http_get.c`

## Typical response

TBD

*/


void onExit(void) {
  ucxhSERIAL_close();
}


static volatile int lastChannel = -1;
static volatile int numberOfConnections = 0;
static volatile enum {
  EXAMPLE_STATE_WAITING,
  EXAMPLE_STATE_STATION_UP,
  EXAMPLE_STATE_CONNECTED,
  EXAMPLE_STATE_SEND_REQUEST,
  EXAMPLE_STATE_DISCONNECTED,
  EXAMPLE_STATE_QUIT
} exampleState = EXAMPLE_STATE_WAITING;

//
// EDM-only callbacks. These drives the mainloop.
//


int onStartup(void) {
  // This is actually only issued in case the module has been configured to start in EDM mode.

  printf("ex_http_get.c/onStartup: Startup event\n");

  return 0;
}


int onConnectIpV4(const ucxhEDM_connectionEventIPv4_t *event) 
{
  assert(event != NULL);


  printf("ex_http_get.c/onConnectIpV4: Channel %d: Connection event to localport %u from %u.%u.%u.%u:%u, protocol %s, lastChannel=%d\n",
    event->hChannel, 
    event->localPort,
    event->remoteAddress[0],
    event->remoteAddress[1],
    event->remoteAddress[2],
    event->remoteAddress[3],
    event->remotePort,
      (event->protocol == ucxhEDM_IP_PROTOCOL_TCP ? "TCP" : // NB: This example only uses TCP
      (event->protocol == ucxhEDM_IP_PROTOCOL_UDP ? "UDP" : 
      (event->protocol == ucxhEDM_IP_PROTOCOL_MQTT ? "MQTT" : "UNKNOWN"))),
    lastChannel
  );

  if (event->localPort != 0 && event->remotePort != 0) {
    lastChannel = event->hChannel;
    numberOfConnections ++;

    if (exampleState != EXAMPLE_STATE_CONNECTED) {
      exampleState = EXAMPLE_STATE_CONNECTED;
    }
  }

  return 0;
}

int onData(int channel, const uint8_t *data, size_t length)
{
  assert(data != NULL);

  printf("ex_http_get.c/onData: Channel %d: Data event of %d bytes: '", channel, length);
  ucxhPARSER_printBuffer(data, length);
  printf("'\n");

  //... Handle the incoming data here...  

  if (strncmp(data, "quit", 4) == 0) {
    exampleState = EXAMPLE_STATE_QUIT;
  } else {
    // Simply echo back.
    const char *echoText = "Server sent '";
    ucxhEDM_sendData(channel, echoText, strlen(echoText));
    ucxhEDM_sendData(channel, data, length);
    const char *echoText2 = "' to me.\r\n";
    ucxhEDM_sendData(channel, echoText2, strlen(echoText2));
  }

  return 0;
}


bool uses_default_peer = false;
int onDisconnect(const ucxhEDM_disconnectEvent_t *pEvent)
{
  assert(pEvent != NULL);

  printf("ex_http_get.c/onDisconnect: Channel %d: Disconnect event, lastChannel=%d, numberOfConnections=%d\n", pEvent->hChannel, lastChannel, numberOfConnections);

  lastChannel = -1;

  if (uses_default_peer) {
    assert(numberOfConnections > 0);
  } else if (numberOfConnections > 0) {
    numberOfConnections --;
  }

  if (numberOfConnections == 0) {
    exampleState = EXAMPLE_STATE_DISCONNECTED;
  }

  return 0;
}


//
// Onther useful callbacks
//


const char *interfaceTypeToString( ucxhPARSER_interfaceType_t interfaceType)
{
  const char *interfaceTypeName[] = {
    "UNKNOWN",
    "WIFI_STATION",
    "WIFI_AP",
    "ETHERNET",
    "PPP",
    "BRIDGE",
    "PAN"
  };

  if (interfaceType < ELEMENTS_OF(interfaceTypeName)) {
    return interfaceTypeName[interfaceType];
  } else {
    return "UNDEFINED";
  }
}

static void printNetworkStatusEvent(ucxhPARSER_urcNetworkStatusEvent_t *pEvent)
{
  assert(pEvent != NULL);

  printf("  interface_hardware_address: '%s'\n",  pEvent->interface_hardware_address);
  printf("  network_status: %u\n",                pEvent->network_status);
  printf("  interface_id: %u\n",                  pEvent->interface_id);
  printf("  interface_type: %u (%s)\n",           pEvent->interface_type, interfaceTypeToString(pEvent->interface_type));
  printf("  ipv4_address: '%s'\n",                pEvent->ipv4_address);
  printf("  ipv4_subnet_mask: '%s'\n",            pEvent->ipv4_subnet_mask);
  printf("  ipv4_gateway: '%s'\n",                pEvent->ipv4_gateway);
  printf("  primary_dns_server: '%s'\n",          pEvent->primary_dns_server);
  printf("  secondary_dns_server: '%s'\n",        pEvent->secondary_dns_server);
  printf("  ipv6_link_local_address: '%s'\n",     pEvent->ipv6_link_local_address);
  for (int i = 0; i < ELEMENTS_OF(pEvent->ipv6_address); i++) {
    printf("  ipv6_address[%d]: '%s', %u\n", i, pEvent->ipv6_address[i].ipv6_address, pEvent->ipv6_address[i].ipv6_status_state);
  }
}

int onNetworkStatus(ucxhPARSER_urcNetworkStatusEvent_t *pEvent)
{
  assert(pEvent != NULL);

  printf("ex_http_get.c/onNetworkStatus:\n");
  printNetworkStatusEvent(pEvent);

  return true;
}

int onNetworkStationUp(ucxhMODULE_urcNetworkStationStatusEvent_t *pEvent)
{
  assert(pEvent != NULL);
  assert(pEvent->interface_type == ucxhURC_INTERFACE_TYPE_WIFI_STATION);

  printf("ex_http_get.c/onNetworkStationUp:\n");
  printNetworkStatusEvent(pEvent);

  exampleState = EXAMPLE_STATE_STATION_UP;

  return true; 
}

int onNetworkStationDown(ucxhMODULE_urcNetworkStationStatusEvent_t *pEvent)
{
  assert(pEvent != NULL);
  assert(pEvent->interface_type == ucxhURC_INTERFACE_TYPE_WIFI_STATION);

  // This is invoked periodically as a result of AT+UWSCA=3 until
  // the WiFi Station is actually up.

  printf("ex_http_get.c/onNetworkStationDown:\n");
  printNetworkStatusEvent(pEvent);

  return true; 
}


int onPeerConnected(ucxhPARSER_urcPeerConnectedEvent_t *pEvent)
{
  assert(pEvent != NULL);

  // In case of AT mode, this event does not always arrive.
  
  printf("ex_http_get.c/onPeerConnected: peer_handle %u\n", pEvent->peer_handle);
  switch (pEvent->type) {
    case ucxhURC_PEER_TYPE_IPV4:
      printf("  IPv4 local_port: %u\n",    pEvent->ipv4_peer.local_port);
      printf("  IPv4 remote_port: %u\n",   pEvent->ipv4_peer.remote_port);
      printf("  IPv4 protocol: %u (%s)\n", pEvent->ipv4_peer.protocol, 
        (pEvent->ipv4_peer.protocol == ucxhURC_IP_PROTOCOL_TCP ? "TCP" :
        (pEvent->ipv4_peer.protocol == ucxhURC_IP_PROTOCOL_UDP ? "UDP" :
        ("UNKNOWN")))
      );
      printf("  IPv4 local_address: '%s'\n", pEvent->ipv4_peer.local_address);
      printf("  IPv4 remote_address: '%s'\n", pEvent->ipv4_peer.remote_address);
      break;

    case ucxhURC_PEER_TYPE_IPV6:
      printf("  IPv6 local_port: %u\n",    pEvent->ipv6_peer.local_port);
      printf("  IPv6 remote_port: %u\n",   pEvent->ipv6_peer.remote_port);
      printf("  IPv6 protocol: %u (%s)\n", pEvent->ipv6_peer.protocol, 
        (pEvent->ipv6_peer.protocol == ucxhURC_IP_PROTOCOL_TCP ? "TCP" :
        (pEvent->ipv6_peer.protocol == ucxhURC_IP_PROTOCOL_UDP ? "UDP" :
        ("UNKNOWN")))
      );
      printf("  IPv6 local_address: '%s'\n", pEvent->ipv6_peer.local_address);
      printf("  IPv6 remote_address: '%s'\n", pEvent->ipv6_peer.remote_address);
      break;

    case ucxhURC_PEER_TYPE_BLUETOOTH:
    default:
      printf("ex_http_get.c/onPeerConnected: Unimplemented peer type %u\n", pEvent->type);
      return true;
  }
  return true;
}


int onPeerDisconnected(ucxhPARSER_urcPeerDisconnectedEvent_t *pEvent)
{
  assert(pEvent != NULL);

  printf("ex_http_get.c/onPeerDisconnected peer_handle %u\n", pEvent->peer_handle);

  return true;
}


int onNetworkDown(ucxhPARSER_urcNetworkDownEvent_t *pEvent)
{
  assert(pEvent != NULL);

  printf("ex_http_get.c/onNetworkDown interface_id %u\n", pEvent->interface_id);

  return true;
}


int onNetworkUp(ucxhPARSER_urcNetworkUpEvent_t *pEvent)
{
  assert(pEvent != NULL);

  printf("ex_http_get.c/onNetworkUp interface_id %u\n", pEvent->interface_id);

  return true;
}


int onWifiLinkDisconnected(ucxhPARSER_urcWifiLinkDisconnectedEvent_t *pEvent)
{
  assert(pEvent != NULL);

  const char *reasonStr = "UNDEFINED";

  switch (pEvent->reason) {
    case URC_WIFI_DISCONNECT_REASON_UNKNOWN:          reasonStr = "UNKNOWN"; break;
    case URC_WIFI_DISCONNECT_REASON_REMOTE_CLOSE:     reasonStr = "REMOTE_CLOSE"; break;
    case URC_WIFI_DISCONNECT_REASON_OUT_OF_RANGE:     reasonStr = "OUT_OF_RANGE"; break;
    case URC_WIFI_DISCONNECT_REASON_ROAMING:          reasonStr = "ROAMING"; break;
    case URC_WIFI_DISCONNECT_REASON_SECURITY_PROBLEM: reasonStr = "SECURITY_PROBLEM"; break;
    case URC_WIFI_DISCONNECT_REASON_NETWORK_DISABLED: reasonStr = "NETWORK_DISABLED"; break;
  }

  printf("ex_http_get.c/onWifiLinkDisconnected: connection_id %u, reason %u (%s)\n", pEvent->connection_id, pEvent->reason, reasonStr);

  return true;
}


int onWifiLinkConnected(ucxhPARSER_urcWifiLinkConnectedEvent_t *pEvent)
{
  assert(pEvent != NULL);

  printf("ex_http_get.c/onWifiLinkConnected:                        connection_id:        %d\n"
         "                                                          BSSID:                '%s'\n"
         "                                                          WiFi channel:         %d\n",
         pEvent->connection_id, pEvent->bssid, pEvent->channel);

  return true;
}


int onWifiStationStatus(ucxhPARSER_urcWifiStationStatusEvent_t *pEvent)
{
  assert(pEvent != NULL);

  printf("ex_http_get.c/onWifiStationStatus: TODO: Print content\n");

  return true;
}

ucxhPARSER_urcIdentificationInformationApplicationVersionStr_t application_version;
int onIdentificationInformation(ucxhPARSER_urcIdentificationInformationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    printf("ex_http_get.c/onIdentificationInformation:                Application Version:  '%s'\n"
           "                                                          Unique Identifier:    '%s'\n",
           pEvent->application_version, pEvent->unique_identifier);

    strcpy(application_version, pEvent->application_version);
    return true;
}


ucxhPARSER_urcModelIdentificationStr_t module_model;
int onModelIdentification(ucxhPARSER_urcModelIdentificationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    printf("ex_callbacks_edm_wifi.c/onModelIdentification:            Model:                '%s'\n", pEvent->model);
    strcpy(module_model, pEvent->model);

    return true;
}

ucxhPARSER_urcIdentificationInformationApplicationVersionStr_t module_software_version;
int onSoftwareVersionIdentification(ucxhPARSER_urcSoftwareVersionIdentificationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    printf("ex_callbacks_edm_wifi.c/onSoftwareVersionIdentification:  Software Version:     '%s'\n", pEvent->software_version);
    strcpy(module_software_version, pEvent->software_version);
    return true;
}

int onLocalAddress(ucxhPARSER_urcLocalAddressEvent_t *pEvent)
{
    assert(pEvent != NULL);

    const char *hardware_interface_name ="UNKNOWN";

    switch (pEvent->interface_id) {
      case ucxhPARSER_HARDWARE_INTERFACE_BLUETOOTH:     hardware_interface_name = "Bluetooth"; break;
      case ucxhPARSER_HARDWARE_INTERFACE_WIFI_STATION:  hardware_interface_name = "WiFi STA"; break;
      case ucxhPARSER_HARDWARE_INTERFACE_ETHERNET:      hardware_interface_name = "Ethernet"; break;
      case ucxhPARSER_HARDWARE_INTERFACE_WIFI_AP:       hardware_interface_name = "WiFI AP"; break;
    }

    printf("ex_http_get.c/onLocalAddress:                   Interface_id:         %d (%s)\n"
           "                                                          MAC Address           '%s'\n",
           pEvent->interface_id, hardware_interface_name, pEvent->mac_address);

    return true;
}


int onOkReceived(const char *lastcommand)
{
  assert(lastcommand != NULL);

  printf("ex_http_get.c/onOkReceived: last command '%s'\n", lastcommand);
}

int onErrorReceived(const char *lastcommand)
{
  assert(lastcommand != NULL);

  printf("ex_http_get.c/onErrorReceived: last command '%s'\n", lastcommand);
}

//
// A catch-all callback.
//


int onUnknownURC(const char *urc)
{
  assert(urc != NULL);

  if(strlen(urc) > 0) {
    // ... Act on incoming URCs and other stuff that weren't understood by the AT parser.

    // Note: EDM does not echo AT commands.

    // Remember that URCs that are understood by the AT parser will not 
    // invoke this callback. Instead, they invoke the callback either registered by 
    // ucxhPARSER_setOnURCParsed() or the the URC's corresponding ucxhPARSER_setOnXXX().

    printf("ex_http_get.c/onUnknownURC: '%s'\n", urc);
  }
}

#define DEFAULT_URL "www.google.com"
//#define DEFAULT_URL "www.u-blox.com"
#define GET_COMMAND "GET /%s HTTP/1.1\n"
#define HOST_HEADER "Host: %s\n"
#define CONNECTION_KEEP_ALIVE_HEADER "Connection: keep-alive\n"
#define USER_AGENT_HEADER "User-Agent: u-curl/0.1;u-connectXpress-%s/%s\n"
#define ACCEPT_HEADER "Accept: */*\n"
#define HTTP "http://"
#define HTTPS "https://"

char *get_command = NULL;
char *url = NULL;
const char *domain = NULL;
const char *portstr = NULL;
const char *path = NULL;

char *host_header = NULL;
char *user_agent_header = NULL;
int port_number = -1;
const char *ap_ssid = AP_SSID;
const char *ap_passphrase = AP_PASSWORD;
int timeout_seconds = -1;

void print_usage(void)
{
  printf("Usage:\n");
  printf("ucurl [--ssid <ssid>] [--passphrase <password>] <URL>\n");
}


char *parse_url(const char *param)
{
  size_t url_len = strlen(param);
  url = malloc(url_len+1);
  assert(url != NULL);
  strcpy(url, param);

  bool is_https = false;
  bool is_http = false;

  for (int i = 0; i < url_len; i++) {
    if (domain == NULL) {
      if (i < strlen(HTTP) && tolower(url[i]) == HTTP[i]) {
        // eat it
        is_http = true;
      } else if (i < strlen(HTTPS) && tolower(url[i]) == HTTPS[i]) {
        // 
        is_https = true;
      } else if (i == strlen(HTTP) && is_http) {
        // is an HTTP url starting at url[i]
        domain = &url[i];
      } else if (i == strlen(HTTPS) && is_https) {
        // is an HTTPS url starting at url[i]
        domain = &url[i];
      } else if (i < strlen(HTTP)) {
        domain = url;
      } else {
        fprintf(stderr, "ERROR: Malformed domain '%s'\n", url);
        exit(EXIT_FAILURE);
      }
    } else if (url[i] == ':') {
      portstr = &url[i+1];
      url[i] = '\0';
    } else if (url[i] == '/') {
      path = &url[i+1];
      url[i] = '\0';
      break;
    }
  }

  if (portstr == NULL || portstr == "") {
    portstr = is_https ? "443" : "80";
  }

  assert(domain != NULL);
  if (path == NULL) {
    path = "";
  }

  if (path[0] == '/') {
    //  abc://sdf.com/ causes the '/sdf.com/' to become path and 'abc' to be domain
    fprintf(stderr, "ERROR: Unknown protocol '%s'", url);
    exit(EXIT_FAILURE);
  }

  if (strchr(domain, '.') == NULL || strlen(domain) < 4) {
      // Must at least be a.bc or 0.0.0.0
      fprintf(stderr, "ERROR: Malformed domain '%s'\n", domain);
      exit(EXIT_FAILURE);
  }

  if (strlen(domain) >= sizeof(uxchAT_urlHostStr_t)) {
    fprintf(stderr, "ERROR: Domain contains more than %d characters!\n", sizeof(uxchAT_urlHostStr_t)-1);
    exit(EXIT_FAILURE);
  }

  int res = sscanf(portstr, "%u", &port_number);
  if (res != 1 || port_number < 1 || port_number > 65535) {
    fprintf(stderr, "ERROR: Invalid portstr '%s'!\n", portstr);
    exit(EXIT_FAILURE);
  }
  
  printf("GET using protocol %s from domain '%s' port %u path '/%s'\n",
    is_https ? "HTTPS" : "HTTP",
    domain,
    port_number,
    path
  );

  get_command = malloc(sizeof(GET_COMMAND) + strlen(path));
  assert(get_command != NULL);
  sprintf(get_command, GET_COMMAND, path);

  host_header = malloc(sizeof(HOST_HEADER) + strlen(domain));
  assert(host_header != NULL);
  sprintf(host_header, HOST_HEADER, domain);

  return url;
}


void parse_args(int argc, const char *argv[])
{
  if (argc < 1) {
    print_usage();
    exit(EXIT_FAILURE);
  }

  for (int a = 1; a < argc; a++) {
    if        (strcmp(argv[a], "--ssid")       == 0 && a+1 < argc) {
      ap_ssid = argv[++a];
    } else if (strcmp(argv[a], "--password")   == 0 && a+1 < argc) {
      ap_passphrase = argv[++a];
    } else if (strcmp(argv[a], "--passphrase") == 0 && a+1 < argc) {
      ap_passphrase = argv[++a];
    } else if (strcmp(argv[a], "--timeout")   == 0 && a+1 < argc) {
      if (sscanf(argv[++a], "%d", &timeout_seconds) != 1) {
        timeout_seconds = -1;
      }
    } else if (url == NULL) {
      if (a < argc) {
        url = parse_url(argv[a]);
        assert(url != NULL);
      }
    } else {
      print_usage();
      exit(EXIT_FAILURE);
    }
  }
}


int main(int argc, const char *argv[])
{
  bool result = false;

  parse_args(argc, argv);

  if(url == NULL) {
    url = parse_url(DEFAULT_URL);
  }

  assert(url != NULL);
  assert(domain != NULL);
  assert(ap_ssid != NULL);
  assert(ap_passphrase != NULL);
  assert(strlen(url) > 0);
  assert(strlen(domain) > 0);
  assert(strlen(ap_ssid) > 0);
  assert(strlen(ap_passphrase) > 0);

  // printf("url='%s'\n", url);
  // printf("domain='%s'\n", domain);
  // printf("port_number=%d\n", port_number);
  // printf("ap_ssid='%s'\n", ap_ssid);
  // printf("ap_passphrase='%s'\n", ap_passphrase);

  // To make sure the Windows serial port is closed in case of premature exit (aka assert())
  atexit(onExit);
  
  // Register callbacks that are mandatory for this example
  ucxhPARSER_setOnDataReceived(onData);
  ucxhEDM_setOnConnectIpV4Received(onConnectIpV4);
  ucxhEDM_setOnDisconnectReceived(onDisconnect);


  // Register other callbacks that can be of use. In this example, they'll only print the events.
  ucxhEDM_setOnStartupReceived(onStartup);

  ucxhPARSER_setOnNetworkStatus(onNetworkStatus);
  ucxhPARSER_setOnPeerConnected(onPeerConnected);
  ucxhPARSER_setOnPeerDisconnected(onPeerDisconnected);
  ucxhPARSER_setOnNetworkDown(onNetworkDown);
  ucxhPARSER_setOnNetworkUp(onNetworkUp);
  ucxhPARSER_setOnWifiLinkDisconnected(onWifiLinkDisconnected);
  ucxhPARSER_setOnWifiLinkConnected(onWifiLinkConnected);
  ucxhPARSER_setOnWifiStationStatus(onWifiStationStatus);
  ucxhMODULE_setOnNetworkStationUp(onNetworkStationUp);
  ucxhMODULE_setOnNetworkStationDown(onNetworkStationDown);

  ucxhPARSER_setOnModelIdentification(onModelIdentification);
  ucxhPARSER_setOnSoftwareVersionIdentification(onSoftwareVersionIdentification);
  ucxhPARSER_setOnIdentificationInformation(onIdentificationInformation);

  // And some catch-all callbacks. They, too only print the event in this example.
  ucxhPARSER_setOnUnknownURCReceived(onUnknownURC);
  ucxhPARSER_setOnErrorReceived(onErrorReceived);
  // ucxhPARSER_setOnOkReceived(onOkReceived); // Normally not of interest, but there is the possibility
  // ucxhPARSER_setOnURCParsed(onUnknownURC); // Normally not of interest, but there is the possibility


  //
  // Conduct typical boot-time initialization.  
  //

  printf("Opening serial port...\n");  
  result = ucxhSERIAL_open();
  assert(result == true); // Likely in use by another program...

  printf("Entering AT mode (in case NINA was in DATA mode)...\n");
  result = ucxhMODULE_enterAtMode();
  assert(result == true); //The module is likely already in Extended Data Mode!

  printf("Handling stray responses\n");
  ucxhPARSER_wait(1);   // 1 second is really long, but that's the timer's resolution right now. 
  
  printf("Entering extended data mode (EDM)... ");
  result = ucxhMODULE_enterExtendedDataMode();
  assert(result == true); // The module is likely already in Extended Data Mode!

  printf("Handling stray responses again\n");
  ucxhPARSER_wait(1);  // 1 second is really long, but that's the timer's resolution right now.

  printf("Query NINA...\n");

  // All commands are always queued and transmitted one at a time, each one
  // waiting in the mainloop until the OK or ERROR response from the
  // previous command has been received. 

  result &= ucxhCOMMAND_readModelIdentification();
  result &= ucxhCOMMAND_readSoftwareVersionIdentification();
  result &= ucxhCOMMAND_readIdentificationInformation();
  
  printf("Configuring WiFi for SSID '%s' with passphrase '%s'...\n", ap_ssid, ap_passphrase);
  result &= ucxhMODULE_deactivateWifiStation();
  ucxhPARSER_wifiStationConfiguration_t cfg = {
    .authentication       = ucxhPARSER_WIFI_STATION_AUTHENTICATION_WPA_WPA2,
  };
  strncpy(cfg.ssid, ap_ssid, sizeof(cfg.ssid));
  strncpy(cfg.passphrase, ap_passphrase, sizeof(cfg.passphrase));
  result &= ucxhMODULE_writeWifiStationConfiguration(&cfg);
  assert(result == true);
  
  printf("Activating WiFi...\n");
  result &= ucxhMODULE_activateWifiStation();

#if 0
  if (uses_default_peer) {
    ucxhPARSER_defaultRemotePeerParam_t remote_peer = {
      .url.scheme = ucxhPARSER_PEER_URL_SCHEME_TCP,
      .url.tcp.domain = REMOTE_PEER_IP,
      .url.tcp.remote_port = REMOTE_PEER_PORT,
      .url.tcp.query.flush_tx = true,
      .url.tcp.query.keep_alive.idle_ms = 5000,
      .url.tcp.query.keep_alive.interval_ms = 1000,
      .url.tcp.query.keep_alive.count  = 5,
      .connect_scheme.always_connected = true,
    };
    result &= ucxhCOMMAND_writeDefaultRemotePeer(&remote_peer);
  }
#endif
  assert(result == true);
  assert(uses_default_peer == false);

  // Lets start the main loop here...
  

  if (timeout_seconds <= 0) {
    timeout_seconds = 120;
  }
  int32_t numSeconds = timeout_seconds;  
  printf("\n\n\n\nPolling for up to %us. Ensure a TCP server at %s is listening on port %u...\n\n\n\n", numSeconds, domain, port_number);

  time_t until = time(NULL) + numSeconds;

  int retries_left = 1; // Don't retry a disconnected connection.

  do {
      // In a real-world application, ucxhPARSER_poll() would be invoked in the superloop or a designated reader task
      ucxhPARSER_poll();

      // Let's have a simple statemachine driving the example.
      // `exampleState` and `lastChannel` is modified when a connection is detected/dropped
      // by  onConnectIpV4(), onDisconnect() and onData().
      // The rest of the callbacks are only printing the events for informative purposes.
      switch (exampleState) {
        case EXAMPLE_STATE_WAITING:
          break;

        case EXAMPLE_STATE_STATION_UP:
          printf("\n\n\nMain loop detected WiFi Station interface configured and up.\n\n");
          if (!uses_default_peer) {
            printf("\nManually trying to connect to %s:%u\n", domain, port_number);
            ucxhPARSER_peerURL_t remote_peer = {
              .scheme = ucxhPARSER_PEER_URL_SCHEME_TCP,
              .tcp.remote_port = port_number,
              .tcp.query.flush_tx = true,
              .tcp.query.keep_alive.idle_ms = 5000,
              .tcp.query.keep_alive.interval_ms = 1000,
              .tcp.query.keep_alive.count  = 5,
            };
            strcpy(remote_peer.tcp.domain, domain);
            result &= ucxhCOMMAND_connectPeer(&remote_peer);
            assert(result == true);
          } else {
            // UDDRP will try to reconnect
          }
          exampleState = EXAMPLE_STATE_WAITING;

          break;

        case EXAMPLE_STATE_CONNECTED:
          assert(path != NULL);
          printf("\n\n\nMain loop detected connection on channel %d: Transmitting 'GET %s HTTP/1.1' from mainloop.\n\n", lastChannel, path[0] != '\0' ? path : "/");
          exampleState = EXAMPLE_STATE_SEND_REQUEST;
          break;

        case EXAMPLE_STATE_SEND_REQUEST:
          assert(lastChannel >= 0);
          if (user_agent_header == NULL) {
            user_agent_header = malloc(strlen(USER_AGENT_HEADER) + strlen(module_model) + strlen(module_software_version));
            assert (user_agent_header != NULL);
            sprintf(user_agent_header, USER_AGENT_HEADER, module_model, module_software_version);
          }

          assert(get_command != NULL);
          assert(host_header != NULL);
          assert(user_agent_header != NULL);
          
          ucxhEDM_sendData(lastChannel, get_command, strlen(get_command));
          ucxhEDM_sendData(lastChannel, host_header, strlen(host_header));
          ucxhEDM_sendData(lastChannel, user_agent_header, strlen(user_agent_header));
          ucxhEDM_sendData(lastChannel, CONNECTION_KEEP_ALIVE_HEADER, strlen(CONNECTION_KEEP_ALIVE_HEADER));
          ucxhEDM_sendData(lastChannel, "\n", 1);

          printf("Waiting for timeout or closed connection....\n\n\n");

          exampleState = EXAMPLE_STATE_WAITING;
          break;

        case EXAMPLE_STATE_DISCONNECTED:
          retries_left --;
          if (retries_left <= 0) {
            printf("\n\nMain loop detected disconnection. Abort\n");
            exampleState = EXAMPLE_STATE_QUIT;
          } else if (uses_default_peer) {
            printf("\n\nMain loop detected disconnection. UDDRP will try to reconnect\n");
            exampleState = EXAMPLE_STATE_WAITING;
          } else {
            printf("\n\nMain loop detected disconnection. Let's manually try to reconnect again...\n");
            exampleState = EXAMPLE_STATE_STATION_UP;
          }
          break;

        case EXAMPLE_STATE_QUIT:
          printf("\n\nMain loop detected 'quit'\n");
          break;
      }
  } while (time(NULL) < until && exampleState != EXAMPLE_STATE_QUIT);


  printf("Restarting module\n");
  ucxhMODULE_restart();

  printf("Example completed\n");
  ucxhSERIAL_close();

  return 0;
}
