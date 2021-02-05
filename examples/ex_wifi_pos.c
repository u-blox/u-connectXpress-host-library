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
#include "ucxh_wifi.h"

/** 
 * \file ex_wifi_pos.c
 * \example ex_wifi_pos Using EDM, configure WiFi, scan for APs and request position from Thingstream WiFi positioning server using MQTT over TLS
 * \brief Using EDM, configure WiFi, scan for APs and request position from Thingstream WiFi positioning server using MQTT over TLS
 
## Introduction
 
This example is a complete example, which shows how to
 * Start the Serial Port
 * Enter EDM
 * Configure the module to connect to an AP
 * Connect to a Thingstream WiFi positioning server at mqtt.thingstream.io using MQTT over TLS
   and the credentials configured in ex_config.h
 * Scan for nearby APs
 * Publish the list of APs to the Thingstream WiFi positioning server using MQTT
 * Obtain and print the position as determined by the Thingstream WiFi positioning server 

\note In order for this example to work, you must obtain a Thingstream account
and configure the flow for a WiFi positioning server. See the u-connectXpress IoT
cloud connectivity application note at https://www.u-blox.com/en/docs/UBX-19010078 for
more information.

Once configured, this example is driven by events, which are handled by the callbacks
onConnectIpV4(), onNetworkStationUp(), onWifiScanHandler() and onData(). 

A number of other callbacks, are registered as well, but these are only done 
to show how AT URCs and responses are reported as callbacks.

Full source in `examples/ex_wifi_pos.c`

## Typical response

    Opening serial port...
    Entering AT mode (in case NINA was in DATA mode)...
    Handling stray responses
    ex_wifi_pos.c/onUnknownURC: 'AT'
    Entering extended data mode (EDM)... ex_wifi_pos.c/onUnknownURC: 'ATO2'
    Handling stray responses again
    Query NINA...
    Configuring WiFi...
    Activating WiFi...




    Polling for up to 60s. Ensure a TCP server at mqtt.thingstream.io is listening on port 8883...



    ex_wifi_pos.c/onManufacturerIdentification:               Manufacturer:         'u-blox'
    ex_wifi_pos.c/onModelIdentification:                      Model:                'NINA-W152'
    ex_wifi_pos.c/onIdentificationInformation:                Application Version:  '3.0.0-013'
                                                              Unique Identifier:    'NINA-W15X-3.0.0-013-0-g259e42c3'
    ex_wifi_pos.c/onSoftwareVersionIdentification:            Software Version:     '3.0.0-013'
    ex_wifi_pos.c/onLocalAddress:                             Interface_id:         2 (WiFi STA)
                                                              MAC Address           'D4CA6EFD9674'
    ex_wifi_pos.c/onWifiLinkConnected:                        connection_id:        0
                                                              BSSID:                'B03956E9E0D0'
                                                              WiFi channel:         11
    ex_wifi_pos.c/onNetworkUp interface_id 0
    ex_wifi_pos.c/onNetworkStatus:
      interface_hardware_address: 'D4CA6EFD9674'
      network_status: 1
      interface_id: 0
      interface_type: 1 (WIFI_STATION)
      ipv4_address: '192.168.0.128'
      ipv4_subnet_mask: '255.255.255.0'
      ipv4_gateway: '192.168.0.1'
      primary_dns_server: '192.168.0.1'
      secondary_dns_server: '192.168.0.1'
      ipv6_link_local_address: 'FE80::D6CA:6EFF:FEFD:9674'
      ipv6_address[0]: 'FE80::D6CA:6EFF:FEFD:9674', 2
      ipv6_address[1]: '::', 0
      ipv6_address[2]: '::', 0

    ==== Main loop detected WiFi Station interface configured and up. ====


    Manually trying to connect to mqtt.thingstream.io:8883
    ex_wifi_pos.c/onPeerDisconnected peer_handle 2
    ex_wifi_pos.c/onDisconnect: Channel 0: Disconnect event, lastChannel=-1, numberOfConnections=0


    Main loop detected disconnection. Let's manually try to reconnect again...



    ==== Main loop detected WiFi Station interface configured and up. ====


    Manually trying to connect to mqtt.thingstream.io:8883
    ex_wifi_pos.c/onConnectIpV4: Channel 1: Connection event to localport 0 from 52.211.238.211:8883, protocol MQTT, lastChannel=-1
    ex_wifi_pos.c/onPeerConnected: peer_handle 2
      IPv4 local_port: 0
      IPv4 remote_port: 8883
      IPv4 protocol: 6 (MQTT)
      IPv4 local_address: '0.0.0.0'
      IPv4 remote_address: '52.211.238.211'

    ==== MQTT connection successfull ====



    Main loop detected connection on channel 1: Performing WiFi Scan.

    Sending Wifi Scan
    ex_wifi_pos.c/onWifiScanHandler:  1062EBF242F4,3,dlink-C9D4-EXT,-54
    ex_wifi_pos.c/onWifiScanHandler:  908D7823C9D4,3,dlink-C9D4,-73
    ex_wifi_pos.c/onWifiScanHandler:  9EDAC43107EE,8,,-88
    ex_wifi_pos.c/onWifiScanHandler:  88D7F64684B8,9,Muppet labs,-83
    ex_wifi_pos.c/onWifiScanHandler:  021599A8C94A,9,DIRECT-gmCLP-360 Series,-84
    ex_wifi_pos.c/onWifiScanHandler:  B03956E9E0D0,11,Mehet-Weret_2GEXT,-41
    ex_wifi_pos.c/onWifiScanHandler:  D847325256D1,11,Mehet-Weret-2G,-68
    ex_wifi_pos.c/onWifiScanHandler:  EA47325256D1,11,,-71
    ex_wifi_pos.c/onWifiScanHandlerOk: Scan complete
    ex_wifi_pos.c/onData: Channel 1: Data event of 46 bytes: '{"longitude":12.983666,"latitude":55.6128367}'

*/

char scanresult[512];


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

  printf("ex_wifi_pos.c/onStartup: Startup event\n");

  return 0;
}


int onConnectIpV4(const ucxhEDM_connectionEventIPv4_t *event) 
{
  assert(event != NULL);


  printf("ex_wifi_pos.c/onConnectIpV4: Channel %d: Connection event to localport %u from %u.%u.%u.%u:%u, protocol %s, lastChannel=%d\n",
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

  lastChannel = event->hChannel;
  numberOfConnections ++;

  return 0;
}

int onData(int channel, const uint8_t *data, size_t length)
{
  assert(data != NULL);

  //... Handle the incoming data here...

  // In this example, we know the data is generated by the Thingstream WiFi positioning server, 
  // which returns a JSON result on the form
  // {"longitude":12.983666,"latitude":55.6128367}
  // so let's just print the result as-is.

  printf("ex_wifi_pos.c/onData: Channel %d: Data event of %d bytes: '", channel, length);
  ucxhPARSER_printBuffer(data, length);
  printf("'\n");

  return 0;
}


int onDisconnect(const ucxhEDM_disconnectEvent_t *pEvent)
{
  assert(pEvent != NULL);

  printf("ex_wifi_pos.c/onDisconnect: Channel %d: Disconnect event, lastChannel=%d, numberOfConnections=%d\n", pEvent->hChannel, lastChannel, numberOfConnections);

  lastChannel = -1;

if (numberOfConnections > 0) {
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

  printf("ex_wifi_pos.c/onNetworkStatus:\n");
  
  printNetworkStatusEvent(pEvent);

  return true;
}

int onNetworkStationUp(ucxhMODULE_urcNetworkStationStatusEvent_t *pEvent)
{
  assert(pEvent != NULL);
  assert(pEvent->interface_type == ucxhURC_INTERFACE_TYPE_WIFI_STATION);

  printf("ex_wifi_pos.c/onNetworkStationUp:\n");

  // Trigger the next step in the state-machine.

  exampleState = EXAMPLE_STATE_STATION_UP;
  
  return true; 
}

int onNetworkStationDown(ucxhMODULE_urcNetworkStationStatusEvent_t *pEvent)
{
  assert(pEvent != NULL);
  assert(pEvent->interface_type == ucxhURC_INTERFACE_TYPE_WIFI_STATION);

  // This is invoked periodically as a result of AT+UWSCA=3 until
  // the WiFi Station is actually up.

  printf("ex_wifi_pos.c/onNetworkStationDown:\n");

  return true; 
}


int onPeerConnected(ucxhPARSER_urcPeerConnectedEvent_t *pEvent)
{
  assert(pEvent != NULL);

  // In case of AT mode, this event does not always arrive.
  
  printf("ex_wifi_pos.c/onPeerConnected: peer_handle %u\n", pEvent->peer_handle);
  switch (pEvent->type) {
    case ucxhURC_PEER_TYPE_IPV4:
      printf("  IPv4 local_port: %u\n",    pEvent->ipv4_peer.local_port);
      printf("  IPv4 remote_port: %u\n",   pEvent->ipv4_peer.remote_port);
      printf("  IPv4 protocol: %u (%s)\n", pEvent->ipv4_peer.protocol, 
        (pEvent->ipv4_peer.protocol == ucxhURC_IP_PROTOCOL_TCP ? "TCP" :
        (pEvent->ipv4_peer.protocol == ucxhURC_IP_PROTOCOL_UDP ? "UDP" :
        (pEvent->ipv4_peer.protocol == ucxhURC_IP_PROTOCOL_MQTT ? "MQTT" :
        ("UNKNOWN"))))
      );
      printf("  IPv4 local_address: '%s'\n", pEvent->ipv4_peer.local_address);
      printf("  IPv4 remote_address: '%s'\n", pEvent->ipv4_peer.remote_address);
      
      if(pEvent->ipv4_peer.protocol == ucxhURC_IP_PROTOCOL_MQTT) {
          exampleState = EXAMPLE_STATE_CONNECTED;
          printf("\n==== MQTT connection successfull ====\n");
      }
      
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
      printf("ex_wifi_pos.c/onPeerConnected: Unimplemented peer type %u\n", pEvent->type);
      return true;
  }
  return true;
}


int onPeerDisconnected(ucxhPARSER_urcPeerDisconnectedEvent_t *pEvent)
{
  assert(pEvent != NULL);

  printf("ex_wifi_pos.c/onPeerDisconnected peer_handle %u\n", pEvent->peer_handle);

  return true;
}


int onNetworkDown(ucxhPARSER_urcNetworkDownEvent_t *pEvent)
{
  assert(pEvent != NULL);

  printf("ex_wifi_pos.c/onNetworkDown interface_id %u\n", pEvent->interface_id);

  return true;
}


int onNetworkUp(ucxhPARSER_urcNetworkUpEvent_t *pEvent)
{
  assert(pEvent != NULL);

  printf("ex_wifi_pos.c/onNetworkUp interface_id %u\n", pEvent->interface_id);

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

  printf("ex_wifi_pos.c/onWifiLinkDisconnected: connection_id %u, reason %u (%s)\n", pEvent->connection_id, pEvent->reason, reasonStr);

  return true;
}


int onWifiLinkConnected(ucxhPARSER_urcWifiLinkConnectedEvent_t *pEvent)
{
  assert(pEvent != NULL);

  printf("ex_wifi_pos.c/onWifiLinkConnected:                        connection_id:        %d\n"
         "                                                          BSSID:                '%s'\n"
         "                                                          WiFi channel:         %d\n",
         pEvent->connection_id, pEvent->bssid, pEvent->channel);

  return true;
}


int onWifiStationStatus(ucxhPARSER_urcWifiStationStatusEvent_t *pEvent)
{
  assert(pEvent != NULL);

  printf("ex_wifi_pos.c/onWifiStationStatus: TODO: Print content\n");

  return true;
}

int onIdentificationInformation(ucxhPARSER_urcIdentificationInformationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    printf("ex_wifi_pos.c/onIdentificationInformation:                Application Version:  '%s'\n"
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

    printf("ex_wifi_pos.c/onLocalAddress:                             Interface_id:         %d (%s)\n"
           "                                                          MAC Address           '%s'\n",
           pEvent->interface_id, hardware_interface_name, pEvent->mac_address);

    return true;
}

int onManufacturerIdentification(ucxhPARSER_urcManufacturerIdentificationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    printf("ex_wifi_pos.c/onManufacturerIdentification:               Manufacturer:         '%s'\n", pEvent->manufacturer);

    return true;
}

int onModelIdentification(ucxhPARSER_urcModelIdentificationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    printf("ex_wifi_pos.c/onModelIdentification:                      Model:                '%s'\n", pEvent->model);
    return true;
}

int onSoftwareVersionIdentification(ucxhPARSER_urcSoftwareVersionIdentificationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    printf("ex_wifi_pos.c/onSoftwareVersionIdentification:            Software Version:     '%s'\n", pEvent->software_version);
    return true;
}


//
// The interesting code starts here...
//



int nbrResults = 0;

int onWifiScanHandler(ucxhPARSER_urcWifiScanEvent_t *pEvent)
{
    assert(pEvent != NULL);

    // Re-format the scan result into a JSON string which Thingstream
    // Typical format is
    // [
    //   {
    //     "macAddress": "66:32:B1:CE:9E:32",
    //     "signalStrength": -84,
    //     "signalToNoiseRatio": 0,
    //     "channel": 7,
    //     "age": 0
    //   },
    //     ...
    //   {
    //     "macAddress": "60:F4:45:E7:A7:64",
    //     "signalStrength": -55,
    //     "signalToNoiseRatio": 0,
    //     "channel": 11,
    //     "age": 0
    //   }
    // ]

    if(nbrResults <= MAX_WIFI_SCAN_RESSULTS) {

      if(strlen(scanresult) == 0) {
        strcat(scanresult, "[");
        nbrResults = 0;
      }
      else {
        strcat(scanresult, ",");
      }
 
      nbrResults++;

      strcat(scanresult, "{\"macAddress\":\"");

      char bssid[20];

      sprintf(bssid,"%s",pEvent->bssid);

      char formatted_bssid[18];
      sprintf(formatted_bssid, "%c%c:%c%c:%c%c:%c%c:%c%c:%c%c", bssid[0], bssid[1], bssid[2], bssid[3],bssid[4], bssid[5],bssid[6], bssid[7], bssid[8], bssid[9], bssid[10], bssid[11]);

      strcat(scanresult, formatted_bssid);

      strcat(scanresult, "\",\"signalStrength\":");

      char rssi[4];
      sprintf(rssi,"%d",pEvent->rssi);

      strcat(scanresult, rssi);

      strcat(scanresult, ",\"signalToNoiseRatio\":0");

      strcat(scanresult, ",\"channel\":");

      char channel[3];
      sprintf(channel,"%d",pEvent->channel);
      strcat(scanresult, channel);


      strcat(scanresult, ",\"age\":0}");

      printf("ex_wifi_pos.c/onWifiScanHandler:  %s,%d,%s,%d %s\n", pEvent->bssid, pEvent->channel, pEvent->ssid, pEvent->rssi, nbrResults > MAX_WIFI_SCAN_RESSULTS ? "(Enough APs found, ignoring this one)" : "");
    }

  return true;
}



int onWifiScanHandlerOk(const char *lastcommand)
{
  assert(lastcommand != NULL);

  if (strncmp(lastcommand, "+UWSCAN", 7) == 0) {
    printf("ex_wifi_pos.c/onWifiScanHandlerOk: Scan complete\n");
    strcat(scanresult, "]"); // Finalize the format for the Thingstream WiFi Positioning server

    // Just for testing the json string 
    //char test[] = "[{\"macAddress\":\"60:32:B1:CE:9E:32\",\"signalStrength\":-83,\"signalToNoiseRatio\":0,\"channel\":7,\"age\":0},{\"macAddress\":\"6A:32:B1:CE:9E:32\",\"signalStrength\":-84,\"signalToNoiseRatio\":0,\"channel\":7,\"age\":0},{\"macAddress\":\"66:32:B1:CE:9E:32\",\"signalStrength\":-84,\"signalToNoiseRatio\":0,\"channel\":7,\"age\":0},{\"macAddress\":\"60:F4:45:E7:A7:64\",\"signalStrength\":-55,\"signalToNoiseRatio\":0,\"channel\":11,\"age\":0}]";
    //printf("Sending EDM data: channel: %d:%s\n", lastChannel, test);
    //ucxhEDM_sendData(lastChannel, test, strlen(test));

    //printf("Sending EDM data: channel: %d:%s\n", lastChannel, scanresult);
    // if UCXH_PARSER_TRACE_SEND_DATA is set to 3, the ucxhEDM_sendData will print the data
    ucxhEDM_sendData(lastChannel, scanresult, strlen(scanresult));
  }
}

int onErrorReceived(const char *lastcommand)
{
  assert(lastcommand != NULL);

  printf("ex_wifi_pos.c/onErrorReceived: last command '%s'\n", lastcommand);
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

    printf("ex_wifi_pos.c/onUnknownURC: '%s'\n", urc);
  }
}


int main()
{
  bool result = false;
  int numberOfConnectionAttempts = 0;
  
  // To make sure the Windows serial port is closed in case of premature exit (aka assert())
  atexit(onExit);

  printf("\nMake sure the credentials in ex_config.h match your configuration on\nthingstream.io prior to running this example.\n\n");
  
  // Register callbacks that are mandatory for this example
  ucxhPARSER_setOnDataReceived(onData);
  ucxhEDM_setOnConnectIpV4Received(onConnectIpV4);
  ucxhEDM_setOnDisconnectReceived(onDisconnect);
  ucxhMODULE_setOnNetworkStationUp(onNetworkStationUp);
  ucxhMODULE_setOnNetworkStationDown(onNetworkStationDown);

  ucxhPARSER_setOnWifiScan(onWifiScanHandler);
  // ucxhPARSER_setOnWifiScanOk(onWifiScanHandlerOk);  // TODO: ucxh_parser needs to autogenerate the ucxhPARSER_setOnCommandOk() and ucxhPARSER_setOnCommandError() functions.

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
  ucxhPARSER_setOnIdentificationInformation(onIdentificationInformation);
  ucxhPARSER_setOnLocalAddress(onLocalAddress);
  ucxhPARSER_setOnManufacturerIdentification(onManufacturerIdentification);
  ucxhPARSER_setOnModelIdentification(onModelIdentification);
  ucxhPARSER_setOnSoftwareVersionIdentification(onSoftwareVersionIdentification);


  // And some catch-all callbacks. They, too only print the event in this example.
  ucxhPARSER_setOnUnknownURCReceived(onUnknownURC);
  ucxhPARSER_setOnErrorReceived(onErrorReceived);
  ucxhPARSER_setOnOkReceived(onWifiScanHandlerOk); // Normally not of interest, but for simplicity, we use this catch-all callback. (And the fact that ucxhPARSER_setOnWifiScanOk() is yet to be autogenerated)
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

  // Lets start the main loop here...
  

  int32_t numSeconds = 60;  
  printf("\n\n\n\nPolling for up to %us. Ensure a TCP server at %s is listening on port %u...\n\n\n\n", numSeconds, MQTT_REMOTE_PEER_IP, MQTT_REMOTE_PEER_PORT);

  time_t until = time(NULL) + numSeconds;

  do {
      // In a real-world application, ucxhPARSER_poll() would be invoked in the superloop or a designated reader task
      ucxhPARSER_poll();

      // Let's have a simple statemachine driving the example.
      // `exampleState` and `lastChannel` is modified when a connection is detected, dropped
      // by  onConnectIpV4(), onDisconnect() 
      // The rest of the callbacks are only printing the events for informative purposes.
      switch (exampleState) {
        case EXAMPLE_STATE_WAITING:
          break;

        case EXAMPLE_STATE_STATION_UP:
          printf("\n\n\n==== Main loop detected WiFi Station interface configured and up. ====\n\n");
          printf("\nManually trying to connect to %s:%u\n", MQTT_REMOTE_PEER_IP, MQTT_REMOTE_PEER_PORT);
          ucxhPARSER_peerURL_t remote_peer = {
            .scheme = ucxhPARSER_PEER_URL_SCHEME_MQTT,
            .mqtt.domain = MQTT_REMOTE_PEER_IP,
            .mqtt.remote_port = MQTT_REMOTE_PEER_PORT,
            .mqtt.query.client = MQTT_CLIENT,
            .mqtt.query.user = MQTT_USER,
            .mqtt.query.password = MQTT_PASSWORD,
            .mqtt.query.publish_topic = MQTT_PUBLISH,
            .mqtt.query.subscribe_topic = MQTT_SUBSCRIBE,
            .mqtt.query.encr = 3, // TLS 1.2
          };

          result &= ucxhCOMMAND_connectPeer(&remote_peer);
          assert(result == true);

          exampleState = EXAMPLE_STATE_WAITING;

          break;

        case EXAMPLE_STATE_CONNECTED:
          printf("\n\n\nMain loop detected connection on channel %d: Performing WiFi Scan.\n\n", lastChannel);

          printf("Sending Wifi Scan\n");
          result &= ucxhCOMMAND_WifiScan();
          assert(result == true);

          exampleState = EXAMPLE_STATE_WAITING;
          break;

        case EXAMPLE_STATE_DISCONNECTED:
          if (numberOfConnectionAttempts <= 3) {
            printf("\n\nMain loop detected disconnection. Let's manually try to reconnect again...\n");
            exampleState = EXAMPLE_STATE_STATION_UP;
            numberOfConnectionAttempts ++;
          } else {
            printf("\n\nERROR: Unable to connect to Thingstream.\n       Make sure the AP is connected to internet and that the credentials in ex_config.h\n       match your configuration on thingstream.io.\n\n");
            exampleState = EXAMPLE_STATE_QUIT;
          }
          break;

        case EXAMPLE_STATE_QUIT:
          printf("\n\nLeaving example\n");
          break;
      }
  } while (time(NULL) < until && exampleState != EXAMPLE_STATE_QUIT);


  printf("Restarting module\n");
  ucxhMODULE_restart();

  printf("Example completed\n");
  ucxhSERIAL_close();

  return 0;
}
