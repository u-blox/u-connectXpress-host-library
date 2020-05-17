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

#ifndef _TEST_TOOLS_H
#define _TEST_TOOLS_H

#define EOL "\r\n"

#define OK        "OK" EOL

#define STRING(s)     "\"" s "\""

#define THEN_READ(v) ucxhSERIAL_setReadResult(ELEMENTS_OF(v), v, 0)

/** \brief Invoke function with a structure containing one defined field, assert that the corresponding single AT-command has been written.
 * \param str Expected output for the specific param_tag, excluding the specific command.
 * \param pt Fieldname containing the param_type enum.
 * \param pv Value written to the param_type field.
 * \param arg Complete .-statement for the actual field.
 * \param fun Function to invoke with the struct as parameter
 * \param type Name of the type of the struct
 * \param expected String of the expected command without the actual value, e.g. "USCWC=0,"
 */
#define ASSERTSTRUCTWRITTEN(str, pv, arg, fun, type, pt, expected) \
do { \
    assert(fun(&((struct type) { \
        .pt = pv, \
        arg \
    }))); \
    returnOk(); \
    assert(ucxhSERIAL_wasStringWritten(expected str "\r\n")); \
} while (0)

/** \brief Next read will return ERROR, then nothing
 */
void returnError(void);

/** \brief Next read will return OK, then nothing
 */
void returnOk(void);

/** \brief Next read will return OK twice, then nothing
 */
void return2Ok(void);

/** \brief Next read will return OK thrice, then nothing
 */
void return3Ok(void);

/** \brief Invokes SERIAL_close()
 */
void onExit(void);

/** \brief Prints the content of the URC
 */
int onUnknownURC(const char *urc);

/** \brief Prints the content of the data, asserting channel is -1.
 */
int onData(int channel, const uint8_t *data, size_t length);

/** \brief Prints the content of the URC
 */
int onURC(uint8_t numParams, const char *ppParams[]);

#endif