#ifndef __SOCK_UTIL_DEF_H__
#define __SOCK_UTIL_DEF_H__

extern void sock_util_put_nonblock(int sd);
extern void sock_set_keepalive(int sd, int cnt, int idle, int interval);

#endif /* !__SOCK_UTIL_DEF_H__ */
