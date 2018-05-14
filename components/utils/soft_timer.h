////////////////////////////////////////////////////////////////////////////////
//
// soft timer for deterministic single threaded timer management
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __SOFT_TIMER_DEF_H__
#define __SOFT_TIMER_DEF_H__

#include "list.h"

#define SOFT_TIMER_NUM_BUCKETS      8

typedef struct _soft_timer_elem SoftTimerElem;

/**
 * timer callback function
 */
typedef void (*timer_cb)(SoftTimerElem*);

/**
 * timer element representing one timer
 */
struct _soft_timer_elem
{
  struct list_head  next;       /** a list head for next timer element in the bucket  */
  timer_cb          cb;         /** timeout callback                                  */
  unsigned int      tick;       /** absolute timeout tick count                       */
  void*             priv;       /** private argument for timeout callback             */
};

/**
 * a context block for timer manager
 */
typedef struct _timer
{
  int                  tick_rate;                                  /** tick rate 1 means a tick per 1ms      */
  unsigned int         tick;                                       /** current tick                          */
  struct list_head     buckets[SOFT_TIMER_NUM_BUCKETS];            /** bucket array                          */
} SoftTimer;

extern int soft_timer_init(SoftTimer* timer, int tick_rate);
extern void soft_timer_deinit(SoftTimer* timer);
extern void soft_timer_init_elem(SoftTimerElem* elem);
extern void soft_timer_add(SoftTimer* timer, SoftTimerElem* elem, int expires);
extern void soft_timer_del(SoftTimer* timer, SoftTimerElem* elem);
extern void soft_timer_drive(SoftTimer* timer);

/**
 * check if a given timer element is currently running
 *
 * @param elem timer element to check with
 * @return 0 if timer elemnt is not running, 1 if running
 */
static inline int
is_soft_timer_running(SoftTimerElem* elem)
{
  if(elem->next.next == &elem->next && elem->next.prev == &elem->next)
  {
    return 0;
  }
  return 1;
}

/**
 * calculate timer tick count from given milli second
 *
 * @param timer timer manager context block
 * @param milsec desired millisecond
 * @return the closes tick count for a given tick rate
 */
static inline unsigned int
get_soft_tick_from_milsec(SoftTimer* timer, int milsec)
{
  unsigned int   tick,
                 mod;

  tick = milsec / timer->tick_rate;
  mod  = milsec % timer->tick_rate;

  if(mod != 0)
  {
    tick += 1;
  }
  return tick;
}

#endif //!__SOFT_TIMER_DEF_H__
