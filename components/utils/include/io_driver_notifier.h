////////////////////////////////////////////////////////////////////////////////
//
// with non POSIX based RTOS,  it's quite hard to multiplex sockets and other
// freertos events,
// which causes hell lot of uglyness in integrating select based event loop.
//
// In this kind of situations, I've been using two approaches.
//
// One is to motify LwIP stack so that with minimal resource and performance usage,
// we can notify select based event loop
// using a special IOCTL.
//
// The other is using a tcp socket as a pipe.
//
// This implementation is about the latter since I don't wanna rely on modifying
// LwIP stack maintained by ESP32 community.
//
// -hkim-
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __IO_DRIVER_NOTIFIER_DEF_H__
#define __IO_DRIVER_NOTIFIER_DEF_H__

#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "common_def.h"
#include "io_driver.h"

struct __io_driver_notifier_t;
typedef struct __io_driver_notifier_t io_driver_notifier_t;

typedef void (*io_driver_notifier_callback)(io_driver_notifier_t* notifier, uint8_t cmd);

struct __io_driver_notifier_t
{
  io_driver_watcher_t           watcher;
  io_driver_t*                  driver;
  int                           pipe[2];      // 0 is for reading, 1 is for writing
  io_driver_notifier_callback   cb;
};

extern void io_driver_notifier_init(io_driver_t* driver, io_driver_notifier_t* notifier);
extern void io_driver_notifier_deinit(io_driver_notifier_t* notifier);
extern void io_driver_notifier_notify(io_driver_notifier_t* notifier, uint8_t cmd);

#endif /* !__IO_DRIVER_NOTIFIER_DEF_H__ */
