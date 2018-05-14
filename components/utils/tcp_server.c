#include <sys/types.h>
#include <sys/socket.h>
#include "tcp_server.h"
#include "list.h"

///////////////////////////////////////////////////////////////////////////////
//
// watcher callback
//
///////////////////////////////////////////////////////////////////////////////
static void
__tcp_server_watcher_callback(io_driver_watcher_t*  watcher, io_driver_event evt)
{
  tcp_server_t*   server = container_of(watcher, tcp_server_t, watcher);

  server->rx_event(server);
}

///////////////////////////////////////////////////////////////////////////////
//
// public interfaces
//
///////////////////////////////////////////////////////////////////////////////
void
tcp_server_init(io_driver_t* driver, tcp_server_t* server, int sd, tcp_server_rx_event cb)
{
  server->sd            = sd;
  server->is_listening  = FALSE;
  server->rx_event      = cb;

  server->driver        = driver;

  server->watcher.fd = sd;
  server->watcher.callback = __tcp_server_watcher_callback;

  io_driver_watch(server->driver, &server->watcher, IO_DRIVER_EVENT_RX);
}

void
tcp_server_deinit(tcp_server_t* server)
{
  tcp_server_stop(server);

  if(server->sd != -1)
  {
    close(server->sd);
    server->sd = -1;
  }
}

void
tcp_server_start(tcp_server_t* server)
{
  if(server->is_listening)
  {
    return;
  }

  server->is_listening = TRUE;
}

void
tcp_server_stop(tcp_server_t* server)
{
  if(!server->is_listening)
  {
    return;
  }

  server->is_listening = FALSE;

  io_driver_no_watch(server->driver, &server->watcher, IO_DRIVER_EVENT_RX);
  io_driver_no_watch(server->driver, &server->watcher, IO_DRIVER_EVENT_TX);
  io_driver_no_watch(server->driver, &server->watcher, IO_DRIVER_EVENT_EX);
}

void
tcp_server_get_port_name(tcp_server_t* server, char name[MAX_ADDRESS_STRING_LEN])
{
  server->get_bound_addr(server, name);
}
