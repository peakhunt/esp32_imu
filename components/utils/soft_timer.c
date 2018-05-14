////////////////////////////////////////////////////////////////////////////////
//
// an old invention of hkim and open nature of the internet
//
////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include "soft_timer.h"

/**
 * initialize a timer manager
 *
 * @param timer timer manager context block
 * @param tick_rate desired tick rate
 * @param n_buckets number of buckets desired
 * @return 0 on success, -1 on failure
 */
int
soft_timer_init(SoftTimer* timer, int tick_rate)
{
  int i;

  timer->tick_rate           = tick_rate;
  timer->tick                =      0;

  for(i = 0; i < SOFT_TIMER_NUM_BUCKETS; i++)
  {
    INIT_LIST_HEAD(&timer->buckets[i]);
  }
  return 0;
}

/**
 * deinitialize a timer manager
 *
 * @param timer timer context block
 */
void
soft_timer_deinit(SoftTimer* timer)
{
}

/**
 * initialize a timer element before using it
 *
 * @param elem timer element to initialize
 */
void
soft_timer_init_elem(SoftTimerElem* elem)
{
  INIT_LIST_HEAD(&elem->next);
}

/**
 * start a stopped timer element by adding it to timer manager
 *
 * @param timer timer manager context block
 * @param elem new timer element to add to timer manager
 * @param expires desired timeout value in milliseconds
 */
void
soft_timer_add(SoftTimer* timer, SoftTimerElem* elem, int expires)
{
  int target;

  if(is_soft_timer_running(elem))
  {
    // XXX add crash code
    return;
  }

  INIT_LIST_HEAD(&elem->next);

  elem->tick     = timer->tick + get_soft_tick_from_milsec(timer, expires);
  target         = elem->tick % SOFT_TIMER_NUM_BUCKETS;

  list_add_tail(&elem->next, &timer->buckets[target]);
}

/**
 * stop a running timer element by deleting it from timer manager
 *
 * @param timer tmier manager context block
 * @param elem timer elemen to delete
 */
void
soft_timer_del(SoftTimer* timer, SoftTimerElem* elem)
{
  if(!is_soft_timer_running(elem))
  {
    return;
  }
  list_del_init(&elem->next);
}

static void
timer_tick(SoftTimer* timer)
{
  int               current;
  SoftTimerElem     *p, *n;
  struct list_head  timeout_list = LIST_HEAD_INIT(timeout_list);

  timer->tick++;

  current = timer->tick % SOFT_TIMER_NUM_BUCKETS;

  //
  // be careful with this code..
  // Here is the logic behind this
  // 1. once timer expires, it should be able to re-add the same timer again
  // 2. when a timer expires, it should be able to remove
  //    other timers including ones timed out inside the timeout handler
  //
  list_for_each_entry_safe(p, n, &timer->buckets[current], next)
  {
    if(p->tick == timer->tick)
    {
      list_del(&p->next);
      list_add_tail(&p->next, &timeout_list);
    }
  }

  while(!list_empty(&timeout_list))
  {
    p = list_first_entry(&timeout_list, SoftTimerElem, next);
    list_del_init(&p->next);
    p->cb(p);
  }
}

/**
 * drive a given timer manager
 * this routine should be called every tick rate as close as possible
 * On non-realtime systems,  some late timeout is just inevitable
 *
 * @param timer timer manager context block
 */
void
soft_timer_drive(SoftTimer* timer)
{
  timer_tick(timer);
}
