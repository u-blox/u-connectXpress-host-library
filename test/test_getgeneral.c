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

#include "test_tools.h"

#include "ucxh_library.h"
#include "ucxh_serial.h"
#include "ucxh_serial_mock.h"

#include "ex_config.h"

/** 
 * \file test_getgeneral.c
 * \test test_getgeneral.c Unit test: AT-commands I, GMI, GMM, GMR, GSN .
 * \brief Tests from chapter 3: AT-commands I, GMI, GMM, GMR, GSN .
 *  
## Introduction
 
This mocked test tests parsing of URCs for

 + Ix, GMI, GMM, GMR, GSN

See test_getgeneral

Full source in `test/test_getgeneral.c`

## Typical response

### Description

*/



#define ATPLUS_GMI     "AT+GMI\r\n"
#define ATPLUS_GMM     "AT+GMM\r\n"
#define ATPLUS_GMR     "AT+GMR\r\n"
#define ATPLUS_GSN     "AT+GSN\r\n"
#define ATI0          "ATI0\r\n"
#define ATI9          "ATI9\r\n"
#define ATI10         "ATI10\r\n"


#define PLUS_GMI_RESPONSE    "u-blox"
#define PLUS_GMM_RESPONSE    "NINA-W132"
#define PLUS_GMR_RESPONSE    "2.1.0-017"
#define PLUS_GSN_RESPONSE    "0233965910595296"
#define I0_RESPONSE          "NINA-W152-00B-00"
#define I9_2_RESPONSE        "NINA-W13X-2.1.0-017-0-g253211b6"
#define I10_RESPONSE         "000000000000246F28119B14"

static const char *readResultsPlusGMI[] = {
  /* AT+GMI  */       ATPLUS_GMI STRING(PLUS_GMI_RESPONSE) EOL OK,
};

static const char *readResultsPlusGMM[] = {
  /* AT+GMM  */       ATPLUS_GMM STRING(PLUS_GMM_RESPONSE) EOL OK,
};

static const char *readResultsPlusGMR[] = {
  /* AT+GMR  */       ATPLUS_GMR STRING(PLUS_GMR_RESPONSE) EOL OK,
};

static const char *readResultsPlusGSN[] = {
  /* AT+GSN  */       ATPLUS_GSN STRING(PLUS_GSN_RESPONSE) EOL OK,
};

static const char *readResultsI0[] = {
  /* ATI0  */         ATI0 STRING(I0_RESPONSE) EOL OK,
};

static const char *readResultsI9[] = {
  /* ATI9  */         ATI9 STRING(PLUS_GMR_RESPONSE) "," STRING(I9_2_RESPONSE) EOL OK,
};

static const char *readResultsI10[] = {
  /* ATI10  */        ATI10 STRING(I10_RESPONSE) EOL OK,
};




int gotPlusGMI = 0;
int onPlusGMI(ucxhPARSER_urcManufacturerIdentificationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    assert(gotPlusGMI == 0);
    printf("test_getgeneral.c/onPlusGMI: Manufacturer: '%s'\n", pEvent->manufacturer);
    assert(strcmp(pEvent->manufacturer, PLUS_GMI_RESPONSE) == 0);

    gotPlusGMI ++;
    return true;
}

int gotPlusGMM = 0;
int onPlusGMM(ucxhPARSER_urcModelIdentificationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    assert(gotPlusGMM == 0);
    printf("test_getgeneral.c/onPlusGMM: Model: '%s'\n", pEvent->model);
    assert(strcmp(pEvent->model, PLUS_GMM_RESPONSE) == 0);

    gotPlusGMM ++;
    return true;
}

int gotPlusGMR = 0;
int onPlusGMR(ucxhPARSER_urcSoftwareVersionIdentificationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    assert(gotPlusGMR == 0);
    printf("test_getgeneral.c/onPlusGMR Software Versions: '%s'\n", pEvent->software_version);
    assert(strcmp(pEvent->software_version, PLUS_GMR_RESPONSE) == 0);

    gotPlusGMR ++;
    return true;
}

int gotPlusGSN = 0;
int onPlusGSN(ucxhPARSER_urcSerialNumberEvent_t *pEvent)
{
    assert(pEvent != NULL);

    assert(gotPlusGSN == 0);
    printf("test_getgeneral.c/onPlusGSN Serial Number: '%s'\n", pEvent->serial_number);
    assert(strcmp(pEvent->serial_number, PLUS_GSN_RESPONSE) == 0);

    gotPlusGSN ++;
    return true;
}


int gotI0 = 0;
int onI0(ucxhPARSER_urcTypeCodeIdentificationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    assert(gotI0 == 0);
    printf("test_getgeneral.c/onI0: Type Code: '%s'\n", pEvent->type_code);
    assert(strcmp(pEvent->type_code, I0_RESPONSE) == 0);

    gotI0 ++;
    return true;
}

int gotI9 = 0;
int onI9(ucxhPARSER_urcIdentificationInformationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    assert(gotI9 == 0);
    printf("test_getgeneral.c/onI9: Application Version: '%s' Unique Identifier: '%s'\n", pEvent->application_version, pEvent->unique_identifier);
    assert(strcmp(pEvent->application_version, PLUS_GMR_RESPONSE) == 0);
    assert(strcmp(pEvent->unique_identifier, I9_2_RESPONSE) == 0);

    gotI9 ++;
    return true;
}

int gotI10 = 0;
int onI10(ucxhPARSER_urcMcuIdentificationEvent_t *pEvent)
{
    assert(pEvent != NULL);

    assert(gotI10 == 0);
    printf("test_getgeneral.c/onI0: MCU Identification: '%s'\n", pEvent->mcu_identification);
    assert(strcmp(pEvent->mcu_identification, I10_RESPONSE) == 0);

    gotI10 ++;
    return true;
}



int main()
{
  bool result = false;
  ucxhAT_response_t atResponse = ucxhAT_RESPONSE_OK;
  
  // To make sure the Windows serial port is closed in case of premature exit (aka assert())
  atexit(onExit);
  
  // Register Callbacks
  ucxhPARSER_setOnUnknownURCReceived(onUnknownURC);
  ucxhPARSER_setOnManufacturerIdentification(onPlusGMI);
  ucxhPARSER_setOnModelIdentification(onPlusGMM);
  ucxhPARSER_setOnSoftwareVersionIdentification(onPlusGMR);
  ucxhPARSER_setOnSerialNumber(onPlusGSN);
  ucxhPARSER_setOnTypeCodeIdentification(onI0);
  ucxhPARSER_setOnIdentificationInformation(onI9);
  ucxhPARSER_setOnMcuIdentification(onI10);
  
  result = ucxhSERIAL_open();
  assert(result == true);

  //
  //
  //


  // +GMI

  printf("Trying AT+GMI...\n");
  assert(gotPlusGMI == 0);
  ucxhPARSER_sendCommand(ATPLUS_GMI);
  THEN_READ(readResultsPlusGMI);
   assert(ucxhSERIAL_wasStringWritten(ATPLUS_GMI));

  printf("Trying Waiting for OK\n");
  atResponse = ucxhPARSER_waitForOk(-1); // -1 implies the default AT timeout. Otherwize, specify the number of seconds to wait.
  assert(atResponse == ucxhAT_RESPONSE_OK);
  assert(gotPlusGMI == 1);
  printf("AT+GMI OK!\n");

  // +GMM

  printf("Trying AT+GMM...\n");
  assert(gotPlusGMM == 0);
  ucxhPARSER_sendCommand(ATPLUS_GMM);
  THEN_READ(readResultsPlusGMM);
  assert(ucxhSERIAL_wasStringWritten(ATPLUS_GMM));

  printf("Trying Waiting for OK\n");
  atResponse = ucxhPARSER_waitForOk(-1); // -1 implies the default AT timeout. Otherwize, specify the number of seconds to wait.
  assert(atResponse == ucxhAT_RESPONSE_OK);
  assert(gotPlusGMM == 1);
  printf("AT+GMM OK!\n");


  // +GMR

  printf("Trying AT+GMR...\n");
  assert(gotPlusGMR == 0);
  ucxhPARSER_sendCommand(ATPLUS_GMR);
  THEN_READ(readResultsPlusGMR);
  assert(ucxhSERIAL_wasStringWritten(ATPLUS_GMR));

  printf("Trying Waiting for OK\n");
  atResponse = ucxhPARSER_waitForOk(-1); // -1 implies the default AT timeout. Otherwize, specify the number of seconds to wait.
  assert(atResponse == ucxhAT_RESPONSE_OK);
  assert(gotPlusGMR == 1);
  printf("AT+GMR OK!\n");

  // +GSN

  printf("Trying AT+GSN...\n");
  assert(gotPlusGSN == 0);
  ucxhPARSER_sendCommand(ATPLUS_GSN);
  THEN_READ(readResultsPlusGSN);
  assert(ucxhSERIAL_wasStringWritten(ATPLUS_GSN));

  printf("Trying Waiting for OK\n");
  atResponse = ucxhPARSER_waitForOk(-1); // -1 implies the default AT timeout. Otherwize, specify the number of seconds to wait.
  assert(atResponse == ucxhAT_RESPONSE_OK);
  assert(gotPlusGSN == 1);
  printf("AT+GSN OK!\n");



  // I0

  printf("Trying ATI0...\n");
  assert(gotI0 == 0);
  ucxhPARSER_sendCommand(ATI0);
  THEN_READ(readResultsI0);
  assert(ucxhSERIAL_wasStringWritten(ATI0));

  printf("Trying Waiting for OK\n");
  atResponse = ucxhPARSER_waitForOk(-1); // -1 implies the default AT timeout. Otherwize, specify the number of seconds to wait.
  assert(atResponse == ucxhAT_RESPONSE_OK);
  assert(gotI0 == 1);
  printf("ATI0 OK!\n");


  // I9

  printf("Trying ATI9...\n");
  assert(gotI9 == 0);
  ucxhPARSER_sendCommand(ATI9);
  THEN_READ(readResultsI9);
  assert(ucxhSERIAL_wasStringWritten(ATI9));

  printf("Trying Waiting for OK\n");
  atResponse = ucxhPARSER_waitForOk(-1); // -1 implies the default AT timeout. Otherwize, specify the number of seconds to wait.
  assert(atResponse == ucxhAT_RESPONSE_OK);
  assert(gotI9 == 1);
  printf("ATI9 OK!\n");

  // I10

  printf("Trying ATI10...\n");
  assert(gotI10 == 0);
  ucxhPARSER_sendCommand(ATI10);
  THEN_READ(readResultsI10);
  assert(ucxhSERIAL_wasStringWritten(ATI10));

  printf("Trying Waiting for OK\n");
  atResponse = ucxhPARSER_waitForOk(-1); // -1 implies the default AT timeout. Otherwize, specify the number of seconds to wait.
  assert(atResponse == ucxhAT_RESPONSE_OK);
  assert(gotI10 == 1);
  printf("ATI10 OK!\n");



  gotI0 = 0;
  gotI9 = 0;
  gotI10 = 0;

  // I0

  printf("Trying ucxhCOMMAND_readTypeCodeIdentification()...\n");
  assert(gotI0 == 0);
  assert(ucxhCOMMAND_readTypeCodeIdentification());
  THEN_READ(readResultsI0);
  assert(ucxhSERIAL_wasStringWritten(ATI0));

  printf("Trying Waiting for OK\n");
  atResponse = ucxhPARSER_waitForOk(-1); // -1 implies the default AT timeout. Otherwize, specify the number of seconds to wait.
  assert(atResponse == ucxhAT_RESPONSE_OK);
  assert(gotI0 == 1);
  printf("ucxhCOMMAND_readTypeCodeIdentification() OK!\n");

  // I9

  printf("Trying ucxhCOMMAND_readIdentificationInformation()...\n");
  assert(gotI9 == 0);
  assert(ucxhCOMMAND_readIdentificationInformation());
  THEN_READ(readResultsI9);
  assert(ucxhSERIAL_wasStringWritten(ATI9));

  printf("Trying Waiting for OK\n");
  atResponse = ucxhPARSER_waitForOk(-1); // -1 implies the default AT timeout. Otherwize, specify the number of seconds to wait.
  assert(atResponse == ucxhAT_RESPONSE_OK);
  assert(gotI9 == 1);
  printf("ucxhCOMMAND_readIdentificationInformation() OK!\n");

  // I10

  printf("Trying ucxhCOMMAND_readMcuIdentification()...\n");
  assert(gotI10 == 0);
  assert(ucxhCOMMAND_readMcuIdentification());
  THEN_READ(readResultsI10);
  assert(ucxhSERIAL_wasStringWritten(ATI10));

  printf("Trying Waiting for OK\n");
  atResponse = ucxhPARSER_waitForOk(-1); // -1 implies the default AT timeout. Otherwize, specify the number of seconds to wait.
  assert(atResponse == ucxhAT_RESPONSE_OK);
  assert(gotI10 == 1);
  printf("ucxhCOMMAND_readMcuIdentification() OK!\n");


  printf("test_getgeneral completed OK!\n");

  ucxhSERIAL_close();
  
  return 0;
}
