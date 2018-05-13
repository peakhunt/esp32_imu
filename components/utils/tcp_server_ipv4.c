#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include "list.h"
#include "tcp_server_ipv4.h"
#include "sock_util.h"

static const char* TAG = "tcp_ipv4_server";

///////////////////////////////////////////////////////////////////////////////
//
// rx event handler on server socket
//
///////////////////////////////////////////////////////////////////////////////
static void
tcp_server_ipv4_rx_event(tcp_server_t* self)
{
  struct sockaddr_in    from;
  socklen_t             from_len;
  int                   newsd;

  from_len = sizeof(struct sockaddr_in);
  memset(&from, 0, from_len);

  newsd = accept(self->sd, (struct sockaddr*)&from, &from_len);
  if(newsd < 0)
  {
    ESP_LOGE(TAG, "accept() failed: %d", errno);
    return;
  }

  self->conn_cb(self, newsd, (struct sockaddr*)&from);
}

///////////////////////////////////////////////////////////////////////////////
//
// bound address info for debugging
//
///////////////////////////////////////////////////////////////////////////////
static void
tcp_server_ipv4_get_bound_addr(tcp_server_t* self, char string[MAX_ADDRESS_STRING_LEN])
{
  snprintf(string, MAX_ADDRESS_STRING_LEN - 1, "ipv4_tcp:%d", ntohs(self->ipv4_addr.sin_port));
}

///////////////////////////////////////////////////////////////////////////////
//
// utilities
//
///////////////////////////////////////////////////////////////////////////////
static int
__init_tcp_server_ipv4(io_driver_t* driver, tcp_server_t* server, struct sockaddr_in* addr, int backlog)
{
  int         sd;
  const int   on = 1;

  server->server_type   = tcp_server_type_ipv4;
  memcpy(&server->ipv4_addr, addr, sizeof(struct sockaddr_in));

  server->sd = -1;

  sd = socket(AF_INET, SOCK_STREAM, 0);
  if(sd < 0)
  {
    ESP_LOGE(TAG, "socket() failed: %d", errno);
    return -1;
  }

  sock_util_put_nonblock(sd);

  setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  if(bind(sd, (struct sockaddr*)&server->ipv4_addr, sizeof(struct sockaddr_in)) != 0)
  {
    ESP_LOGE(TAG, "bind() failed: %d", errno);
    close(sd);
    return -1;
  }

  if(listen(sd, backlog) != 0)
  {
    ESP_LOGE(TAG, "listen() failed: %d", errno);
    close(sd);
    return -1;
  }

  tcp_server_init(driver, server, sd, tcp_server_ipv4_rx_event);
  server->get_bound_addr    = tcp_server_ipv4_get_bound_addr;

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// public interfaces
//
///////////////////////////////////////////////////////////////////////////////
/**
 * initialize IPV4 tcp server
 *
 * @param server      server structure to initialize
 * @param port        port to use
 * @return 0 on success, -1 on failure
 */
int
tcp_server_ipv4_init(io_driver_t* driver, tcp_server_t* server, int port, int backlog)
{
  struct sockaddr_in    my_addr;

  memset(&my_addr, 0, sizeof(struct sockaddr_in));

  my_addr.sin_family      = AF_INET;
  my_addr.sin_addr.s_addr = INADDR_ANY;
  my_addr.sin_port        = htons(port);

  return __init_tcp_server_ipv4(driver, server, &my_addr, backlog);
}

int
tcp_server_ipv4_init_with_addr(io_driver_t* driver, tcp_server_t* server, struct sockaddr_in* addr, int backlog)
{
  return __init_tcp_server_ipv4(driver, server, addr, backlog);
}

int
tcp_server_ipv4_get_local_port(tcp_server_t* server)
{
  struct sockaddr_in    sin;
  socklen_t             len = sizeof(sin);

  if(server->sd < 0)
  {
    return -1;
  }

  getsockname(server->sd, (struct sockaddr*)&sin, &len);
  return ntohs(sin.sin_port);
}
