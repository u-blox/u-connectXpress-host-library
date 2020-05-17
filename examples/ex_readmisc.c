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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ucxh_library.h"
#include "ucxh_serial.h"

#include "ex_config.h"

/** 
 * \file ex_readmisc.c
 * \example ex_readmisc Example on how to various parametersfrom the module
 * \brief Example on how to various parameters from the module
 *  
## Introduction
 
This example shows how to read various parameters from the module


Full source in `test/ex_readmisc.c`

## Typical response

    Opening serial port...
    Entering EDM...
    Handling stray responses
    Requesting misc parameters:
    ex_readmisc.c/onManufacturerIdentification:               Manufacturer:         'u-blox'
    ex_readmisc.c/onModelIdentification:                      Model:                'NINA-W132'
    ex_readmisc.c/onSoftwareVersionIdentification:            Software Version:     '2.1.0-017'
    ex_readmisc.c/onSerialNumber:                             Serial Number:        '0233965910595296'
    ex_readmisc.c/onTypeCodeIdentification:                   Type Code:            'NINA-W152-00B-00'
    ex_readmisc.c/onIdentificationInformation:                Application Version:  '2.1.0-017'
                                                              Unique Identifier:    'NINA-W13X-2.1.0-017-0-g253211b6'
    ex_readmisc.c/onMcuIdentification:                        MCU Identification:   '000000000000246F28119B14'
    ex_readmisc.c/onErrorReceived:                            Last command:         '+UMLA'
                                                              Last parameter:       '1'
    ex_readmisc.c/onLocalAddress:                             Interface_id:         2 (WiFi STA)
                                                              MAC Address           'D4CA6EFD96E0'
    ex_readmisc.c/onLocalAddress:                             Interface_id:         3 (Ethernet)
                                                              MAC Address           'D4CA6EFD96E1'
    ex_readmisc.c/onLocalAddress:                             Interface_id:         4 (WiFI AP)
                                                              MAC Address           'D4CA6EFD96E3'
    Restarting module
    ex_readmisc completed OK!



### Description

*/


int onTypeCodeIdentification(ucxhPARSER_urcTypeCodeIdentificationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    printf("ex_readmisc.c/onTypeCodeIdentification:                   Type Code:            '%s'\n", pEvent->type_code);
    return true;
}

int onIdentificationInformation(ucxhPARSER_urcIdentificationInformationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    printf("ex_readmisc.c/onIdentificationInformation:                Application Version:  '%s'\n"
           "                                                          Unique Identifier:    '%s'\n", pEvent->application_version, pEvent->unique_identifier);
    return true;
}

int onMcuIdentification(ucxhPARSER_urcMcuIdentificationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    printf("ex_readmisc.c/onMcuIdentification:                        MCU Identification:   '%s'\n", pEvent->mcu_identification);
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

    printf("ex_readmisc.c/onLocalAddress:                             Interface_id:         %d (%s)\n"
           "                                                          MAC Address           '%s'\n", pEvent->interface_id, hardware_interface_name, pEvent->mac_address);

    return true;
}

int onManufacturerIdentification(ucxhPARSER_urcManufacturerIdentificationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    printf("ex_readmisc.c/onManufacturerIdentification:               Manufacturer:         '%s'\n", pEvent->manufacturer);

    return true;
}

int onModelIdentification(ucxhPARSER_urcModelIdentificationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    printf("ex_readmisc.c/onModelIdentification:                      Model:                '%s'\n", pEvent->model);
    return true;
}

int onSoftwareVersionIdentification(ucxhPARSER_urcSoftwareVersionIdentificationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    printf("ex_readmisc.c/onSoftwareVersionIdentification:            Software Version:     '%s'\n", pEvent->software_version);
    return true;
}

int onSerialNumber(ucxhPARSER_urcSerialNumberEvent_t *pEvent)
{
    assert(pEvent != NULL);

    printf("ex_readmisc.c/onSerialNumber:                             Serial Number:        '%s'\n", pEvent->serial_number);
    return true;
}


void onExit(void) {
  ucxhSERIAL_close();
}

int onErrorReceived(const char *lastcommand)
{
    assert(lastcommand != NULL);

    printf("ex_readmisc.c/onErrorReceived:                            Last command:         '%s'\n"
           "                                                          Last parameter:       '%s'\n", lastcommand, ucxhPARSER_getLastCommandParam());
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

    printf("ex_readmisc.c/onUnknownURC: '%s'\n", urc);
  }
}

int main()
{
  bool result = false;
  
  // To make sure the Windows serial port is closed in case of premature exit (aka assert())
  atexit(onExit);
  
// Register callbacks that are mandatory for this example
  ucxhPARSER_setOnTypeCodeIdentification(onTypeCodeIdentification);
  ucxhPARSER_setOnIdentificationInformation(onIdentificationInformation);
  ucxhPARSER_setOnMcuIdentification(onMcuIdentification);
  ucxhPARSER_setOnLocalAddress(onLocalAddress);
  ucxhPARSER_setOnManufacturerIdentification(onManufacturerIdentification);
  ucxhPARSER_setOnModelIdentification(onModelIdentification);
  ucxhPARSER_setOnSoftwareVersionIdentification(onSoftwareVersionIdentification);
  ucxhPARSER_setOnSerialNumber(onSerialNumber);

  // Register other callbacks that can be of use. In this example, they'll only print the events.
  ucxhPARSER_setOnErrorReceived(onErrorReceived);


  printf("Opening serial port...\n");  
  result = ucxhSERIAL_open();
  assert(result == true);

  printf("Entering EDM...\n");
  //ucxhMODULE_enterExtendedDataMode();
  ucxhMODULE_enterAtMode();

  printf("Handling stray responses\n");
  ucxhPARSER_wait(1);
  
  printf("Requesting misc parameters:\n");

  // All commands are always queued and transmitted one at a time, each one
  // waiting in the mainloop until the OK or ERROR response from the
  // previous command has been received.
  
  ucxhCOMMAND_readManufacturerIdentification();
  ucxhCOMMAND_readModelIdentification();
  ucxhCOMMAND_readSoftwareVersionIdentification();
  ucxhCOMMAND_readSerialNumber();

  ucxhCOMMAND_readTypeCodeIdentification();
  ucxhCOMMAND_readIdentificationInformation();
  ucxhCOMMAND_readMcuIdentification();

  ucxhCOMMAND_readLocalAddress(ucxhPARSER_HARDWARE_INTERFACE_BLUETOOTH);  // Will return ERROR on NINA-W132
  ucxhCOMMAND_readLocalAddress(ucxhPARSER_HARDWARE_INTERFACE_WIFI_STATION);
  ucxhCOMMAND_readLocalAddress(ucxhPARSER_HARDWARE_INTERFACE_ETHERNET); 
  ucxhCOMMAND_readLocalAddress(ucxhPARSER_HARDWARE_INTERFACE_WIFI_AP);    // May return ERROR on EVK-W262U 
  
  // This represents the mainloop of the program:
  ucxhPARSER_wait(-1);  // -1 implies the default AT timeout. Otherwize, specify the number of seconds to wait.

  printf("Restarting module\n");
  ucxhMODULE_restart();
  
  printf("ex_readmisc completed OK!\n");
  ucxhSERIAL_close();
  
  return 0;
}
