#include <string.h>

#include "io_driver.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

#include "esp_log.h"

static const char* TAG = "io_driver";

typedef struct
{
  fd_set      rset;
  fd_set      wset;
  fd_set      eset;
  bool        rset_empty;
  bool        wset_empty;
  bool        eset_empty;
  int         maxfd;
} select_call_arg_t;

///////////////////////////////////////////////////////////////////////////////
//
// private utilities
//
///////////////////////////////////////////////////////////////////////////////
static void
io_driver_preselect(io_driver_t* driver, select_call_arg_t* s)
{
  io_driver_watcher_t*   watcher;

  FD_ZERO(&s->rset);
  FD_ZERO(&s->wset);
  FD_ZERO(&s->eset);

  s->rset_empty = TRUE;
  s->wset_empty = TRUE;
  s->eset_empty = TRUE;

  s->maxfd = 0;

  list_for_each_entry(watcher, &driver->watchers, le)
  {
    if(watcher->event_listening & IO_DRIVER_EVENT_RX)
    {
      FD_SET(watcher->fd, &s->rset);
      s->maxfd = MAX(watcher->fd, s->maxfd);
      s->rset_empty = FALSE;
    }

    if(watcher->event_listening & IO_DRIVER_EVENT_TX)
    {
      FD_SET(watcher->fd, &s->wset);
      s->maxfd = MAX(watcher->fd, s->maxfd);
      s->wset_empty = FALSE;
    }

    if(watcher->event_listening & IO_DRIVER_EVENT_EX)
    {
      FD_SET(watcher->fd, &s->eset);
      s->maxfd = MAX(watcher->fd, s->maxfd);
      s->eset_empty = FALSE;
    }
  }
}

static void
io_driver_postselect(io_driver_t* driver, select_call_arg_t* s)
{
  //
  // I know what you think. But even on single threaded env, manipulating elements on temporary run_list
  // is the only way to ensure list structure consistency.
  //
  // by employing temporary run_list, we can solve the following requirements.
  // 1) callbacks should be able to add new watcher (by calling XXX_watch)
  //    ==> new watcher is added to driver->watchers immediately and scheduled at next loop.
  //
  // 2) callbacks should be able to delete itself (by calling XXX_no_watch)
  //    ==> the watcher is deleted from driver->watchers .
  //
  // 3) callbakcs should be able to delete other watchers (by calling XXX_no_watch)
  //    ==> other watchers are deleted from run_list and never gets scheduled.
  //
  // I understand this is confusing even to me LoL. Good Luck to us!
  //
  // the only remaining issue here is calling sequence.
  // Currently, the sequence is RX-> TX -> EX
  // I believe it should be ususally OK except some exceptionally rare cases
  // -hkim-
  //
  io_driver_watcher_t*    watcher;
  struct list_head        run_list;

  INIT_LIST_HEAD(&run_list);

  list_splice_init(&driver->watchers, &run_list);

  while(!list_empty(&run_list))
  {
    watcher = list_first_entry(&run_list, io_driver_watcher_t, le);

    list_del_init(&watcher->le);
    list_add_tail(&watcher->le, &driver->watchers);

    if((watcher->event_listening & IO_DRIVER_EVENT_RX) && FD_ISSET(watcher->fd, &s->rset))
    {
      watcher->callback(watcher, IO_DRIVER_EVENT_RX);
    }

    if((watcher->event_listening & IO_DRIVER_EVENT_TX) && FD_ISSET(watcher->fd, &s->wset))
    {
      watcher->callback(watcher, IO_DRIVER_EVENT_TX);
    }

    if((watcher->event_listening & IO_DRIVER_EVENT_EX) && FD_ISSET(watcher->fd, &s->eset))
    {
      watcher->callback(watcher, IO_DRIVER_EVENT_EX);
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
//
// public interfaces
//
///////////////////////////////////////////////////////////////////////////////
void
io_driver_init(io_driver_t* driver)
{
  INIT_LIST_HEAD(&driver->watchers);
}

void
io_driver_run(io_driver_t* driver)
{
  select_call_arg_t       s;
  struct timeval          tv = 
  {
    .tv_sec   = 1,
    .tv_usec  = 0,
  };
  int                     ret;

  io_driver_preselect(driver, &s);

  ret = lwip_select(s.maxfd + 1, 
                    s.rset_empty ? NULL : &s.rset,
                    s.wset_empty ? NULL : &s.wset,
                    s.eset_empty ? NULL : &s.eset,
                    &tv);

  if(ret < 0)
  {
    ESP_LOGE(TAG, "select returned error: %d", ret);
    return;
  }

  if(ret == 0)
  {
    return;
  }

  io_driver_postselect(driver, &s);
}

void
io_driver_watcher_init(io_driver_watcher_t* watcher)
{
  INIT_LIST_HEAD(&watcher->le);

  watcher->fd = -1;
  watcher->event_listening = 0;
}

void
io_driver_watch(io_driver_t* driver, io_driver_watcher_t* watcher, io_driver_event event)
{
  uint8_t   old_set = watcher->event_listening;

  watcher->event_listening |= event;

  if(old_set == 0 && watcher->event_listening != 0)
  {
    list_add_tail(&watcher->le, &driver->watchers);
  }
}

void
io_driver_no_watch(io_driver_t* driver, io_driver_watcher_t* watcher, io_driver_event event)
{
  uint8_t   old_set = watcher->event_listening;

  watcher->event_listening &= ~event;

  if(old_set != 0 && watcher->event_listening == 0)
  {
    list_del_init(&watcher->le);
  }
}
