#ifndef __TCP_SERVER_IPV4_DEF_H__
#define __TCP_SERVER_IPV4_DEF_H__

#include "common_def.h"
#include "tcp_server.h"
#include "io_driver.h"

extern int tcp_server_ipv4_init(io_driver_t* driver, tcp_server_t* server, int port, int backlog);
extern int tcp_server_ipv4_init_with_addr(io_driver_t* driver, tcp_server_t* server, struct sockaddr_in* addr, int backlog);
extern int tcp_server_ipv4_get_local_port(tcp_server_t* server);

#endif /* !__TCP_SERVER_IPV4_DEF_H__ */
