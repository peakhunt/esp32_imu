#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

#include "sock_util.h"

void
sock_util_put_nonblock(int sd)
{
  const int            const_int_1 = 1;

  ioctl(sd, FIONBIO, (char*)&const_int_1);
}

void
sock_set_keepalive(int sd, int cnt, int idle, int interval)
{
  const int            const_int_1 = 1;

#ifdef SOL_TCP
  if(setsockopt(sd, SOL_SOCKET, SO_KEEPALIVE, &const_int_1, sizeof(const_int_1)) < 0)
  {
    ESP_LOGE("sock_uitil", "failed to enable keep alive");
    return;
  }

  setsockopt(sd, SOL_TCP, TCP_KEEPCNT,   &cnt, sizeof(cnt));
  setsockopt(sd, SOL_TCP, TCP_KEEPIDLE,  &idle, sizeof(idle));
  setsockopt(sd, SOL_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));
#else // mostly OSX
  setsockopt(sd, SOL_SOCKET, SO_KEEPALIVE, &const_int_1, sizeof(const_int_1));

  setsockopt(sd, IPPROTO_TCP, TCP_KEEPCNT, &cnt, sizeof(cnt));
  setsockopt(sd, IPPROTO_TCP, TCP_KEEPALIVE, &idle, sizeof(idle));
  setsockopt(sd, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));
#endif
}
