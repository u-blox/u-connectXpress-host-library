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
 * \file ex_callbacks_edm_wifi.c
 * \example ex_callbacks_edm_wifi Using EDM, configure WiFi and transmit TCP data as a WiFi Station
 * \brief Using EDM, configure WiFi and transmit TCP data as a WiFi Station
 
## Introduction
 
This example is a complete example, which shows how to

 * Start the Serial Port
 * Configure the module to autoconnect to a server
 * Enter EDM
 * Wait for connection to server 
 * Send and receive data over EDM

Once configured, the example is driven by events, which are
handled by the callbacks onConnectIpV4(), onData() and onDisconnect(). 

A number of other callbacks, are registered as well, but these are only done 
to show how AT URCs and responses are reported as callbacks, and
that some that require more intelligence, such as onNetworkStationUp() which
indicates that the station has received its IP adress, also are reported as callbacks.

Full source in `examples/ex_callbacks_edm_wifi.c`

## Typical response

    Opening serial port...
    Entering AT mode (in case NINA was in DATA mode)...
    ex_callbacks_edm_wifi.c/onUnknownURC: '+STARTUP'
    Handling stray responses
    ex_callbacks_edm_wifi.c/onUnknownURC: 'AT'
    Entering extended data mode (EDM)... ex_callbacks_edm_wifi.c/onUnknownURC: 'ATO2'
    Handling stray responses again
    Query NINA...
    Configuring WiFi...
    Activating WiFi...




    Polling for up to 120s. Ensure a TCP server at 192.168.2.1 is listening on port 12345...



    ex_callbacks_edm_wifi.c/onManufacturerIdentification:     Manufacturer:         'u-blox'
    ex_callbacks_edm_wifi.c/onModelIdentification:            Model:                'NINA-W132'
    ex_callbacks_edm_wifi.c/onIdentificationInformation:      Application Version:  '2.1.0-017'
                                                              Unique Identifier:    'NINA-W13X-2.1.0-017-0-g253211b6'
    ex_callbacks_edm_wifi.c/onSoftwareVersionIdentification:  Software Version:     '2.1.0-017'
    ex_callbacks_edm_wifi.c/onLocalAddress:                   Interface_id:         2 (WiFi STA)
                                                              MAC Address           'D4CA6EFD96E0'
    ex_callbacks_edm_wifi.c/onWifiLinkConnected:              connection_id:        0
                                                              BSSID:                'D4CA6E889F82'
                                                              WiFi channel:         6
    ex_callbacks_edm_wifi.c/onNetworkUp interface_id 0
    ex_callbacks_edm_wifi.c/onNetworkStatus:
      interface_hardware_address: 'D4CA6EFD96E0'
      network_status: 1
      interface_id: 0
      interface_type: 1 (WIFI_STATION)
      ipv4_address: '192.168.2.101'
      ipv4_subnet_mask: '255.255.255.0'
      ipv4_gateway: '192.168.2.1'
      primary_dns_server: '192.168.2.1'
      secondary_dns_server: '0.0.0.0'
      ipv6_link_local_address: '::'
      ipv6_address[0]: 'FE80::D6CA:6EFF:FEFD:96E0', 1
      ipv6_address[1]: '::', 0
      ipv6_address[2]: '::', 0
    ex_callbacks_edm_wifi.c/onNetworkStatus:
      interface_hardware_address: '000000000000'
      network_status: 0
      interface_id: 13
      interface_type: 5 (BRIDGE)
      ipv4_address: '0.0.0.0'
      ipv4_subnet_mask: '0.0.0.0'
      ipv4_gateway: '0.0.0.0'
      primary_dns_server: '0.0.0.0'
      secondary_dns_server: '0.0.0.0'
      ipv6_link_local_address: '::'
      ipv6_address[0]: '::', 0
      ipv6_address[1]: '::', 0
      ipv6_address[2]: '::', 0
    ex_callbacks_edm_wifi.c/onNetworkStatus:
      interface_hardware_address: '000000000000'
      network_status: 0
      interface_id: 14
      interface_type: 5 (BRIDGE)
      ipv4_address: '0.0.0.0'
      ipv4_subnet_mask: '0.0.0.0'
      ipv4_gateway: '0.0.0.0'
      primary_dns_server: ''
      secondary_dns_server: ''
      ipv6_link_local_address: ''
      ipv6_address[0]: '', 0
      ipv6_address[1]: '', 0
      ipv6_address[2]: '', 0
    ex_callbacks_edm_wifi.c/onNetworkUp interface_id 0
    ex_callbacks_edm_wifi.c/onNetworkStationUp:
      interface_hardware_address: 'D4CA6EFD96E0'
      network_status: 1
      interface_id: 0
      interface_type: 1 (WIFI_STATION)
      ipv4_address: '192.168.2.101'
      ipv4_subnet_mask: '255.255.255.0'
      ipv4_gateway: '192.168.2.1'
      primary_dns_server: '192.168.2.1'
      secondary_dns_server: '0.0.0.0'
      ipv6_link_local_address: '::'
      ipv6_address[0]: 'FE80::D6CA:6EFF:FEFD:96E0', 1
      ipv6_address[1]: '::', 0
      ipv6_address[2]: '::', 0



    Main loop detected WiFi Station interface configured and up.


    Manually trying to connect to 192.168.2.1:12345
    ex_callbacks_edm_wifi.c/onConnectIpV4: Channel 0: Connection event to localport 62809 from 192.168.2.1:12345, protocol TCP, lastChannel=-1



    Main loop detected connection on channel 0: Transmitting 'Hello, World' from mainloop.

    Send anything or 'quit' to us, shutdown server or wait for timeout


    ex_callbacks_edm_wifi.c/onPeerConnected: peer_handle 0
      IPv4 local_port: 62809
      IPv4 remote_port: 12345
      IPv4 protocol: 0 (TCP)
      IPv4 local_address: '192.168.2.101'
      IPv4 remote_address: '192.168.2.1'
    ex_callbacks_edm_wifi.c/onData: Channel 0: Data event of 7 bytes: 'Howdy!↵'
    ex_callbacks_edm_wifi.c/onData: Channel 0: Data event of 5 bytes: 'quit↵'


    Main loop detected 'quit'
    Restarting module
    Example completed

  The server (netcat) would have shown

    $ nc -lk 12345
    Hello, World!
    Howdy!
    Server sent 'Howdy!' to me.
    quit

  In s-center, it would have been displayed as 

    Hello, World!
    Server sent 'Howdy!' to me.

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
  EXAMPLE_STATE_DISCONNECTED,
  EXAMPLE_STATE_QUIT
} exampleState = EXAMPLE_STATE_WAITING;

//
// EDM-only callbacks. These drives the mainloop.
//


int onStartup(void) {
  // This is actually only issued in case the module has been configured to start in EDM mode.

  printf("ex_callbacks_edm_wifi.c/onStartup: Startup event\n");

  return 0;
}


int onConnectIpV4(const ucxhEDM_connectionEventIPv4_t *event) 
{
  assert(event != NULL);


  printf("ex_callbacks_edm_wifi.c/onConnectIpV4: Channel %d: Connection event to localport %u from %u.%u.%u.%u:%u, protocol %s, lastChannel=%d\n",
    event->hChannel, 
    event->localPort,
    event->remoteAddress[0],
    event->remoteAddress[1],
    event->remoteAddress[2],
    event->remoteAddress[3],
    event->remotePort,
      (event->protocol == ucxhEDM_IP_PROTOCOL_TCP ? "TCP" : 
      (event->protocol == ucxhEDM_IP_PROTOCOL_UDP ? "UDP" : "UNKNOWN")),
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

  printf("ex_callbacks_edm_wifi.c/onData: Channel %d: Data event of %d bytes: '", channel, length);
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

  printf("ex_callbacks_edm_wifi.c/onDisconnect: Channel %d: Disconnect event, lastChannel=%d, numberOfConnections=%d\n", pEvent->hChannel, lastChannel, numberOfConnections);

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

  printf("ex_callbacks_edm_wifi.c/onNetworkStatus:\n");
  printNetworkStatusEvent(pEvent);

  return true;
}

int onNetworkStationUp(ucxhMODULE_urcNetworkStationStatusEvent_t *pEvent)
{
  assert(pEvent != NULL);
  assert(pEvent->interface_type == ucxhURC_INTERFACE_TYPE_WIFI_STATION);

  printf("ex_callbacks_edm_wifi.c/onNetworkStationUp:\n");
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

  printf("ex_callbacks_edm_wifi.c/onNetworkStationDown:\n");
  printNetworkStatusEvent(pEvent);

  return true; 
}


int onPeerConnected(ucxhPARSER_urcPeerConnectedEvent_t *pEvent)
{
  assert(pEvent != NULL);

  // In case of AT mode, this event does not always arrive.
  
  printf("ex_callbacks_edm_wifi.c/onPeerConnected: peer_handle %u\n", pEvent->peer_handle);
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
      printf("ex_callbacks_edm_wifi.c/onPeerConnected: Unimplemented peer type %u\n", pEvent->type);
      return true;
  }
  return true;
}


int onPeerDisconnected(ucxhPARSER_urcPeerDisconnectedEvent_t *pEvent)
{
  assert(pEvent != NULL);

  printf("ex_callbacks_edm_wifi.c/onPeerDisconnected peer_handle %u\n", pEvent->peer_handle);

  return true;
}


int onNetworkDown(ucxhPARSER_urcNetworkDownEvent_t *pEvent)
{
  assert(pEvent != NULL);

  printf("ex_callbacks_edm_wifi.c/onNetworkDown interface_id %u\n", pEvent->interface_id);

  return true;
}


int onNetworkUp(ucxhPARSER_urcNetworkUpEvent_t *pEvent)
{
  assert(pEvent != NULL);

  printf("ex_callbacks_edm_wifi.c/onNetworkUp interface_id %u\n", pEvent->interface_id);

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

  printf("ex_callbacks_edm_wifi.c/onWifiLinkDisconnected: connection_id %u, reason %u (%s)\n", pEvent->connection_id, pEvent->reason, reasonStr);

  return true;
}


int onWifiLinkConnected(ucxhPARSER_urcWifiLinkConnectedEvent_t *pEvent)
{
  assert(pEvent != NULL);

  printf("ex_callbacks_edm_wifi.c/onWifiLinkConnected:              connection_id:        %d\n"
         "                                                          BSSID:                '%s'\n"
         "                                                          WiFi channel:         %d\n",
         pEvent->connection_id, pEvent->bssid, pEvent->channel);

  return true;
}


int onWifiStationStatus(ucxhPARSER_urcWifiStationStatusEvent_t *pEvent)
{
  assert(pEvent != NULL);

  printf("ex_callbacks_edm_wifi.c/onWifiStationStatus: TODO: Print content\n");

  return true;
}

int onIdentificationInformation(ucxhPARSER_urcIdentificationInformationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    printf("ex_callbacks_edm_wifi.c/onIdentificationInformation:      Application Version:  '%s'\n"
           "                                                          Unique Identifier:    '%s'\n",
           pEvent->application_version, pEvent->unique_identifier);
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

    printf("ex_callbacks_edm_wifi.c/onLocalAddress:                   Interface_id:         %d (%s)\n"
           "                                                          MAC Address           '%s'\n",
           pEvent->interface_id, hardware_interface_name, pEvent->mac_address);

    return true;
}

int onManufacturerIdentification(ucxhPARSER_urcManufacturerIdentificationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    printf("ex_callbacks_edm_wifi.c/onManufacturerIdentification:     Manufacturer:         '%s'\n", pEvent->manufacturer);

    return true;
}

int onModelIdentification(ucxhPARSER_urcModelIdentificationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    printf("ex_callbacks_edm_wifi.c/onModelIdentification:            Model:                '%s'\n", pEvent->model);
    return true;
}

int onSoftwareVersionIdentification(ucxhPARSER_urcSoftwareVersionIdentificationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    printf("ex_callbacks_edm_wifi.c/onSoftwareVersionIdentification:  Software Version:     '%s'\n", pEvent->software_version);
    return true;
}



int onOkReceived(const char *lastcommand)
{
  assert(lastcommand != NULL);

  printf("ex_callbacks_edm_wifi.c/onOkReceived: last command '%s'\n", lastcommand);
}

int onErrorReceived(const char *lastcommand)
{
  assert(lastcommand != NULL);

  printf("ex_callbacks_edm_wifi.c/onErrorReceived: last command '%s'\n", lastcommand);
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

    printf("ex_callbacks_edm_wifi.c/onUnknownURC: '%s'\n", urc);
  }
}


int main()
{
  bool result = false;
  
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
  ucxhPARSER_setOnIdentificationInformation(onIdentificationInformation);
  ucxhPARSER_setOnLocalAddress(onLocalAddress);
  ucxhPARSER_setOnManufacturerIdentification(onManufacturerIdentification);
  ucxhPARSER_setOnModelIdentification(onModelIdentification);
  ucxhPARSER_setOnSoftwareVersionIdentification(onSoftwareVersionIdentification);

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

  result &= ucxhCOMMAND_readManufacturerIdentification();
  result &= ucxhCOMMAND_readModelIdentification();
  result &= ucxhCOMMAND_readIdentificationInformation();
  result &= ucxhCOMMAND_readSoftwareVersionIdentification();
  result &= ucxhCOMMAND_readLocalAddress(ucxhPARSER_HARDWARE_INTERFACE_WIFI_STATION);
  
  printf("Configuring WiFi...\n");
  result &= ucxhMODULE_deactivateWifiStation();
  ucxhPARSER_wifiStationConfiguration_t cfg = {
    .on_startup           = ucxhPARSER_WIFI_STATION_CONFIGURATION_ON_STARTUP_INACTIVE,  // Same as default
    .ssid                 = AP_SSID,
    .authentication       = ucxhPARSER_WIFI_STATION_AUTHENTICATION_WPA_WPA2,
    .passphrase           = AP_PASSWORD,
    .ipv4_mode            = ucxhPARSER_WIFI_IPV4_MODE_DHCP,                             // Same as default
    //.address_conflict     = ucxhPARSER_WIFI_STATION_ADDRESS_CONFLICT_IGNORE,            // Same as default
    .listen_interval      = 0,                                                          // same as default
    .dtim                 = ucxhPARSER_WIFI_STATION_CONFIGURATION_DTIM_ENABLE,          // same as default
  };
  result &= ucxhMODULE_writeWifiStationConfiguration(&cfg);
  assert(result == true);
  
  printf("Activating WiFi...\n");
  result &= ucxhMODULE_activateWifiStation();

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
  assert(result == true);

  // Lets start the main loop here...
  

  int32_t numSeconds = 120;  
  printf("\n\n\n\nPolling for up to %us. Ensure a TCP server at %s is listening on port %u...\n\n\n\n", numSeconds, REMOTE_PEER_IP, REMOTE_PEER_PORT);

  time_t until = time(NULL) + numSeconds;

  do {
      // In a real-world application, ucxhPARSER_poll() would be invoked in the superloop or a designated reader task
      ucxhPARSER_poll();

      // Let's have a simple statemachine driving the example.
      // `exampleState` and `lastChannel` is modified when a connection is detected, dropped
      // or when 'quit' is received by  onConnectIpV4(), onDisconnect() and onData().
      // The rest of the callbacks are only printing the events for informative purposes.
      switch (exampleState) {
        case EXAMPLE_STATE_WAITING:
          break;

        case EXAMPLE_STATE_STATION_UP:
          printf("\n\n\nMain loop detected WiFi Station interface configured and up.\n\n");          
          if (!uses_default_peer) {
            printf("\nManually trying to connect to %s:%u\n", REMOTE_PEER_IP, REMOTE_PEER_PORT);
            ucxhPARSER_peerURL_t remote_peer = {
              .scheme = ucxhPARSER_PEER_URL_SCHEME_TCP,
              .tcp.domain = REMOTE_PEER_IP,
              .tcp.remote_port = REMOTE_PEER_PORT,
              .tcp.query.flush_tx = true,
              .tcp.query.keep_alive.idle_ms = 5000,
              .tcp.query.keep_alive.interval_ms = 1000,
              .tcp.query.keep_alive.count  = 5,
            };
            result &= ucxhCOMMAND_connectPeer(&remote_peer);
            assert(result == true);
          } else {
            // UDDRP will try to reconnect
          }
          exampleState = EXAMPLE_STATE_WAITING;

          break;

        case EXAMPLE_STATE_CONNECTED:
          printf("\n\n\nMain loop detected connection on channel %d: Transmitting 'Hello, World' from mainloop.\n\n", lastChannel);

          const char *helloWorld = "Hello, World!\r\n";
          assert(lastChannel >= 0);
          ucxhEDM_sendData(lastChannel, helloWorld, strlen(helloWorld));

          printf("Send anything or 'quit' to us, shutdown server or wait for timeout\n\n\n");

          exampleState = EXAMPLE_STATE_WAITING;
          break;

        case EXAMPLE_STATE_DISCONNECTED:
          if (uses_default_peer) {
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
