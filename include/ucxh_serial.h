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


#ifndef _UCXH_SERIAL_H
#define _UCXH_SERIAL_H

#include <stdbool.h>
#include <stdio.h>

#ifndef CR
#define CR	'\r'
#endif

#ifndef LF
#define LF	'\n'
#endif


/*! 
* \ingroup serial
*  @{
*/

int ucxhSERIAL_write(const uint8_t *buffer, size_t length);
int ucxhSERIAL_print(const char *s);
int ucxhSERIAL_read(uint8_t *outbuffer, size_t bufsize);
bool ucxhSERIAL_open(void);
void ucxhSERIAL_close(void);

/*! @} */



#endif
