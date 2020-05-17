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
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "test_tools.h"

#include "ucxh_library.h"
#include "ucxh_serial.h"
#include "ucxh_serial_mock.h"

#include "ex_config.h"

/** 
 * \file test_at_wifi.c
 * \test test_at_wifi.c Unit test: AT-commands UWSSTAT UNSTAT UWSCA UWSC and MODULE_writeWifiStationConfiguration() .
 * \brief Tests from chapter 17, 10: AT-commands UWSSTAT UNSTAT UWSCA UWSC and MODULE_writeWifiStationConfiguration() .
 *  
## Introduction
 
## Typical response

### Description


*/


void test_plusUWSSTAT(void)
{
  // AT+UWSSTAT

  assert(ucxhMODULE_readWifiStationStatus());
  returnOk();
  assert(ucxhSERIAL_wasStringWritten("AT+UWSSTAT\r\n"));

  printf("AT+UNSTAT: ucxhMODULE_readWifiStationStatus OK\n========================================\n");
}

void test_plusUNSTAT(void)
{
  // AT+UNSTAT

  assert(ucxhCOMMAND_readNetworkStatus(42));
  returnOk();
  assert(ucxhSERIAL_wasStringWritten("AT+UNSTAT=42\r\n"));

  assert(ucxhCOMMAND_readNetworkStatus(ucxhURC_INTERFACE_ID_ALL));
  returnOk();
  assert(ucxhSERIAL_wasStringWritten("AT+UNSTAT\r\n"));

  printf("AT+UNSTAT: ucxhCOMMAND_readNetworkStatus OK\n========================================\n");
}

void test_plusUWSCA(void)
{
  // AT+UWSCA
  assert(ucxhMODULE_activateWifiStation());
  returnOk();
  assert(ucxhSERIAL_wasStringWritten("AT+UWSCA=0,3\r\n"));

  assert(ucxhMODULE_deactivateWifiStation());
  returnOk();
  assert(ucxhSERIAL_wasStringWritten("AT+UWSCA=0,4\r\n"));

  assert(ucxhMODULE_storeWifiStationConfiguration());
  returnOk();
  assert(ucxhSERIAL_wasStringWritten("AT+UWSCA=0,1\r\n"));

  assert(ucxhMODULE_resetWifiStationConfiguration());
  returnOk();
  assert(ucxhSERIAL_wasStringWritten("AT+UWSCA=0,0\r\n"));

  printf("AT+UWSCA: ucxhMODULE_activateWifiStation /ucxhMODULE_deactivateWifiStation /  ucxhMODULE_storeWifiStationConfiguration / ucxhMODULE_resetWifiStationConfiguration OK\n========================================\n========================================\n========================================\n========================================\n");
}

void test_plusUWSC(void)
{
#define ASSERTUWSC(str,pv,arg) \
  ASSERTSTRUCTWRITTEN(str, pv, arg, \
    ucxhCOMMAND_writeWifiStationConfiguration, ucxhPARSER_wifiStationConfigurationParam_t, param_tag, "AT+UWSC=0,")


  ASSERTUWSC("0,1",                 ucxhPARSER_WIFI_STATION_CONFIGURATION_ACTIVE_ON_STARTUP,            .active_on_startup = true);
  ASSERTUWSC("2,\"myssid\"",        ucxhPARSER_WIFI_STATION_CONFIGURATION_SSID,                         .ssid = "myssid");
  ASSERTUWSC("5,1",                 ucxhPARSER_WIFI_STATION_CONFIGURATION_AUTHENTICATION,               .authentication = ucxhPARSER_WIFI_STATION_AUTHENTICATION_OPEN);
  ASSERTUWSC("5,2",                 ucxhPARSER_WIFI_STATION_CONFIGURATION_AUTHENTICATION,               .authentication = ucxhPARSER_WIFI_STATION_AUTHENTICATION_WPA_WPA2);
  ASSERTUWSC("5,3",                 ucxhPARSER_WIFI_STATION_CONFIGURATION_AUTHENTICATION,               .authentication = ucxhPARSER_WIFI_STATION_AUTHENTICATION_LEAP);
  ASSERTUWSC("5,4",                 ucxhPARSER_WIFI_STATION_CONFIGURATION_AUTHENTICATION,               .authentication = ucxhPARSER_WIFI_STATION_AUTHENTICATION_PEAP);
  ASSERTUWSC("5,5",                 ucxhPARSER_WIFI_STATION_CONFIGURATION_AUTHENTICATION,               .authentication = ucxhPARSER_WIFI_STATION_AUTHENTICATION_EAP_TLS);
  ASSERTUWSC("8,\"abcdef\"",        ucxhPARSER_WIFI_STATION_CONFIGURATION_PSK_PASSPHRASE,               .psk = "abcdef");
  ASSERTUWSC("8,\"mypassphrase\"",  ucxhPARSER_WIFI_STATION_CONFIGURATION_PSK_PASSPHRASE,               .passphrase = "mypassphrase");
  ASSERTUWSC("9,\"mypassword\"",    ucxhPARSER_WIFI_STATION_CONFIGURATION_PASSWORD,                     .password = "mypassword");
  ASSERTUWSC("100,1",               ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_MODE,                    .ipv4_mode = ucxhPARSER_WIFI_IPV4_MODE_STATIC);
  ASSERTUWSC("100,2",               ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_MODE,                    .ipv4_mode = ucxhPARSER_WIFI_IPV4_MODE_DHCP);
  ASSERTUWSC("101,192.168.2.100",   ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_ADDRESS,                 .ipv4_address = "192.168.2.100");
  ASSERTUWSC("102,255.255.255.0",   ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_SUBNET_MASK,             .ipv4_subnet_mask = "255.255.255.0");
  ASSERTUWSC("103,192.168.2.1",     ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_DEFAULT_GATEWAY,         .ipv4_gateway = "192.168.2.1");
  ASSERTUWSC("104,1.23.45.234",     ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_PRIMARY_DNS_SERVER,      .primary_dns_server = "1.23.45.234");
  ASSERTUWSC("105,2.23.45.234",     ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_SECONDARY_DNS_SERVER,    .secondary_dns_server = "2.23.45.234");
  ASSERTUWSC("300,3",               ucxhPARSER_WIFI_STATION_CONFIGURATION_LISTEN_INTERVAL,              .listen_interval = 3);
  ASSERTUWSC("301,1",               ucxhPARSER_WIFI_STATION_CONFIGURATION_DTIM_ENABLED,                 .dtim_enabled = true);

  printf("AT+UWSC: ucxhCOMMAND_writeWifiStationConfiguration OK\n========================================\n========================================\n========================================\n========================================\n");
}




void test_MODULE_writeWifiStationConfiguration1(void)
{
  // ucxhMODULE_writeWifiStationConfiguration (AT+UWSC)
  #define ASSERT_MwWSC(str) \
    returnOk(); \
    assert(ucxhSERIAL_wasStringWritten("AT+UWSC=0," str "\r\n"))

  ucxhPARSER_wifiStationConfiguration_t cfg = {
    .ssid                 = "myssid",
    .psk                  = "abcdef",
    .passphrase           = "mypassphrase",
    .password             = "mypassword",
    .ipv4_address         = "192.168.2.100",
    .ipv4_subnet_mask     = "255.255.255.0",
    .ipv4_gateway         = "192.168.2.1",
    .primary_dns_server   = "1.23.45.234",
    .secondary_dns_server = "2.23.45.234",
    .listen_interval      = 3,
  };
  assert(ucxhMODULE_writeWifiStationConfiguration(&cfg));

  ASSERT_MwWSC("2,\"myssid\"");       // ucxhPARSER_WIFI_STATION_CONFIGURATION_SSID,                         .ssid = "myssid");
  ASSERT_MwWSC("8,\"abcdef\"");       // ucxhPARSER_WIFI_STATION_CONFIGURATION_PSK_PASSPHRASE,               .psk = "abcdef");
  ASSERT_MwWSC("8,\"mypassphrase\""); // ucxhPARSER_WIFI_STATION_CONFIGURATION_PSK_PASSPHRASE,               .passphrase = "mypassphrase");
  ASSERT_MwWSC("9,\"mypassword\"");   // ucxhPARSER_WIFI_STATION_CONFIGURATION_PASSWORD,                     .password = "mypassword");
  ASSERT_MwWSC("101,192.168.2.100");  // ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_ADDRESS,                 .ipv4_address = "192.168.2.100");
  ASSERT_MwWSC("102,255.255.255.0");  // ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_SUBNET_MASK,             .ipv4_subnet_mask = "255.255.255.0");
  ASSERT_MwWSC("103,192.168.2.1");    // ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_DEFAULT_GATEWAY,         .ipv4_gateway = "192.168.2.1");
  ASSERT_MwWSC("104,1.23.45.234");    // ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_PRIMARY_DNS_SERVER,      .primary_dns_server = "1.23.45.234");
  ASSERT_MwWSC("105,2.23.45.234");    // ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_SECONDARY_DNS_SERVER,    .secondary_dns_server = "2.23.45.234");
  ASSERT_MwWSC("300,3");              // ucxhPARSER_WIFI_STATION_CONFIGURATION_LISTEN_INTERVAL,              .listen_interval = 3);

  printf("AT+UWSC: ucxhMODULE_writeWifiStationConfiguration OK\n========================================\n========================================\n========================================\n========================================\n");
}



void test_MODULE_writeWifiStationConfigurationSingleParam(void)
{
  #define ASSERT_MwWSC2(str, act) \
  do { \
    ucxhPARSER_wifiStationConfiguration_t cfg = { act }; \
    assert(ucxhMODULE_writeWifiStationConfiguration(&cfg)); \
    return2Ok(); /* Since only one param is set, only one OK is really to be returned, but listen_interval is an int, which is always cleared to 0 unless set, so its OK is always also returned */ \
    assert(ucxhSERIAL_wasStringWritten("AT+UWSC=0," str "\r\n")); \
    assert(ucxhSERIAL_wasStringWritten("AT+UWSC=0,300,0\r\n")); \
  } while (0)

  ASSERT_MwWSC2("0,0",                .on_startup           = ucxhPARSER_WIFI_STATION_CONFIGURATION_ON_STARTUP_INACTIVE);
  ASSERT_MwWSC2("0,1",                .on_startup           = ucxhPARSER_WIFI_STATION_CONFIGURATION_ON_STARTUP_ACTIVE);
  ASSERT_MwWSC2("2,\"myssid\"",       .ssid                 = "myssid");
  ASSERT_MwWSC2("5,1",                .authentication       = ucxhPARSER_WIFI_STATION_AUTHENTICATION_OPEN);
  ASSERT_MwWSC2("5,2",                .authentication       = ucxhPARSER_WIFI_STATION_AUTHENTICATION_WPA_WPA2);
  ASSERT_MwWSC2("5,3",                .authentication       = ucxhPARSER_WIFI_STATION_AUTHENTICATION_LEAP);
  ASSERT_MwWSC2("5,4",                .authentication       = ucxhPARSER_WIFI_STATION_AUTHENTICATION_PEAP);
  ASSERT_MwWSC2("5,5",                .authentication       = ucxhPARSER_WIFI_STATION_AUTHENTICATION_EAP_TLS);
  ASSERT_MwWSC2("8,\"abcdef\"",       .psk                  = "abcdef");
  ASSERT_MwWSC2("8,\"mypassphrase\"", .passphrase           = "mypassphrase");
  ASSERT_MwWSC2("9,\"mypassword\"",   .password             = "mypassword");
  ASSERT_MwWSC2("100,1",              .ipv4_mode            = ucxhPARSER_WIFI_IPV4_MODE_STATIC);
  ASSERT_MwWSC2("100,2",              .ipv4_mode            = ucxhPARSER_WIFI_IPV4_MODE_DHCP);
  ASSERT_MwWSC2("101,192.168.2.100",  .ipv4_address         = "192.168.2.100");
  ASSERT_MwWSC2("102,255.255.255.0",  .ipv4_subnet_mask     = "255.255.255.0");
  ASSERT_MwWSC2("103,192.168.2.1",    .ipv4_gateway         = "192.168.2.1");
  ASSERT_MwWSC2("104,1.23.45.234",    .primary_dns_server   = "1.23.45.234");
  ASSERT_MwWSC2("105,2.23.45.234",    .secondary_dns_server = "2.23.45.234");
  ASSERT_MwWSC2("300,3",              .listen_interval      = 3);
  ASSERT_MwWSC2("301,0",              .dtim                 = ucxhPARSER_WIFI_STATION_CONFIGURATION_DTIM_DISABLE);
  ASSERT_MwWSC2("301,1",              .dtim                 = ucxhPARSER_WIFI_STATION_CONFIGURATION_DTIM_ENABLE);
  
  printf("AT+UWSC: test_MODULE_writeWifiStationConfigurationSingleParam OK\n========================================\n========================================\n========================================\n========================================\n");
}

int main()
{
  bool result = true;
  const int  numSeconds = 2;
  
  result &= ucxhSERIAL_open();
  assert(result == true);

  test_plusUWSSTAT();                                     ucxhPARSER_wait(numSeconds);
  test_plusUNSTAT();                                      ucxhPARSER_wait(numSeconds);
  test_plusUWSCA();                                       ucxhPARSER_wait(numSeconds);
  test_plusUWSC();                                        ucxhPARSER_wait(numSeconds);
  test_MODULE_writeWifiStationConfiguration1();           ucxhPARSER_wait(numSeconds);
  test_MODULE_writeWifiStationConfigurationSingleParam(); ucxhPARSER_wait(numSeconds);

  ucxhSERIAL_close();
  return 0;
}
