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


#include <windows.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "ucxh_config.h"

#include "ucxh_serial.h"

static HANDLE hComm = INVALID_HANDLE_VALUE;

// #define COMPORT "\\\\.\\COM25"

// NINA-W15 AT+UMLA=1 = D4CA6EFD9676, AT+UMLA=2 = D4CA6EFD9674
#ifndef UCXH_PARSER_COMPORT
#error UCXH_PARSER_COMPORT must be defined in a format such as "\\\\.\\COM44"
#endif


static bool ucxhSERIAL_configured = false;
bool ucxhSERIAL_configure(void)
{
  if (ucxhSERIAL_configured) {
    return true;
  }
  
  BOOL result = FALSE;
  DCB dcbSerialParams = { 0 }; // Initializing DCB structure
  
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

  // After that retrieve the current settings of the serial port using the GetCommState() function.
  result = GetCommState(hComm, &dcbSerialParams);  
  assert(result == TRUE);

  // Set the values for Baud rate, Byte size, Number of start/Stop bits etc.
  // NINA and ODIN modules use 115200 8N1 CTS/RTS by default
  dcbSerialParams.BaudRate      = 115200;
  dcbSerialParams.ByteSize      = 8;
  dcbSerialParams.StopBits      = ONESTOPBIT;
  dcbSerialParams.Parity        = NOPARITY;
  dcbSerialParams.fOutxCtsFlow  = TRUE;
  dcbSerialParams.fRtsControl   = RTS_CONTROL_HANDSHAKE;
  
  // ASSERT DTR upon start; Toggle from DEASSERTED to ASSERTED to enter AT mode
  // or start from STOP mode to AT mode.  
  dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE; 
  
  result = SetCommState(hComm, &dcbSerialParams);
  assert(result == TRUE);
  
  // Configure timeouts
  COMMTIMEOUTS timeouts = { 0 };
  timeouts.ReadIntervalTimeout         = 1;         // in milliseconds, or MAXDWORD
  timeouts.ReadTotalTimeoutConstant    = 20;        // in milliseconds, or 0
  timeouts.ReadTotalTimeoutMultiplier  = 0;         // in milliseconds, or 0
  timeouts.WriteTotalTimeoutConstant   = 5;         // in milliseconds
  timeouts.WriteTotalTimeoutMultiplier = 0;         // in milliseconds
  
  result = SetCommTimeouts(hComm, &timeouts);
  assert(result == TRUE);

  ucxhSERIAL_configured = true;
  
  return true;
}

int ucxhSERIAL_write(const uint8_t *buffer, size_t length)
{
  if(buffer == NULL || length == 0) {
    return 0;
  }
  
  char *lpBuffer = (char *)buffer;
  DWORD dNoOFBytestoWrite = length;       // No of bytes to write into the port
  DWORD dNoOfBytesWritten = 0;            // No of bytes written to the port

  BOOL result = WriteFile(hComm,               // Handle to the Serial port
                     lpBuffer,            // Data to be written to the port
                     dNoOFBytestoWrite,   //No of bytes to write
                     &dNoOfBytesWritten,  //Bytes written
                     NULL);  

  if (result == FALSE) {
    DWORD error = GetLastError();
    if (error == ERROR_IO_PENDING) {
      printf("ucxhSERIAL_write: ERROR_IO_PENDING\n");  // TODO: How to handle?
    } else {
      printf("ucxhSERIAL_write: Error 0x%04X\n", error);
    }
    
    return -ENOTSUP;
  }
  
  return dNoOfBytesWritten;  
}

int ucxhSERIAL_read(uint8_t *outbuffer, size_t bufsize)
{
  DWORD dwEventMask = 0x0; 
  BOOL result = false;
  uint8_t *dest = outbuffer;  // Current position in output pointer
  size_t bytesLeft = bufsize; // Bytes left in the output buffer
  DWORD NoBytesRead = 0;      // Bytes read during this ReadFile() invocation
  int totalBytesRead = 0;     // Total number of bytes read into the outbuffer
  
  if (outbuffer == NULL || bufsize < 1) {
    return -ENOMEM;
  }
  
  do
  {
    NoBytesRead = 0;
    result = ReadFile(hComm,
                      dest,
                      bytesLeft,
                      &NoBytesRead,
                      NULL);
    if (result == TRUE) {
      totalBytesRead += NoBytesRead;
      dest = dest + NoBytesRead;
      bytesLeft -= NoBytesRead;
    } else {
      return -ENOTSUP;
    }
  } while (NoBytesRead > 0 && bytesLeft > 0);  
  
  return totalBytesRead;
}

bool ucxhSERIAL_open(void)
{
  if (hComm != INVALID_HANDLE_VALUE) {
    printf("Serial port already opened\n");
    return false;    
  }
  
  hComm = CreateFile(UCXH_PARSER_COMPORT,             //port name
                      GENERIC_READ | GENERIC_WRITE, //Read/Write
                      0,                            // No Sharing
                      NULL,                         // No Security
                      OPEN_EXISTING,// Open existing port only
                      0,            // Non Overlapped I/O
                      NULL);        // Null for Comm Devices

  if (hComm == INVALID_HANDLE_VALUE) {
    printf("Serial port already opened?!\n");
    return false;
  }
    
  return ucxhSERIAL_configure();
}

void ucxhSERIAL_close(void)
{
  CloseHandle(hComm);//Closing the Serial Port
  hComm = INVALID_HANDLE_VALUE;  
}


int ucxhSERIAL_print(const char *s)
{
  size_t strLength = strlen(s);
  int bytesWritten = ucxhSERIAL_write(s, strLength);
  assert(bytesWritten == strLength);

  return bytesWritten;
}
