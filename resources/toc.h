///////////////////////////////////////////////////////////////////////////////

/* Desired order
communication OK
    introduction    <-- NEW!
    Commands -- communication -- RENAME to control  OK
        modulecommands -- RENAME to modulecontrol  ok
        Sending commands -- send
        Control using EDM -- edmsend
        Control using AT and data Mode --atsend
        directat
    receiving

    callbacks
        introduction    <-- NEW!
        modulecallbacks <-- NEW!
        edmcallbacks
        parsercallbacks
        urccallbacks -- fix
            urcevents
                urcevents_wifi
                urcevents_network
            urceventparams
                urcevent_ip_params
                urcevent_network_params
                urcevent_bt_params

configuration -- unimportant order, each subchapter is defined in each headerfile
    tracing
    targetconf
    exampleconf

Internal documentation  misc <-- NEW!
    utility
    internal
    busywaits
*/

///////////////////////////////////////////////////////////////////////////////

/*! \defgroup communication Communication with the module
 * \brief The u-connectXpress host library allows easy integration of the
 * u-connectXpress-capable modules into the customer's design using
 * the command-callback framework.
 */


///////////////////////////////////////////////////////////////////////////////

/*! \defgroup control Sending data and controlling
* \ingroup communication
* \brief Controlling the module is done using a combination of functions that are 1-to-1
* mappings of the AT commands documented in the u-connect AT commands manual,
* and functions performing combination of commands. Responses from the module
* are optionally returned as callback.
*/

/*! \defgroup modulecontrol Controlling the module
* \ingroup control
* \brief Functions that controls the module in a way that requires more than transmitting a simple AT command.
*/

/*! \defgroup directat AT commands
* \ingroup control
* \brief Functions that are a 1-to-1 mapping of AT commands.
*/

/*! \defgroup send Direct sending to the module independent of mode
* \ingroup control
* \brief Functions to invoke in order to send generic AT commands and data independent of 
* if the module is in AT, DATA or Extended Data Mode (EDM).
*
* For sending of AT commands, it is generally better to invoke
* ucxhPARSER_sendCommand() or ucxhPARSER_sendAtCommands(), rather than 
* the mode-specific functions  the ucxhAT_sendCommand() or ucxhEDM_sendCommand().
* \note These functions requires that AT or Extended Data Mode (EDM) has been actived
* using the ucxhMODULE_enterATMode() or ucxhMODULE_enterExtendedDataMode().
*/

/*! \defgroup edmsend Direct sending to the module using EDM
* \ingroup control
* \brief Functions to invoke in order to send generic AT commands over the EDM AT channel, 
* or data over a selected EDM data channel.
*
* For sending of AT commands, it is generally better to invoke
* ucxhPARSER_sendCommand() or ucxhPARSER_sendAtCommands(), rather than 
* the ucxhEDM_sendCommand() or ucxhEDM_sendAtCommands().
* \note These functions requires that EDM has been actived using the ucxhMODULE_enterExtendedDataMode()
*/

/*! \defgroup atsend Direct sending to the module using AT and DATA mode
* \ingroup control
* \brief Functions to invoke in order to send generic AT commands in AT mode
* or data in DATA mode.
*
* For sending of AT commands, it is generally better to invoke
* ucxhPARSER_sendCommand() or ucxhPARSER_sendAtCommands(), rather than 
* the ucxhAT_sendCommand() or ucxhAT_sendAtCommands().
* \note These functions requires that AT or DATA mode has been actived using the 
* ucxhMODULE_enterATMode() or ucxhMODULE_enterDataMode().
*/



/*! \defgroup receiving Receiving data from the module
* \ingroup communication
* \brief Functions that are needed to be invoked at regular intervals in order to
* parse the incoming data and convert it to callbacks.
*/


///////////////////////////////////////////////////////////////////////////////



/*! \defgroup callbacks Callbacks
* \ingroup communication
* \brief Upon reception of data from the module, optional callbacks previously registered
* by the application will be invoked.
*
* Several types of callbacks exist.
*   + Module Callbacks are optionally invoked when the module changes state, 
*     not related to a specific AT command, URC or EDM Event, or as a result of functions
*     involving several AT commands.
*   + EDM Callbacks are optionally invoked when an Extended Data Mode (EDM) packet has arrived,
*     or as a result of an EDM event.
*   + Parser callbacks are optinally invoked when an AT OK or AT ERROR response is received,
*     and/or upon the reception of parsed URC or unknown URC, independent of if the module
*     is in EDM, AT or DATA mode.
*   + URC callbacks are optionally invoked upon receiption of URCs.
* 
* Callbacks can be specified at several different levels:
*
*   + Callback to be invoked as result of a specific invokation to a specific control function.
*   + Callback to be invoked as result of the completion of all invocations of a specific control function, unless overridden.
*   + Callback to be invoked as result of the completion of all control functions, unless overridden.
*   + Callback to be invoked upon reception of specific URCs.
*   + Callback to be invoked upon handling of a all known URCs, unless overridden.
*   + Callback to be invoked for all unknown URCs.
*   + Callback to be invoked upon reception of data.
*   + Callback to be invoked upon specific EDM events.
*
* No callback is mandatory to use, although a usable application will need to specify at least a callback for reception of data.
*/


/*! \defgroup modulecallbacks Module Callbacks
* \ingroup callbacks
* \brief Module Callbacks are optionally invoked when the module changes state, 
* not related to a specific AT command, URC or EDM Event, or as a result of functions
* involving several AT commands.
*
* Some of these callbacks may be invoked independent of if the module is in AT, EDM
* or DATA mode, others are dependent of the current mode.
* Register the appropriate callbacks using the corresponding `ucxhYYY_setOnXXX()` function.
*/


/*! \defgroup edmcallbacks EDM Callbacks
* \ingroup callbacks
* \brief EDM Callbacks are optionally invoked when an Extended Data Mode (EDM) packet has arrived, or as a result of 
* an EDM event.
*
* These callbacks are not invoked when the module is in AT or DATA mode.
* Register the appropriate callbacks using the corresponding `ucxhEDM_setOnXXX()` function.
*/

/*! 
* \defgroup parsercallbacks Parser callbacks
* \ingroup callbacks
* \brief Parser callbacks are optinally invoked when an AT OK or AT ERROR response is received,
* and/or upon the reception of parsed URC or unknown URC, independent of if the module
* is in EDM, AT or DATA mode.
*
* These callbacks are invoked independent of if the module is in EDM or AT mode.
* These callbacks are not invoked when the module is in DATA mode.
* Depending on the AT command which initiated the callback, 
* there may may be more than one URC, or unknown URCs transmitted from the module, in addition
* to the OK response. In that case, each URC or unknown URC is handled through a separate invocation 
* to the callbacks registered by ucxhPARSER_setOnURCParsed() or ucxhPARSER_setOnUnknownURCReceived().
* When OK or ERROR is received from the module, the callbacks registered using either
* ucxhPARSER_setOnOkReceived() or ucxhPARSER_setOnErrorReceived() is invoked.
* 
* Register the appropriate callbacks using the corresponding `ucxhPARSER_setOnXXX()` function.
*/



///////////////////////////////////////////////////////////////////////////////


/*!
* \defgroup urccallbacks URC Callbacks
* \ingroup callbacks
* \brief URC Event Callbacks are optionally invoked upon reception of a known URC or command request.
*
* Registered the appropriate callback using the URC's corresponding `ucxhPARSER_setOnUUU()` function.
*/


/*!
* \defgroup urcevents URC Events
* \ingroup urccallbacks
* \brief Events optinally generated upon reception of URCs.
*/



/*!
* \defgroup urcevents_general General Events
* \ingroup urcevents
* \brief Events optionally generated upon reception of URCs and command requests related to General commands
* See chapter 3 of the u-connect AT Commands Manual.
*/

/*!
* \defgroup urcevents_system System Events
* \ingroup urcevents
* \brief Events optionally generated upon reception of URCs and command requests related to General commands
* See chapter 4 of the u-connect AT Commands Manual.
*/

/*!
* \defgroup urcevents_data Peer and Local Server Events
* \ingroup urcevents
* \brief Events optionally generated upon reception of URCs and command requests related to Peer and Local Server.
* See chapter 5 of the u-connect AT Commands Manual.
*/

/*!
* \defgroup urcevents_network Network Events
* \ingroup urcevents
* \brief Events optionally generated upon reception of URCs and command requests related to Network
* See chapter 7 of the u-connect AT Commands Manual.
*/

/*!
* \defgroup urcevents_wifi  WiFi Events
* \ingroup urcevents
* \brief Events optionally generated upon reception of URCs and command requests related to WiFi.
* See chapter 10 of the u-connect AT Commands Manual.
*/


/*!
* \defgroup urceventparams URC Event Parameters
* \ingroup urccallbacks
* \brief Parameter types included in the events generated upon reception of URCs
* and command requests.
*/

/*!
* \defgroup urcevents_general_params General Event parmeters
* \ingroup urceventparams
* \brief Events optionally generated upon reception of URCs and command requests related to General commands
* See chapter 3 of the u-connect AT Commands Manual.
*/

/*!
* \defgroup urcevents_system_params System Event parmeters
* \ingroup urceventparams
* \brief Events optionally generated upon reception of URCs and command requests related to General commands
* See chapter 4 of the u-connect AT Commands Manual.
*/

/*!
* \defgroup urcevents_data_params Peer and Local Server Event parmeters
* \ingroup urceventparams
* \brief Events optionally generated upon reception of URCs and command requests related to Peer and Local Server.
* See chapter 5 of the u-connect AT Commands Manual.
*/

/*!
* \defgroup urcevents_bt_params  Bluetooth Event parameters
* \ingroup urceventparams
* \brief Parameter types related to Bluetooth
* See chapters 6 of the u-connect AT Commands Manual.
*/

/*!
* \defgroup urcevents_ip_params  IP Event parameters
* \ingroup urceventparams
* \brief Parameter types related to IP
* See chapters 7 and 10 of the u-connect AT Commands Manual.
*/

/*!
* \defgroup urcevents_network_params  Network Event parameters
* \ingroup urceventparams
* \brief Parameter types related to Network
* See chapter 10 of the u-connect AT Commands Manual.
*/




/*! 
 * \defgroup eventcreation Event creation functions
 * \ingroup urccallbacks
 * \brief Functions which converts string parameters of a URCs to events. 
 */



/*! 
* \defgroup serial Low-level plaform-dependent serial communication
* \ingroup communication
* \brief Functions that need to be ported for each supported platform.
*/

/*! 
* \defgroup autogenerated Callback registration
* \ingroup callbacks
* \brief Autogenerated functions for callback registration.
* \sa extending
*/

///////////////////////////////////////////////////////////////////////////////


/** 
 * \defgroup configuration Configuration options
 * \brief Defines that may be needed to modified to match the system
 * and application.
 */



///////////////////////////////////////////////////////////////////////////////

/*! \defgroup extending Extending the library
 * \brief Instructions to support new commands, requests and URCs.
 * 
 * To add the parsing of an URC or responses from a command request,
 * such as "NFC URI tag content" `+UNFCURI`, follow these instructions, but
 * replace `XXX` with the CamelCase name of the URC, such as
 * `NfcUriTagContent`, and `UUU` with the actual URC, such as `UNFCURI`.
 *
 *  -# Add a line to the \ref LIST_OF_PLUS_URC #define in ucxh_urc.h .
 *  -# Add a typedef struct  named `ucxhPARSER_urcXXXEvent_t`, containing
 *  the parsed parameters from the URC to the ucxh_urc.h file.
 *    * Ensure the field names in the struct is named as they are named 
 *      in the AT command manual, UNLESS the name in the AT command manual
 *      is OBVIOUSLY incorrect, in which case the AT command manual should
 *      be changed. If so, please contact u-blox through your normal channel,
 *      or file a question on the u-blox forum https://portal.u-blox.com/s/
 *    * Use an enum and union to differentiate between different formats of the URC, 
 *      e.g. URCs that contains either IP adresses or Bluetooth adresses.
 *      For an example, see `ucxhPARSER_urcPeerConnectedEvent_t`. 
 *  -# Implement a function with the function definition
 *      \code
 *        extern ucxhURC_parseResult_t onUUU_createEvent(uint8_t numParams, const char *ppParams[], ucxhPARSER_urcXXXEvent_t *pEvent);
 *      \endcode
 * 
 *     * `numParams` Indicates the number if parameters in the ppParms array, or 0.
 *     * `ppParams` It points to an array of NUL-termitted pointers to each parameter, or NULL.
 *     * `pEvent` Is allocated and deallocated 
 *        by the caller (and typically placed on the stack).
 *        The function must assume its content is uninitialized.
 *        Only update its content upon successfully parsing the parameters.
 * 
 *     * Return
 *        * `ucxhURC_PARSE_RESULT_OK` if `pEvent` is ready to be issued to a registered callback,
 *        * `ucxhURC_PARSE_RESULT_NEED_MORE` if not.
 *          In such case, the content of `pEvent` shall be undefined,
 *          and will not be forwarded to any registered callback.
 *          Instead, the `OK` response from the module will cause this function to be
 *          invoked again, but with `0, NULL` as `numParams` and `ppParams`.
 *        * `ucxhURC_PARSE_RESULT_ERROR` if parsing failed and it is impossible to generate a valid `pEvent`.
 * 
 *     * The function is responsible for parsing the content of `pParams`,
 *       and fill the `pEvent` with the data.
 * 
 *       * If `numParams` is 0 AND `ppParams` is NULL, the function is invoked as a result of
 *         an `OK` response.
 *       * If the function previously returned `ucxhURC_PARSE_RESULT_NEED_MORE`,
 *         the function MUST fill the `pEvent` with the result parsed so far and return
 *         `ucxhURC_PARSE_RESULT_OK`.
 *       * The function must be liberal with parsing the content of `pParams`,
 *         for example: 
 *         + Accept undocumented values and values out-of-range. However, logging
 *           the presence of such value is recommended.
 *           It is the responsibility of the client which registers itself
 *           as recipient of callbacks to the event, to handle undocumented
 *           values and values out of range. 
 *         + Allow (and disregard) more parameters than documented
 *         + Accept empty values - if so, set the corresponding field to 0 or empty string.
 * 
 *     * The function may use a number of utility macros and functions, such as
 *       * ASSERTPARAMS()
 *       * STRCPY2ARRAY()
 *       * STRCPY2ARRAYORFAIL()
 *       * STRCPY2INT()
 *       * STRCPY2INTORFAIL()
 *       * ucxhPARSER_printBuffer()
 *       * ucxhPARSER_isIpV4Address()
 *       * ucxhPARSER_isValidIpV4Address()
 *       * ucxhPARSER_isIpV6Address()
 *       * ucxhPARSER_isValidIpV6Address()
 * 
 *     * For trivial events, that only contains a single parameter, \ref TRIVIAL_EVENT can also be used to define the entire function.
 * 
 *   \warning Do NOT use pointers in the returned `pEvent`.
 *   \warning Do NOT save any pointer to the content of `ppParams` for future use.
 * 
 *   \note Do NOT create a function prototype, since that is done by the
 *   X-MACROS. The X-MACROs will also generate a function pointer and setter for the callback registration
 *   with the names
 *   \code
 *   bool ucxhPARSER_setOnXXX(int (*callback)(ucxhPARSER_urcXXXEvent_t *pEvent));
 *   bool ucxhPARSER_setOnXXXOk(int (*callback)(void *_));
 *   bool ucxhPARSER_setOnXXXError(int (*callback)(void *_));
 *   \endcode

*/


///////////////////////////////////////////////////////////////////////////////

/*! \defgroup misc Miscellaneous functions
* \brief These functions are mostly helpers, utility or internal.
*/


/*! \defgroup utility Misc utility functions
* \ingroup misc
* \brief Small utilites, usable for application use.
*/

/*! \defgroup internal Internal functions
* \ingroup misc
* \brief These functions are only used by the library itself.
* \warning Do not invoke these functions from the user application. 
*/

/*! \defgroup busywaits Functions that busy-waits for timeout or external event
* \ingroup misc
* \brief These functions are currently implemented to busy-wait for a defined time,
* or until a specific condition is met.
*
* During waiting, the UART is checked for incoming data, AT or EDM packets are parsed,
* and the appropriate callback is invoked, depending on input.
* \note These functions exist only to simplify the development of the examples and tests. 
* \warning These functions will hog the MCU, making them unsuitable for use in a super-loop.
*/

