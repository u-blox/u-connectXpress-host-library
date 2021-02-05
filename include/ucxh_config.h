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


#ifndef _UCXH_AT_CONFIG_H
#define _UCXH_AT_CONFIG_H

/** 
 * \defgroup tracingconf Tracing configuration
 * \ingroup configuration
 * \brief Configurations to enable tracing/logging of the internals of the u-connectXpress host library.
 * @{
 */


/** 
 * Tracelevel parsing of incoming EDM packets in EDM mode.
 * 
 * \param Level
 * + 0: No output upon reception of EDM packet.
 * + 1: Display error during EDM packet parsing.
 * + 2: Display EDM packet type, such as ucxhEDM_EVENT_CONNECT_BT,
 * unless a callback has been registered.
 * excluding AT reponse and DATA packets,
 * + 3: Also display AT response packet type,
 * even if callback has been registered. 
 * + 4: Also display content of AT response packets.
 * + 5: Also display DATA packet type.
 * even if callback has been registered.
 * + 6: Also display content of DATA packets.
 * + 7: Also display line extracted from AT response packet,
 * as it will be sent to the AT parser.
 */
#define UCXH_PARSER_TRACE_EDM_PARSER 1


/** 
 * Tracelevel low-level parsing of incoming EDM packets in EDM mode.
 * 
 * \param Level
 * + 0: No output upon low-level of EDM packet.
 * + 1: Display error during low-level EDM packet parsing.
 * + 2: Display synchronization during low-level EDM packet parsing.
 */
#define UCXH_PARSER_TRACE_EDM_LL_PARSER 1


/** 
 * Tracelevel parsing of incoming AT URCs in AT or EDM mode by generic parser.
 * 
 * \param Level
 * + 0: No output upon reception of AT URC or DATA.
 * + 1: Display errors during parsing of AT URC
 * + 2: Also display AT URC, unless a callback has been registered.
 * + 3: Also dispaly AT URC to be handled by callback
 * + 4: Also dispaly tokens as parsed and returncodes from callbacks
 * + 5: Also display if internal callbacks aren't invoked
 */
#define UCXH_PARSER_TRACE_AT_PARSER 1

/** 
 * Tracelevel parsing of incoming known AT URCs in AT or EDM mode by individual parser.
 * 
 * \param Level
 * + 0: No output upon reception of known AT URCs.
 * + 1: Display errors during parsing of known AT URCs
 * + 2: Display content of known AT URCs prior to verification of parameter format
 * + 3: Display content of known AT URCs during verification of asserts()
 */
#define UCXH_PARSER_TRACE_URC 1


/** 
 * Tracelevel transmission of AT commands in AT and EDM mode.
 * \param Level
 * + 0: No output upon transmission of AT commands in AT and EDM mode.
 * + 1: Display errros during building of AT commands in AT or EDM mode.
 * + 2: Display content of outgoing AT commands in AT and EDM mode.
 * + 3: Display enqueued transmissions
 * + 4: Display enqueued transmissions's alloc/dealloc
 * + 5: Display even more information
 */
#define UCXH_PARSER_TRACE_AT 1

/** 
 * Tracelevel waitXXX() functions in test-mode
 * \param Level
 * + 0: No output upon waitXXX()
 * + 1: Display errros during waitXXX()
 * + 2: Display output during waitXXX()
 * + 3: Display loads of output during waitXXX()
 */
#define UCXH_PARSER_TRACE_WAIT 1

/** 
 * Tracelevel XXX_poll() functions
 * \param Level
 * + 0: No output upon XXX_poll()
 * + 1: Display errrors during XXX_poll()
 * + 2: Display output during XXX_poll()
 * + 3: Display output duringe every invocation of reading in XXX_poll()
 */
#define UCXH_PARSER_TRACE_POLL 1

/** 
 * Tracelevel sending of DATA in datamode and EDM.
 * \param Level
 * + 0: No output upon outgoing DATA in datamode and EDM.
 * + 1: Display errors during sending of DATA in datamode and EDM.
 * + 2: Display when DATA event are sent in datamode and EDM.
 * + 3: Also display content of outgoing DATA in datamode and EDM.
 */
#define UCXH_PARSER_TRACE_SEND_DATA 1


/** 
 * Tracelevel WiFi-specific .
 * \param Level
 * + 0: No output upon WiFi-specific events and commands.
 * + 1: Display errors during handling of WiFi-specific events and commands.
 * + 2: Display whenever WiFi-specific events occurrs.
 * + 3: Also display whenever WiFi-specific commands are issued.
 * + 4: Also display internal tracing.
 * + 5: Also display internal tracing during building the commands to issue.
 */
#define UCXH_PARSER_TRACE_WIFI 1



/** @} */


///////////////////////////////////////////////////////////////////////////////


/** 
 * \defgroup targetconf Target-specific configuration
 * \ingroup configuration
 * \brief Configurations related to the specific host target.
 * Currently, CygWin on Windows is the only supported host target.
 * @{
 */

/** 
 * Serial port to use. 
 * \param Portname
 * On windows, use the format `"\\\\.\\COM44"`
 */
#ifndef UCXH_PARSER_COMPORT
#define UCXH_PARSER_COMPORT "\\\\.\\COM20"
#endif

/** @} */



///////////////////////////////////////////////////////////////////////////////

/**
 * Turn A into a string literal without expanding macro definitions
 * (however, if invoked from a macro, macro arguments are expanded).
 */
#define STRINGIZE_NX(A) #A

/**
 * Turn A into a string literal after macro-expanding it.
 */
#define STRINGIZE(A) STRINGIZE_NX(A)

#define udprintf(define, level, fmt, ...) do { if (define >= level) { printf(fmt, __VA_ARGS__); }} while(0)

#endif
