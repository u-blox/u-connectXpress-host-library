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


#ifndef _UCXH_AT_WIFI_H
#define _UCXH_AT_WIFI_H

#include <stdbool.h>

#include "ucxh_urc.h"

/*! 
*  @{
* \protected
*/

/** Maximum number of interfaces
 * TBD
 */
#define ucxhAT_MAX_NETWORK_INTERFACES 32

extern bool wifiLinkConnected;
extern bool wifiChanged;

extern bool networkUp[ucxhAT_MAX_NETWORK_INTERFACES+1];
extern ucxhPARSER_urcNetworkStatusEvent_t networkStatus[ucxhAT_MAX_NETWORK_INTERFACES];
extern bool networkChanged;

/*! @} */

/*! 
* \ingroup modulecallbacks
*  @{
*/

typedef struct ucxhPARSER_urcNetworkStatusEvent_t ucxhMODULE_urcNetworkStationStatusEvent_t;

/*! @} */


/*! 
* \ingroup modulecontrol
*  @{
*/


/**
 * \brief Indicates when WiFi Station has gotten an IP address, or dropped connection to an AP
 * \sa ucxhMODULE_setOnNetworkStationUp()
 * \sa ucxhMODULE_setOnNetworkStationDown()
 * \sa ucxhMODULE_activateWifiStation()
 */
extern bool ucxhMODULE_setOnNetworkStationUp(int (*callback)(ucxhMODULE_urcNetworkStationStatusEvent_t *pEvent));
extern bool ucxhMODULE_setOnNetworkStationDown(int (*callback)(ucxhMODULE_urcNetworkStationStatusEvent_t *pEvent));
extern bool ucxhMODULE_readWifiStationStatus(void);

/** \brief Activate the WiFi and WiFi Station with the last written configuration.
 * When Wifi Station has been activated and received an IPv4 address,
 * the callback registered with ucxhMODULE_setOnNetworkStationUp() will be
 * invoked.
 * If the Wifi Station gets deactivated, 
 * the callback registered with ucxhMODULE_setOnNetworkStationDown() will be
 * invoked.
 * Configure the WiFi Station using ucxhMODULE_writeWifiStationConfiguration().
 * prior to invoking this command. 
 * \warning The Wifi Station must not be activated when executing this command.
 * \cmd AT+UWSCA
 * \event ucxhMODULE_urcNetworkStationStatusEvent_t, ucxhPARSER_urcNetworkStatusEvent_t,
 * ucxhPARSER_urcWifiLinkConnectedEvent_t, ucxhPARSER_urcWifiLinkDisconnectedEvent_t
 * ucxhPARSER_urcNetworkUpEvent_t, ucxhPARSER_urcNetworkDownEvent_t
 * \sa ucxhMODULE_deactivateWifiStation()
 * \sa ucxhMODULE_resetWifiStationConfiguration()
 * \sa ucxhMODULE_storeWifiStationConfiguration()
 * \sa ucxhMODULE_writeWifiStationConfiguration()
 */
extern bool ucxhMODULE_activateWifiStation(void);

/** \brief Deactivate the WiFi and WiFi Station client.
 * When the Wifi Station has been deactivated, 
 *  the callback registered with ucxhMODULE_setOnNetworkStationDown() will be
 * invoked.
 * \cmd AT+UWSCA
 * \event ucxhMODULE_urcNetworkStationStatusEvent_t, ucxhPARSER_urcNetworkStatusEvent_t,
 * ucxhPARSER_urcWifiLinkDisconnectedEvent_t
 * ucxhPARSER_urcNetworkDownEvent_t
 * \sa ucxhMODULE_activateWifiStation()
 * \sa ucxhMODULE_resetWifiStationConfiguration()
 * \sa ucxhMODULE_storeWifiStationConfiguration()
 */
extern bool ucxhMODULE_deactivateWifiStation(void);

/** \brief Store the currently written configuration for the WiFI station.
 * Configure the WiFi Station and write the configuration using ucxhMODULE_writeWifiStationConfiguration()
 * prior to invoking this command.
 * \warning The Wifi Station must not be activated when executing this command.
 * \cmd AT+UWSCA
 * \sa ucxhMODULE_activateWifiStation()
 * \sa ucxhMODULE_deactivateWifiStation()
 * \sa ucxhMODULE_resetWifiStationConfiguration()
 * \sa ucxhMODULE_writeWifiStationConfiguration()
 */
extern bool ucxhMODULE_storeWifiStationConfiguration(void);

/** \brief Reset the WiFi station's configuration to last stored in NVRAM.
 * \warning The Wifi Station must not be activated when executing this command.
 * \cmd AT+UWSCA
 * \sa ucxhMODULE_activateWifiStation()
 * \sa ucxhMODULE_deactivateWifiStation()
 * \sa ucxhMODULE_storeWifiStationConfiguration()
 */
extern bool ucxhMODULE_resetWifiStationConfiguration(void);

/*! @} */

/*!
 * \ingroup internal
 *  @{
 */
//void ucxhMODULE_waitForNetworkStation(void);
//void ucxhMODULE_waitForWifiLink(void);
bool ucxhMODULE_prePollWifi(void);
bool ucxhMODULE_postPollWifi(void);

/*! @} */


/** \brief WiFi configuration Id
 */
typedef uint8_t ucxhPARSER_wifiStationConfigurationId_t;

typedef enum ucxhPARSER_wifiStationConfigurationParamTag_t {
    ucxhPARSER_WIFI_STATION_CONFIGURATION_ACTIVE_ON_STARTUP = 0,
    ucxhPARSER_WIFI_STATION_CONFIGURATION_SSID = 2,
    // 4 reserved
    ucxhPARSER_WIFI_STATION_CONFIGURATION_AUTHENTICATION = 5,
    // 6, 7 only for ODIN-W2
    ucxhPARSER_WIFI_STATION_CONFIGURATION_PSK_PASSPHRASE = 8,
    ucxhPARSER_WIFI_STATION_CONFIGURATION_PASSWORD = 9,
    // ucxhPARSER_WIFI_STATION_CONFIGURATION_USER_NAME = 10,
    // ucxhPARSER_WIFI_STATION_CONFIGURATION_DOMAIN_NAME = 11,
    // ucxhPARSER_WIFI_STATION_CONFIGURATION_CLIENT_CERTIFICATE_NAME = 12,
    // ucxhPARSER_WIFI_STATION_CONFIGURATION_CLIENT_PRIVATE_KEY = 13,
    // ucxhPARSER_WIFI_STATION_CONFIGURATION_CA_CERTIFICATE_NAME = 14,
    // ucxhPARSER_WIFI_STATION_CONFIGURATION_VALIDATE_CA_CERTIFICATE = 15,
    ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_MODE = 100,
    ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_ADDRESS = 101,
    ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_SUBNET_MASK = 102,
    ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_DEFAULT_GATEWAY = 103,
    ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_PRIMARY_DNS_SERVER = 104,
    ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV4_SECONDARY_DNS_SERVER = 105,
    // ucxhPARSER_WIFI_STATION_CONFIGURATION_ADDRESS_CONFLICT_DETECTION = 107,
    // ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV6_MODE = 200,
    // ucxhPARSER_WIFI_STATION_CONFIGURATION_IPV6_LINK_LOCAL_ADDRESS = 201,
    ucxhPARSER_WIFI_STATION_CONFIGURATION_LISTEN_INTERVAL = 300,
    ucxhPARSER_WIFI_STATION_CONFIGURATION_DTIM_ENABLED = 301
} ucxhPARSER_wifiStationConfigurationParamTag_t;

typedef enum ucxhPARSER_wifiStationAuthentication_t {
    ucxhPARSER_WIFI_STATION_AUTHENTICATION_NO_CHANGE = 0,
    ucxhPARSER_WIFI_STATION_AUTHENTICATION_OPEN = 1,      //!< Default
    ucxhPARSER_WIFI_STATION_AUTHENTICATION_WPA_WPA2 = 2,  //!< Use `psk` or `passphrase`. \note `psk` is only supported on ODIN-W2.
    ucxhPARSER_WIFI_STATION_AUTHENTICATION_LEAP = 3,      //!< Use `password`.
    ucxhPARSER_WIFI_STATION_AUTHENTICATION_PEAP = 4,      //!< Use `password`.
    ucxhPARSER_WIFI_STATION_AUTHENTICATION_EAP_TLS = 5,
} ucxhPARSER_wifiStationAuthentication_t;

typedef char ucxhPARSER_wifiAuthenticationPsk_t[32+1];
typedef char ucxhPARSER_wifiAuthenticationPassphrase_t[63+1];
typedef char ucxhPARSER_wifiAuthenticationPassword_t[31+1];

typedef enum ucxhPARSER_wifiIpV4Mode_t {
  ucxhPARSER_WIFI_IPV4_MODE_NO_CHANGE = 0,
  ucxhPARSER_WIFI_IPV4_MODE_STATIC = 1,
  ucxhPARSER_WIFI_IPV4_MODE_DHCP = 2 // Default
} ucxhPARSER_wifiIpV4Mode_t;

typedef enum ucxhPARSER_wifiIpV6Mode_t {
  ucxhPARSER_WIFI_IPV6_MODE_NO_CHANGE  = 0,
  ucxhPARSER_WIFI_IPV6_MODE_LINK_LOCAL = 1, // Default
} ucxhPARSER_wifiIpV6Mode_t;

typedef struct ucxhPARSER_wifiStationConfigurationParam_t {
    ucxhPARSER_wifiStationConfigurationParamTag_t param_tag;
    union {
        bool active_on_startup;
        ucxhAT_ssidStr_t ssid;
        ucxhPARSER_wifiStationAuthentication_t authentication;
        ucxhPARSER_wifiAuthenticationPsk_t psk;                 //!< Use `param_tag ucxhPARSER_WIFI_STATION_CONFIGURATION_PSK_PASSPHRASE` and `autentication ucxhPARSER_WIFI_STATION_AUTHENTICATION_WPA_WPA2`. \note Only supported on ODIN-W2.
        ucxhPARSER_wifiAuthenticationPassphrase_t passphrase;   //!< Use `param_tag ucxhPARSER_WIFI_STATION_CONFIGURATION_PSK_PASSPHRASE` and `autentication ucxhPARSER_WIFI_STATION_AUTHENTICATION_WPA_WPA2`.
        ucxhPARSER_wifiAuthenticationPassword_t password;       //!< Use `param_tag ucxhPARSER_WIFI_STATION_CONFIGURATION_PASSWORD` and `authentication ucxhPARSER_WIFI_STATION_AUTHENTICATION_LEAP` or ucxhPARSER_WIFI_STATION_AUTHENTICATION_PEAP`.
        ucxhPARSER_wifiIpV4Mode_t ipv4_mode;
        ucxhAT_ipV4AddrStr_t ipv4_address;
        ucxhAT_ipV4AddrStr_t ipv4_subnet_mask;
        ucxhAT_ipV4AddrStr_t ipv4_gateway;  
        ucxhAT_ipV4AddrStr_t primary_dns_server;
        ucxhAT_ipV4AddrStr_t secondary_dns_server;
        ucxhPARSER_wifiIpV6Mode_t ipv6_mode;
        ucxhAT_ipV6AddrStr_t ipv6_link_local_address;
        uint8_t listen_interval;
        bool dtim_enabled;
    };
} ucxhPARSER_wifiStationConfigurationParam_t;

typedef enum ucxhPARSER_wifiStationConfigurationOnStartup_t {
  ucxhPARSER_WIFI_STATION_CONFIGURATION_ON_STARTUP_NO_CHANGE = 0,
  ucxhPARSER_WIFI_STATION_CONFIGURATION_ON_STARTUP_ACTIVE = 1,
  ucxhPARSER_WIFI_STATION_CONFIGURATION_ON_STARTUP_INACTIVE = 2, // Default
} ucxhPARSER_wifiStationConfigurationOnStartup_t;

typedef enum ucxhPARSER_wifiStationConfigurationDtim_t {
  ucxhPARSER_WIFI_STATION_CONFIGURATION_DTIM_NO_CHANGE = 0,
  ucxhPARSER_WIFI_STATION_CONFIGURATION_DTIM_ENABLE = 1, // Default
  ucxhPARSER_WIFI_STATION_CONFIGURATION_DTIM_DISABLE = 2,
} ucxhPARSER_wifiStationConfigurationDtim_t;

typedef struct ucxhPARSER_wifiStationConfiguration_t {
    ucxhPARSER_wifiStationConfigurationOnStartup_t on_startup;
    ucxhAT_ssidStr_t ssid;
    ucxhPARSER_wifiStationAuthentication_t authentication;
    ucxhPARSER_wifiAuthenticationPsk_t psk;                 //!< Use `autentication ucxhPARSER_WIFI_STATION_AUTHENTICATION_WPA_WPA2`. \note Only supported on ODIN-W2.
    ucxhPARSER_wifiAuthenticationPassphrase_t passphrase;   //!< Use `autentication ucxhPARSER_WIFI_STATION_AUTHENTICATION_WPA_WPA2`.
    ucxhPARSER_wifiAuthenticationPassword_t password;       //!< Use `authentication ucxhPARSER_WIFI_STATION_AUTHENTICATION_LEAP` or ucxhPARSER_WIFI_STATION_AUTHENTICATION_PEAP`.
    ucxhPARSER_wifiIpV4Mode_t ipv4_mode;
    ucxhAT_ipV4AddrStr_t ipv4_address;
    ucxhAT_ipV4AddrStr_t ipv4_subnet_mask;
    ucxhAT_ipV4AddrStr_t ipv4_gateway;  
    ucxhAT_ipV4AddrStr_t primary_dns_server;
    ucxhAT_ipV4AddrStr_t secondary_dns_server;
    ucxhPARSER_wifiIpV6Mode_t ipv6_mode;
    ucxhAT_ipV6AddrStr_t ipv6_link_local_address;
    uint8_t listen_interval;
    ucxhPARSER_wifiStationConfigurationDtim_t dtim;
} ucxhPARSER_wifiStationConfiguration_t; 

/*! 
* \ingroup directat
*  @{
*/


/** \brief Define and write a configuration parameter to RAM for the WiFi Station.
 * \note Does not store the configuration to NVRAM or activate the WiFi Station.
 * \note To write several parameters at once (recommended), use ucxhMODULE_writeWifiStationConfiguration() instad.
 * To optionally store the configuration to NVRAM, use ucxhMODULE_storeWifiStationConfiguration().
 * To activate the WiFi Station, use ucxhMODULE_activateWifiStation().
 * \cmd AT+UWSC
 * \sa ucxhMODULE_storeWifiStationConfiguration()
 * \sa ucxhMODULE_resetWifiStationConfiguration()
 * \sa ucxhMODULE_activateWifiStation()
 * \sa ucxhMODULE_deactivateWifiStation()
 * \warning The Wifi Station must not be activated when executing this command.
 */
bool ucxhCOMMAND_writeWifiStationConfiguration(const ucxhPARSER_wifiStationConfigurationParam_t *param);

/** \brief Read Network Status
 * \cmd AT+UNSTAT
 * \event ucxhPARSER_urcNetworkStatusEvent_t
 */
bool ucxhCOMMAND_readNetworkStatus(ucxhPARSER_networkInterfaceId_t interface_id);

/*! @} */

/*! 
* \ingroup modulecontrol
*  @{
*/

/** \brief Define and write a number of configuration parameter to RAM for the WiFi Station.
 * \note Does not store the configuration to NVRAM or activate the WiFi Station.
 * \warning Configuration parameter enumeration values set to 0 will 
 * not be updated, so make sure to use the enumeration values defined by the 
 * corresponding type, rather than the values documented in the u-connectXpress AT command manual.
 * \note Configuration parameters with an empty string will not be updated.
 * \note Configuration parameters with integer values 0 WILL be updated.
 * 
 * To optionally store the configuration to NVRAM, use ucxhMODULE_storeWifiStationConfiguration().
 * To activate the WiFi Station, use ucxhMODULE_activateWifiStation().
 * \cmd AT+UWSC
 * \sa ucxhMODULE_storeWifiStationConfiguration()
 * \sa ucxhMODULE_resetWifiStationConfiguration()
 * \sa ucxhMODULE_activateWifiStation()
 * \sa ucxhMODULE_deactivateWifiStation()
 * \warning The Wifi Station must not be activated when executing this command.
 */
bool ucxhMODULE_writeWifiStationConfiguration(const ucxhPARSER_wifiStationConfiguration_t *pConfiguration);
/*! @} */


/*! 
* \ingroup utility
*  @{
*/

bool ucxhPARSER_isIpV4Address(const char *s);
bool ucxhPARSER_isValidIpV4Address(const char *s);
bool ucxhPARSER_isIpV6Address(const char *s);
bool ucxhPARSER_isValidIpV6Address(const char *s);


/*! @} */

#endif
