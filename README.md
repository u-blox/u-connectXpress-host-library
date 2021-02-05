# Introduction

The u-connectXpress host library is an example on how to interface the
u-blox standalone shortrange modules with a host of any kind.
The AT commands are normally transmitted in the AT mode, but
this library also supports transmitting AT commands and receiving data using the
Extended Data Mode (EDM), which significantly simplifies the development of
more advanced products on the host MCU.

The u-connectXpress AT protocol is implemented in the u-connectXpress software for the following u-blox standalone shortrange modules:

* ANNA-B1
* NINA-B1
* NINA-B2
* NINA-B31
* NINA-B41
* NINA-W13
* NINA-W15
* ODIN-W2

Use this library on the host to

* Transmit AT commands to the module using AT mode or EDM
* Receive and manage URC responses using  AT mode or EDM
* Transmit raw data to any peer connected to the module using Data mode or EDM
* Receive raw data from any peer connected to the module using Data mode or EDM

While this example is written for Windows, it is trivial to convert it to any other host, 
including Raspberry PI, FreeRTOS, or hostless embedded MCUs such as STMicroelectronics STM32 series.

## Usage

### Running your first example on CygWin

The example will issue some commands to your module, and display the responses.

+ Connect a NINA-W13, NINA-W15 or ODIN-W2 to your computer.
+ Define the serial port to use in the `include/ucxh_config.h` file.
+ Open a command terminal on your PC and start 

      ./runExamples.sh --run ex_readmisc

\ref ex_readmisc "More information ..." 

### Running the second examples on CygWin

This example will show how to communicate with another device over WiFi, using 
EDM between your NINA-W13, NINA-W14 or ODIN-W2 module and your PC.

+ Connect a NINA-W13, NINA-W15 or ODIN-W2 to your computer.
+ Define the serial port to use in the `include/ucxh_config.h` file.
+ Configure your network and create a small server.
  This can be done either by connecting to your existing network, and using your computer as a server, or 
  by using another NINA-W10/W13 or ODIN-W2 as WiFi AP and/or server:

  * To use another u-blox module as WiFi AP and server, configure it to use
    IP number and gateway address `192.168.2.1`, and  enable a TCP server on port `12345`.
    This is easiest done using `s-center`.

     + In s-center, connect to the u-blox standalone module you intend to use as WiFI AP.
     + From the File menu, select Download Configuration
       and select the file `resources/config_ex_callbacks_edm_wifi.txt`.
     + Once the module has activated the WiFi AP, enter Data Mode, by pressing the button at the bottom.
     + Open a command terminal on your PC and start 

            ./runExamples.sh --run ex_callbacks_edm_wifi

     + When propmted by the example, type something into the s-center input box.
       Some text appears on the program's terminal, and the example will echo back the result.

  * To use your own network and PC as server you need some form of program capable of accepting 
    incoming connections on your pc, such as `nc`.

      + Edit the `examples/ex_config.h` file, and replace the SSID, and password with the one
        used on your network, and the IP-number with your PC's IP-number.
      + Open a command terminal on your PC, start netcat using 

            nc -l -k 12345

      + Open a new terminal on your PC and start

            ./runExamples.sh --run ex_callbacks_edm_wifi

      + When propmted by the example, type something into netcat's terminal.
        Some text appears on the program's terminal, and the example will echo back the result.
  
Independent on method, the example will quit automatically after 2 minutes. It can also
be aborted by sending the line `quit` from the server.

\ref ex_callbacks_edm_wifi "More information ..." 

### Using the AT command library with your host application

+ ensure your `include/ucxh_config.h` is configured properly
+ Compile and link the files
in the `src` directory as well as the
`src/windows/ucxh_serial_win.c` file with your application.

## Documentation

See the examples for various use-cases

For full documentation see the `html/index.html` file.

For documentation on the EDM packet format, see the
u-blox Extended Data Mode Protocol Specification at
https://www.u-blox.com/en/docs/UBX-14044126

For documentation on the u-blox u-connectXpress AT command set
for the u-blox shortrange standalone modes,
see the u-connect AT commands manual at
https://www.u-blox.com/en/docs/UBX-14044127


## Examples

### ex_readmisc

This example shows how to read various parameters from the module.

\ref ex_readmisc "More information ..." 

### ex_callbacks_edm_wifi

This example is a shows how to connect to a remote host using EDM:

 + Enter Extended Data Mode (EDM)
 + Configure the module as a WiFi station
 + Wait until the WiFi link is up and the module has received a DHCP IP address.
 + Connect to a server (for example a PC)
 + Transmit raw data to the server
 + Receive raw data from the server
 + Reboot
 
\ref ex_callbacks_edm_wifi "More information ..." 

### ex_wifi_pos

This example shows how to scan for APs and request position from Thingstream WiFi positioning server using MQTT over TLS:

 + Enter Extended Data Mode (EDM)
 + Configure the module to connect to an AP
 + Connect to a Thingstream WiFi positioning server at mqtt.thingstream.io using MQTT over TLS
   and the credentials configured in ex_config.h
 + Scan for nearby APs
 + Publish the list of APs to the Thingstream WiFi positioning server using MQTT
 + Obtain and print the position as determined by the Thingstream WiFi positioning server 

\note In order for this example to work, you must obtain a Thingstream account
and configure the flow for a WiFi positioning server. See the u-connectXpress IoT
cloud connectivity application note at https://www.u-blox.com/en/docs/UBX-19010078 for
more information.

\ref ex_wifi_pos "More information ..." 

### ex_http_get

This example is a shows how to connect to a HTTP-server and issue a GET-command:

 + Enter Extended Data Mode (EDM)
 + Configure the module as a WiFi station
 + Wait until the WiFi link is up and the module has received a DHCP IP address.
 + Connect to a remote server.
 + Reboot
 
\ref ex_http_get "More information ..." 

## Porting

The library has been tested using gcc on CygWin, with the only file which contains
Microsoft Windows-specific code, is ucxh_serial_win.c, in the `src/windows` directory.

Replace that file with code which implements the interface described in serial.h .

In order to port to an RTOS, such as FreeRTOS, a few lines needs to be changed in atParseResponse()
of the `src/ucxh_parser.c` file

## Testing

A number of test programs for unit-tests, and some programs 
with an expected output are also available, when `runExamples.sh` is executed:

### Unit-tests

#### test_getgeneral 

Unit-test for the generation and parsing of General commands from chapter 3 in the u-connectXpress AT command manual.

\ref test_getgeneral.c "More information ..." 

#### test_getsystem

Unit-test for the generation and parsing of System commands from chapter 4 in the u-connectXpress AT command manual.

\ref test_getsystem.c "More information ..." 

#### test_at_wifi

Unit-test for the generation and parsing of WiFi commands from chapter 7 and some Network command from chapter 10
in the u-connectXpress AT command manual.

\ref test_at_wifi.c "More information ..." 

#### test_tokenizer

Unit-test for the tokenizer part of the library.

\ref test_tokenizer.c "More information ..." 

#### test_send

Unit-testing the functions used for building and sending AT commands.

\ref test_send.c "More information ..." 

#### test_mock_ati9

Unit-testinf for the actual mocking framework, mimicing the behavior of `\ref test_ati9.c` .

\ref test_mock_ati9.c "More information ..." 


### Manual test

#### test_ati9

This test is a basic "Hello, World",  which - using AT mode - tests

 + Open the serial port
 + Enter AT mode
 + Transmit an AT command
 + Receive the response
 + Close the serial port

\ref test_ati9.c "More information ..." 

#### test_datamode

This test is a partial test which  - using AT mode and Data mode - tests

 + Enter the Data mode
 + Transmit raw data in the Data mode

\ref test_datamode.c "More information ..." 

#### test_edm_at

This test is a basic Hello, World, which - using EDM - tests

 + Enter EDM mode
 + Transmit an AT command
 + Receive the response
 + Leave EDM mode

\ref test_edm_at.c "More information ..." 


#### test_edm_at_read

This test is a basic Hello, World, which - using EDM - tests

 + Enter EDM mode
 + Ensure nothing is received apart from responses from AT-c
 + Leave EDM mode

\ref test_edm_at.c "More information ..." 


All testing has been done using gcc 9.2.0 on CygWin connected to either NINA-W152 2.1.0 or NINA-W132 2.1.0.

## Using runExamples.sh

By default, it will

* Run all examples
* Execute all automatic unit-tests
* Execute all manual unit-tests
* Re-generate all documentation (assuming `doxygen` and `graphwiz` is installed).


## Revision history

### 0.3 

Initial public release.

### 0.4

New examples:
 * ex_wifi_pos: Example shows how to scan for APs and request position from Thingstream WiFi positioning server using MQTT over TLS
  \ref ex_wifi_pos "More information ..." 
 * ex_http_get: Example shows how to connect to a HTTP-server and issue a GET-command
   \ref ex_http_get "More information ..." 

Fixed some bugs:
 * EDM parse failed for incomingg packets > 128 bytes
 * Potential buffer overflow in createUrlQueryString()
 * Non-portable integer conversion in STRCPY2INT()

## Disclaimer
Copyright (C) u-blox 

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