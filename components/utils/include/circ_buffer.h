#ifndef __CIRC_BUFFER_DEF_H__
#define __CIRC_BUFFER_DEF_H__

#include "common_def.h"

/**
 * circular buffer structure
 */
typedef struct
{
   uint8_t*    buffer;        /** buffer pointer to store data    */
   int         size;          /** size of buffer                  */
   int         data_size;     /** size of data in buffer          */
   int         begin;         /** buffer begin index              */
   int         end;           /** buffer end index                */
} circ_buffer_t;

extern int circ_buffer_init(circ_buffer_t* cb, int size);
extern void circ_buffer_deinit(circ_buffer_t* cb);
extern int circ_buffer_put(circ_buffer_t* cb, uint8_t* buf, int size);
extern int circ_buffer_get(circ_buffer_t* cb, uint8_t* buf, int size);
extern int circ_buffer_peek(circ_buffer_t* cb, uint8_t* buf, int size);
extern int circ_buffer_advance(circ_buffer_t* cb, int size);

/*
 * reset circular buffer
 *
 * @param cb   circular buffer
 */
static inline void 
circ_buffer_reset(circ_buffer_t* cb)
{
   cb->begin      = 0;
   cb->end        = 0;
   cb->data_size  = 0;
}

/*
 * get data size in circular buffer
 *
 * @param cb   circular buffer
 * @return data size
 */
static inline int
circ_buffer_get_data_size(circ_buffer_t* cb)
{
   return cb->data_size;
}

/*
 * get buffer size of circular buffer
 *
 * @param cb   circular buffer
 * @return buffer size
 */
static inline int
circ_buffer_get_size(circ_buffer_t* cb)
{
   return cb->size;
}

/*
 * check if circular buffer is full
 *
 * @param cb   circular buffer
 * @return TRUE when full, FALSE otherwise
 */
static inline bool
circ_buffer_is_full(circ_buffer_t* cb)
{
   if(cb->data_size == cb->size)
   {
      return TRUE;
   }
   return FALSE;
}

/*
 * check if circular buffer is empty
 *
 * @param cb   circular buffer
 * @return TRUE when empty, FALSE otherwise
 */
static inline bool
circ_buffer_is_empty(circ_buffer_t* cb)
{
   if(cb->data_size == 0)
   {
      return TRUE;
   }
   return FALSE;
}

#endif //!__CIRC_BUFFER_DEF_H__
