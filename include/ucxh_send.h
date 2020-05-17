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


#ifndef _UCXH_AT_SEND_H
#define _UCXH_AT_SEND_H

#include <stdio.h>

/** Transmit a series of AT command in AT mode.
 * \attention This function requires the module to be in AT mode. 
 * If not, invoke ucxhMODULE_enterAtMode() first.
 * \note Avoid using this method directly. Use ucxhPARSER_sendAtCommands() instead, since the latter
 * will use AT mode in case EDM is not activated.
 * \protected
 * \ingroup atsend
 * @param [in] atList Array of Command to transmit, e.g. "ATI9\r"
 * @param [in] elements Number of commands in the atList to send
 * \sa ucxhAT_sendCommand()
 * \sa ucxhEDM_sendAtCommands()
 */
void ucxhAT_sendAtCommands(const char *atList[], size_t elements);

/** Transmit a series of AT command over the EDM AT command channel.
 * \attention This function requires the module to be in Extended Data Mode (EDM).
 * If not, invoke ucxhMODULE_enterExtendedDataMode() first.
 * \note Avoid using this method directly. Use ucxhPARSER_sendAtCommands() instead, since the latter
 * will use AT mode in case EDM is not activated.
 * \protected
 * \ingroup edmsend
 * @param [in] atList Array of Command to transmit, e.g. "ATI9\r"
 * @param [in] elements Number of commands in the atList to send
 * \sa ucxhEDM_sendCommand()
 * \sa ucxhAT_sendAtCommands()
 */
void ucxhEDM_sendAtCommands(const char *atList[], size_t elements);

/** Transmit a series of AT command to the module.
 * In order to send a list of AT commands, this is the preferred function to use.
 * To send s single AT command, use ucxhPARSER_sendCommand().
 * The module must be in EDM or AT mode.
 * If not, invoke ucxhMODULE_enterExtendedDataMode() or cxhMODULE_enterATMode() or first.
 * \ingroup send
 * @param [in] atList Array of Command to transmit, e.g. "ATI9\r"
 * @param [in] elements Number of commands in the atList to send
 * \sa ucxhAT_sendAtCommands()
 * \sa ucxhEDM_sendAtCommands()
 */
void ucxhPARSER_sendAtCommands(const char *atList[], size_t elements);


/** Get the last AT command sent.
 */
const char *ucxhPARSER_getLastCommand(void);

/** Get the first parameter of the last AT command sent, or "" if it wasn't any or wasn't a numeric parameter.
 */
const char *ucxhPARSER_getLastCommandParam(void);

/** Transmit an AT command to the module in AT mode.
 * \attention This function requires the module to be in AT mode. 
 * If not, invoke ucxhMODULE_enterAtMode() first.
 * \note Avoid using this method directly. Use ucxhPARSER_sendAtCommand() instead, since the latter
 * will use AT mode in case EDM is not activated.
 * \protected
 * \ingroup atsend
 * @param [in] cmd Command to transmit, e.g. "ATI9\r"
 * \sa ucxhAT_sendCommands()
 * \sa ucxhEDM_sendCommand()
 */
void ucxhAT_sendCommand(const char *cmd);

/** Transmit an AT command to the module.
 * In order to send a single, trivial AT command, this is the preferred function to use.
 * To send a sequence of trivial AT command, use ucxhPARSER_sendAtCommands().
 * \note To send an AT command that has parameters with varying values,
 * it is possible to use a sequence of builder functions to ensure correct formatting and
 * simplify error-checking in a more powerful way than using sprintf().
 *
 * The module must be in EDM or AT mode.
 * If not, invoke ucxhMODULE_enterExtendedDataMode() or ucxhMODULE_enterATMode() first.
 * \ingroup send
 * @param [in] cmd Command to transmit, e.g. "ATI9\r"
 * \sa ucxhAT_sendCommands()
 * \sa ucxhEDM_sendCommand()
 * \sa ucxhPARSER_buildSetCmd(),  ucxhPARSER_buildActionCmd(), ucxhPARSER_buildRequestCmd(), ucxhPARSER_sendBuiltCommand()
 */
#define ucxhPARSER_sendCommand(cmd)  _ucxhPARSER_sendCommand(cmd, __FILE__, __LINE__)
void _ucxhPARSER_sendCommand(const char *cmd, const char *fle, uint32_t line);
void ucxhPARSER_sendCommandWithCallback(const char *cmd, int (*callback)(bool success, void *userdata), void *userdata);

/** Transmit data to the module in DATA mode.
 * \attention This function requires the module to be in DATA mode. 
 * If not, invoke ucxhMODULE_enterDataMode() first.
 * \ingroup atsend
 * @param [in] data Buffer containing the binary data to transmit
 * @param [in] length Number of bytes in data.
 * \sa ucxhPARSER_sendCommand()
 * \sa ucxhEDM_sendData()
 */
void ucxhDATA_sendData(const uint8_t *data, size_t length);

/** 
 * \brief Start building an AT command on the form `AT+UUU=[\<params\>...]`
 * Add parameters with the functions ucxhPARSER_addXXX() to ensure correct
 * formatting.
 * Finalize and send the command using ucxhPARSER_sendBuiltCommand().
 * If a previously (or partly) build AT command hasn't been sucessfully sent with
 * ucxhPARSER_sendBuiltCommand(), that command is disregarded.
 * The module must be in EDM or AT mode.
 * If not, invoke ucxhMODULE_enterExtendedDataMode() or ucxhMODULE_enterATMode() first.
 * \param [in] cmd Command name excluding `"AT"` and `'='`
 * \returns false if failed, true otherwize
 * \ingroup send
 * \sa ucxhPARSER_addBool(), ucxhPARSER_addSignedInt(), ucxhPARSER_addUnsignedInt(), ucxhPARSER_addString(),
 * \sa ucxhPARSER_addByteArray(), ucxhPARSER_addIpV4Address(), ucxhPARSER_addIpV6Address()
 * \sa ucxhPARSER_sendBuiltCommand()
 * \sa ucxhPARSER_buildRequestCmd(), ucxhPARSER_buildActionCmd()
 * \sa ucxhPARSER_sendCommand()
 */
bool ucxhPARSER_buildSetCmd(const char *cmd);

/** Start building an AT command on the form `AT+UUU`
 * If a previously (or partly) build AT command hasn't been sucessfully sent with
 * ucxhPARSER_sendBuiltCommand(), that command is disregarded.
 * The module must be in EDM or AT mode.
 * If not, invoke ucxhMODULE_enterExtendedDataMode() or ucxhMODULE_enterATMode() first.
 * \param cmd Command name excluding "AT"
 * \returns false if failed, true otherwize
 * \ingroup send
 * \sa ucxhPARSER_sendBuiltCommand()
 * \sa ucxhPARSER_buildSetCmd(), ucxhPARSER_buildRequestCmd()
 * \sa ucxhPARSER_sendCommand()
 */
 bool ucxhPARSER_buildActionCmd(const char *cmd);

/** Start building an AT command on the form `AT+UUU?=[\<params\>]`
 * If a previously (or partly) build AT command hasn't been sucessfully sent with
 * ucxhPARSER_sendBuiltCommand(), that command is disregarded.
 * The module must be in EDM or AT mode.
 * If not, invoke ucxhMODULE_enterExtendedDataMode() or ucxhMODULE_enterATMode() first.
 * \param [in] cmd Command name excluding `"AT"` and `'?'`
 * \returns false if failed, true otherwize
 * \ingroup send
 * \sa ucxhPARSER_sendBuiltCommand()
 * \sa ucxhPARSER_buildSetCmd(), ucxhPARSER_buildActionCmd()
 * \sa ucxhPARSER_sendCommand()
 */
 bool ucxhPARSER_buildRequestCmd(const char *cmd);

/** 
 * \brief Adds a boolean value to an AT-command being built with ucxhPARSER_buildSetCmd()
 * The value is formatted correctly, commas handled, total command length is checked.
 * Finalize and send the command using ucxhPARSER_sendBuiltCommand().
 * \param [in] value boolean to send
 * \returns false if failed, true otherwize.
 * \ingroup send
 * \sa ucxhPARSER_addBool(), ucxhPARSER_addSignedInt(), ucxhPARSER_addUnsignedInt(), ucxhPARSER_addString(),
 * \sa ucxhPARSER_addByteArray(), ucxhPARSER_addIpV4Address(), ucxhPARSER_addIpV6Address()
 * \sa ucxhPARSER_buildSetCmd(), ucxhPARSER_buildRequestCmd(), ucxhPARSER_buildActionCmd()
 * \sa ucxhPARSER_sendBuiltCommand()
 * \sa ucxhPARSER_sendCommand()
 */
bool ucxhPARSER_addBool(bool value);

/** 
 * \brief Adds a signed integer value to an AT-command being built with ucxhPARSER_buildSetCmd()
 * The value is formatted correctly, commas handled, total command length is checked.
 * Finalize and send the command using ucxhPARSER_sendBuiltCommand().
 * \param [in] value signed integer to send.
 * \returns false if failed, true otherwize.
 * \ingroup send
 * \sa ucxhPARSER_addBool(), ucxhPARSER_addSignedInt(), ucxhPARSER_addUnsignedInt(), ucxhPARSER_addString(),
 * \sa ucxhPARSER_addByteArray(), ucxhPARSER_addIpV4Address(), ucxhPARSER_addIpV6Address()
 * \sa ucxhPARSER_buildSetCmd(), ucxhPARSER_buildRequestCmd(), ucxhPARSER_buildActionCmd()
 * \sa ucxhPARSER_sendBuiltCommand()
 * \sa ucxhPARSER_sendCommand()
 */
bool ucxhPARSER_addSignedInt(int32_t value);

/** 
 * \brief Adds an unsigned integer value to an AT-command being built with ucxhPARSER_buildSetCmd()
 * The value is formatted correctly, commas handled, total command length is checked.
 * Finalize and send the command using ucxhPARSER_sendBuiltCommand().
 * \param [in] value unsigned integer to send.
 * \returns false if failed, true otherwize.
 * \ingroup send
 * \sa ucxhPARSER_addBool(), ucxhPARSER_addSignedInt(), ucxhPARSER_addUnsignedInt(), ucxhPARSER_addString(),
 * \sa ucxhPARSER_addByteArray(), ucxhPARSER_addIpV4Address(), ucxhPARSER_addIpV6Address()
 * \sa ucxhPARSER_buildSetCmd(), ucxhPARSER_buildRequestCmd(), ucxhPARSER_buildActionCmd()
 * \sa ucxhPARSER_sendBuiltCommand()
 * \sa ucxhPARSER_sendCommand()
 */
bool ucxhPARSER_addUnsignedInt(uint32_t value);

/** 
 * \brief Adds NUL-terminated string to an AT-command being built with ucxhPARSER_buildSetCmd()
 * The string is formatted correctly, commas handled, total command length is checked.
 * Finalize and send the command using ucxhPARSER_sendBuiltCommand().
 * \param [in] s NUL-terminated string  to send.
 * \returns false if failed, true otherwize.
 * \ingroup send
 * \sa ucxhPARSER_addBool(), ucxhPARSER_addSignedInt(), ucxhPARSER_addUnsignedInt(), ucxhPARSER_addString(),
 * \sa ucxhPARSER_addByteArray(), ucxhPARSER_addIpV4Address(), ucxhPARSER_addIpV6Address()
 * \sa ucxhPARSER_buildSetCmd(), ucxhPARSER_buildRequestCmd(), ucxhPARSER_buildActionCmd()
 * \sa ucxhPARSER_sendBuiltCommand()
 * \sa ucxhPARSER_sendCommand()
 */
bool ucxhPARSER_addString(const char *s);

/** 
 * \brief Adds binary data to an AT-command being built with ucxhPARSER_buildSetCmd()
 * The data is converted to a ByteArray, where each byte is converted to two ascii character hex numbers,
 * formatted correctly, commas handled, total command length is checked.
 * Finalize and send the command using ucxhPARSER_sendBuiltCommand().
 * \param [in] size size of data in bytes.
 * \param [in] data binary data to send.
 * \returns false if failed, true otherwize.
 * \ingroup send
 * \sa ucxhPARSER_addBool(), ucxhPARSER_addSignedInt(), ucxhPARSER_addUnsignedInt(), ucxhPARSER_addString(),
 * \sa ucxhPARSER_addByteArray(), ucxhPARSER_addIpV4Address(), ucxhPARSER_addIpV6Address()
 * \sa ucxhPARSER_buildSetCmd(), ucxhPARSER_buildRequestCmd(), ucxhPARSER_buildActionCmd()
 * \sa ucxhPARSER_sendBuiltCommand()
 * \sa ucxhPARSER_sendCommand()
 */
bool ucxhPARSER_addByteArray(size_t size, const uint8_t *data);

/** 
 * \brief Adds NUL-terminated string containing an IPv4-address to an AT-command being built with ucxhPARSER_buildSetCmd()
 * The string is formatted correctly, commas handled, address checked for formatting, total command length is checked.
 * Finalize and send the command using ucxhPARSER_sendBuiltCommand().
 * \param [in] s NUL-terminated string to send.
 * \returns false if failed, true otherwize.
 * \ingroup send
 * \sa ucxhPARSER_addBool(), ucxhPARSER_addSignedInt(), ucxhPARSER_addUnsignedInt(), ucxhPARSER_addString(),
 * \sa ucxhPARSER_addByteArray(), ucxhPARSER_addIpV4Address(), ucxhPARSER_addIpV6Address()
 * \sa ucxhPARSER_buildSetCmd(), ucxhPARSER_buildRequestCmd(), ucxhPARSER_buildActionCmd()
 * \sa ucxhPARSER_sendBuiltCommand()
 * \sa ucxhPARSER_sendCommand()
 */
bool ucxhPARSER_addIpV4Address(const char *s);

/** 
 * \brief Finalize and send an AT-command being built with ucxhPARSER_buildSetCmd() in EDM or AT mode.
 * The command is formatted correctly, commas handled, total command length is checked.
 * If everything is ok, including all previous invocations to ucxhPARSER_addXXX(), the command
 * is sent to the module.
 * If the module fails to understand or execute the command,
 * the `ERROR` result string will arrive as an event to the callbacks previously registered 
 * with ucxhPARSER_setOnErrorReceived().
 * If the module succeeds, any resulting response codes will generate events to the callback
 * which has been registered using the corresponding
 * ucxhPARSER_setOnXXX() functions, or ucxhPARSER_setOnURCParsed(). Response codes not handled by the library, will 
 * generate events to the callback registered using ucxhPARSER_setOnUnknownURCReceived.
 * Finally, the `OK` result string will arrive as an event to the callbacks previously registered 
 * with ucxhPARSER_setOnOkReceived().
 * The module must be in EDM or AT mode.
 * If not, invoke ucxhMODULE_enterExtendedDataMode() or ucxhMODULE_enterATMode() first, 
 * and rebuild the command.
 * \returns false if failed, true if command has been sent to module. 
 * \ingroup send
 * \sa ucxhPARSER_addBool(), ucxhPARSER_addSignedInt(), ucxhPARSER_addUnsignedInt(), ucxhPARSER_addString(),
 * \sa ucxhPARSER_addByteArray(), ucxhPARSER_addIpV4Address(), ucxhPARSER_addIpV6Address()
 * \sa ucxhPARSER_buildSetCmd(), ucxhPARSER_buildRequestCmd(), ucxhPARSER_buildActionCmd()
 * \sa ucxhPARSER_sendBuiltCommand()
 * \sa ucxhPARSER_sendCommand()
 */
bool _ucxhPARSER_sendBuiltCommand(const char *file, uint32_t line) ;
#define ucxhPARSER_sendBuiltCommand() _ucxhPARSER_sendBuiltCommand(__FILE__, __LINE__)


///////////////////////////////////////////////////////////////////////////////

#endif