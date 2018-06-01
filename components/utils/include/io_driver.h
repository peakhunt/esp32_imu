//
//
// a very simple select based IO driver for memory/resource tight embedded systems
// -hkim-
//
//
#ifndef __IO_DRIVER_DEF_H__
#define __IO_DRIVER_DEF_H__

#include <stdio.h>
#include "common_def.h"
#include "generic_list.h"

typedef enum
{
  IO_DRIVER_EVENT_RX      = 0x01,     // rx
  IO_DRIVER_EVENT_TX      = 0x02,     // tx
  IO_DRIVER_EVENT_EX      = 0x04,     // error
} io_driver_event;

struct __io_driver_watcher;
typedef struct __io_driver_watcher io_driver_watcher_t;

typedef void (*io_driver_callback)(io_driver_watcher_t* watcher, io_driver_event event);

struct __io_driver_watcher
{
  int                   fd;
  uint8_t               event_listening;
  io_driver_callback    callback;
  struct list_head      le;
};


typedef struct 
{
  struct list_head      watchers;
} io_driver_t;

extern void io_driver_init(io_driver_t* driver);
extern void io_driver_run(io_driver_t* driver);
extern void io_driver_watcher_init(io_driver_watcher_t* watcher);
extern void io_driver_watch(io_driver_t* driver, io_driver_watcher_t* watcher, io_driver_event event);
extern void io_driver_no_watch(io_driver_t* driver, io_driver_watcher_t* watcher, io_driver_event event);

#endif /* !__IO_DRIVER_DEF_H__ */
