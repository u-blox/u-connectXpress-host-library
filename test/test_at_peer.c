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
#include <limits.h>
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
 * \file test_at_peer.c
 * \test test_at_peer.c Unit test: AT-commands UDDPRP and ucxhCOMMAND_writeDefaultRemotePeer() .
 * \brief Tests from chapter 5: AT-commands UDDPRP and ucxhCOMMAND_writeDefaultRemotePeer() .
 *  
## Introduction
 
## Typical response

### Description


*/



#define EXAMPLE_COM "example.com"


void test_COMMAND_connectPeer(void)
{
  #define ASSERT_UDCP_TCP1(hostname, str, act, prehost, posthost) \
  do { \
    ucxhPARSER_peerURL_t cfg = { \
      act, \
      .scheme = ucxhPARSER_PEER_URL_SCHEME_TCP, \
      .tcp.remote_port = 2000, \
      .tcp.domain = hostname, \
    }; \
    assert(ucxhCOMMAND_connectPeer(&cfg)); \
    returnOk(); \
    assert(ucxhSERIAL_wasStringWritten("AT+UDCP=\"tcp://" prehost hostname posthost ":2000/" str "\"\r\n")); \
  } while (0)

  #define ASSERT_UDCP_TCP_EXAMPLE_AC(str, act) \
    ASSERT_UDCP_TCP1(EXAMPLE_COM, str, act, "", "");

  ASSERT_UDCP_TCP_EXAMPLE_AC("?local_port=2001",           .tcp.query.local_port = 2001);

  printf("AT+UDCP: test_COMMAND_connectPeer OK\n========================================\n");
}


void test_COMMAND_writeDefaultRemotePeerSingleTcpParam(void)
{
  #define ASSERT_UDDRP_TCP1(hostname, str, act, connect_scheme_flag, connect_scheme_act, prehost, posthost) \
  do { \
    ucxhPARSER_defaultRemotePeerParam_t cfg = { \
      act, \
      connect_scheme_act, \
      .peer_id = 1, \
      .url.scheme = ucxhPARSER_PEER_URL_SCHEME_TCP, \
      .url.tcp.remote_port = 2000, \
      .url.tcp.domain = hostname, \
    }; \
    assert(ucxhCOMMAND_writeDefaultRemotePeer(&cfg)); \
    returnOk(); \
    assert(ucxhSERIAL_wasStringWritten("AT+UDDRP=1,\"tcp://" prehost hostname posthost ":2000/" str "\"," #connect_scheme_flag "\r\n")); \
  } while (0)

  #define ASSERT_UDDRP_TCP_EXAMPLE_AC(str, act) \
    ASSERT_UDDRP_TCP1(EXAMPLE_COM, str, act, 2, .connect_scheme.always_connected = true, "", "");
  #define ASSERT_UDDRP_TCP_EXAMPLE_EC(str, act) \
    ASSERT_UDDRP_TCP1(EXAMPLE_COM, str, act, 4, .connect_scheme.external_connect = true, "", "");
  #define ASSERT_UDDRP_TCP_EXAMPLE_IPV4(str, act) \
    ASSERT_UDDRP_TCP1("192.168.0.1", str, act, 4, .connect_scheme.external_connect = true, "", "");

  ASSERT_UDDRP_TCP_EXAMPLE_AC("?local_port=2001",           .url.tcp.query.local_port = 2001);
  ASSERT_UDDRP_TCP_EXAMPLE_AC("?flush_tx=1",                .url.tcp.query.flush_tx   = true);
  ASSERT_UDDRP_TCP_EXAMPLE_AC("?keepAlive=10000+5000+500",  .url.tcp.query.keep_alive.idle_ms     = 10000);
  ASSERT_UDDRP_TCP_EXAMPLE_AC("?keepAlive=10000+5000+500",  .url.tcp.query.keep_alive.interval_ms = 5000);
  ASSERT_UDDRP_TCP_EXAMPLE_AC("?keepAlive=10000+5000+500",  .url.tcp.query.keep_alive.count       = 500);
  ASSERT_UDDRP_TCP_EXAMPLE_AC("?ca=certauth.der",           .url.tcp.query.ca       = "certauth.der");
  ASSERT_UDDRP_TCP_EXAMPLE_AC("?cert=clientc.der",          .url.tcp.query.cert     = "clientc.der");
  ASSERT_UDDRP_TCP_EXAMPLE_AC("?privKey=xyzzy",             .url.tcp.query.priv_key = "xyzzy");
  ASSERT_UDDRP_TCP_EXAMPLE_AC("?encr=1",                    .url.tcp.query.encr = ucshAT_TLS_VERSION_1_0);
  ASSERT_UDDRP_TCP_EXAMPLE_AC("?encr=2",                    .url.tcp.query.encr = ucshAT_TLS_VERSION_1_1);
  ASSERT_UDDRP_TCP_EXAMPLE_AC("?encr=3",                    .url.tcp.query.encr = ucshAT_TLS_VERSION_1_2);
  ASSERT_UDDRP_TCP_EXAMPLE_AC("",                           .url.tcp.query.encr = 0);

  ASSERT_UDDRP_TCP_EXAMPLE_EC("",                           .url.tcp.query.encr = 0);
  ASSERT_UDDRP_TCP_EXAMPLE_IPV4("?local_port=42",           .url.tcp.query.local_port = 42);

  printf("AT+UDDRP: test_COMMAND_writeDefaultRemotePeerSingleTcpParam OK\n========================================\n");
}

void test_COMMAND_writeDefaultRemotePeerSingleTcpIpv6Param(void)
{
  #define ASSERT_UDDRP_TCP_EXAMPLE_IPV6(str, act, ipv6) \
    ASSERT_UDDRP_TCP1(ipv6, str, act, 4, .connect_scheme.external_connect = true, "[", "]");

  ASSERT_UDDRP_TCP_EXAMPLE_IPV6("?local_port=42",           .url.tcp.query.local_port = 42, "FE80:0000:0000:0000:0202:B3FF:FE1E:8329");
  ASSERT_UDDRP_TCP_EXAMPLE_IPV6("?local_port=42",           .url.tcp.query.local_port = 42, "FE80::0202:B3FF:FE1E:8329");
  ASSERT_UDDRP_TCP_EXAMPLE_IPV6("?local_port=42",           .url.tcp.query.local_port = 42, "FE80:23:1:0:0202:B3FF:FE1E:8329");
  ASSERT_UDDRP_TCP_EXAMPLE_IPV6("?local_port=42",           .url.tcp.query.local_port = 42, "FE80:db8:1:0:0202:B3FF:FE1E:8329");
  ASSERT_UDDRP_TCP_EXAMPLE_IPV6("?local_port=42",           .url.tcp.query.local_port = 42, "2001:0db8:85a3:0000:0000:8a2e:0370:7334");
  ASSERT_UDDRP_TCP_EXAMPLE_IPV6("?local_port=42",           .url.tcp.query.local_port = 42, "2001:db8::1");
  ASSERT_UDDRP_TCP_EXAMPLE_IPV6("?local_port=42",           .url.tcp.query.local_port = 42, "2001:DB8::1");
  ASSERT_UDDRP_TCP_EXAMPLE_IPV6("?local_port=42",           .url.tcp.query.local_port = 42, "2001:0db8::0001:0000");
  ASSERT_UDDRP_TCP_EXAMPLE_IPV6("?local_port=42",           .url.tcp.query.local_port = 42, "2001:db8::1:0");
  ASSERT_UDDRP_TCP_EXAMPLE_IPV6("?local_port=42",           .url.tcp.query.local_port = 42, "2001:db8::1:0:0:1");
  ASSERT_UDDRP_TCP_EXAMPLE_IPV6("?local_port=42",           .url.tcp.query.local_port = 42, "2001:db8:0:1:1:1:1:1");
  ASSERT_UDDRP_TCP_EXAMPLE_IPV6("?local_port=42",           .url.tcp.query.local_port = 42, "::1");
  ASSERT_UDDRP_TCP_EXAMPLE_IPV6("?local_port=42",           .url.tcp.query.local_port = 42, "::");
  ASSERT_UDDRP_TCP_EXAMPLE_IPV6("?local_port=42",           .url.tcp.query.local_port = 42, "::ffff:192.0.2.128");
  
  printf("AT+UDDRP: test_COMMAND_writeDefaultRemotePeerSingleTcpIpv6Param OK\n========================================\n");
}

void test_COMMAND_writeDefaultRemotePeerDualTcpParam(void)
{
  #define ASSERT_UDDRP_TCP2(hostname, str, act, connect_scheme_flag, connect_scheme_act, act2) \
  do { \
    ucxhPARSER_defaultRemotePeerParam_t cfg = { \
      act,  \
      act2, \
      connect_scheme_act, \
      .peer_id = 1, \
      .url.scheme = ucxhPARSER_PEER_URL_SCHEME_TCP, \
      .url.tcp.remote_port = 2000, \
      .url.tcp.domain = hostname, \
    }; \
    assert(ucxhCOMMAND_writeDefaultRemotePeer(&cfg)); \
    returnOk(); \
    assert(ucxhSERIAL_wasStringWritten("AT+UDDRP=1,\"tcp://" hostname ":2000/" str "\"," #connect_scheme_flag "\r\n")); \
  } while (0)

  #define ASSERT_UDDRP_TCP_EXAMPLE2_AC(str, act, act2) \
    ASSERT_UDDRP_TCP2(EXAMPLE_COM, str, act, 2, .connect_scheme.always_connected = true, act2);

  ASSERT_UDDRP_TCP_EXAMPLE2_AC("?local_port=42&flush_tx=1",  .url.tcp.query.local_port = 42, .url.tcp.query.flush_tx = true);
  ASSERT_UDDRP_TCP_EXAMPLE2_AC("?local_port=42&flush_tx=1",  .url.tcp.query.flush_tx = true, .url.tcp.query.local_port = 42);
  ASSERT_UDDRP_TCP_EXAMPLE2_AC("?keepAlive=20000+1500+150",  .url.tcp.query.keep_alive.idle_ms     = 20000, .url.tcp.query.keep_alive.interval_ms = 1500);
  ASSERT_UDDRP_TCP_EXAMPLE2_AC("?keepAlive=20000+10000+10",  .url.tcp.query.keep_alive.idle_ms     = 20000, .url.tcp.query.keep_alive.count       = 10);
  ASSERT_UDDRP_TCP_EXAMPLE2_AC("?keepAlive=4000+2000+100",   .url.tcp.query.keep_alive.interval_ms = 2000,  .url.tcp.query.keep_alive.count       = 100);
  ASSERT_UDDRP_TCP_EXAMPLE2_AC("?keepAlive=20000+1500+150",  .url.tcp.query.keep_alive.idle_ms     = 20000, .url.tcp.query.keep_alive.interval_ms = 1500);

  printf("AT+UDDRP: test_COMMAND_writeDefaultRemotePeerDualTcpParam OK\n========================================\n");
}

void test_COMMAND_writeDefaultRemotePeerTrippleTcpParam(void)
{
  #define ASSERT_UDDRP_TCP3(hostname, str, act, connect_scheme_flag, connect_scheme_act, act2, act3) \
  do { \
    ucxhPARSER_defaultRemotePeerParam_t cfg = { \
      act,  \
      act2, \
      act3, \
      connect_scheme_act, \
      .peer_id = 1, \
      .url.scheme = ucxhPARSER_PEER_URL_SCHEME_TCP, \
      .url.tcp.remote_port = 2000, \
      .url.tcp.domain = hostname, \
    }; \
    assert(ucxhCOMMAND_writeDefaultRemotePeer(&cfg)); \
    returnOk(); \
    assert(ucxhSERIAL_wasStringWritten("AT+UDDRP=1,\"tcp://" hostname ":2000/" str "\"," #connect_scheme_flag "\r\n")); \
  } while (0)

  #define ASSERT_UDDRP_TCP_EXAMPLE3_AC(str, act, act2, act3) \
    ASSERT_UDDRP_TCP3(EXAMPLE_COM, str, act, 2, .connect_scheme.always_connected = true, act2, act3);

  ASSERT_UDDRP_TCP_EXAMPLE3_AC("?keepAlive=20000+1500+100",
    .url.tcp.query.keep_alive.idle_ms     = 20000,
    .url.tcp.query.keep_alive.interval_ms = 1500,
    .url.tcp.query.keep_alive.count       = 100);
  ASSERT_UDDRP_TCP_EXAMPLE3_AC("?keepAlive=5000+1000+5",
    .url.tcp.query.keep_alive.idle_ms     = 5000,
    .url.tcp.query.keep_alive.interval_ms = 1000,
    .url.tcp.query.keep_alive.count       = 5);
  ASSERT_UDDRP_TCP_EXAMPLE3_AC("?ca=ca.crt&cert=client.crt&privKey=client.key",
    .url.tcp.query.ca       = "ca.crt",
    .url.tcp.query.cert     = "client.crt",
    .url.tcp.query.priv_key = "client.key");
  
  printf("AT+UDDRP: test_COMMAND_writeDefaultRemotePeerTrippleTcpParam OK\n========================================\n");
}


int main()
{
  bool result = true;
  const int  numSeconds = 2;
  
  result &= ucxhSERIAL_open();
  assert(result == true);

  test_COMMAND_connectPeer();                              ucxhPARSER_wait(numSeconds);
  test_COMMAND_writeDefaultRemotePeerSingleTcpParam();     ucxhPARSER_wait(numSeconds);
  test_COMMAND_writeDefaultRemotePeerSingleTcpIpv6Param(); ucxhPARSER_wait(numSeconds);
  test_COMMAND_writeDefaultRemotePeerDualTcpParam();       ucxhPARSER_wait(numSeconds);
  test_COMMAND_writeDefaultRemotePeerTrippleTcpParam();    ucxhPARSER_wait(numSeconds);

  printf("test_at_peer completed OK!\n");
  ucxhSERIAL_close();
  return 0;
}
