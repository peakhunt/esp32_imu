#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "circ_buffer.h"

/*
 * initializes circular buffer
 * allocates a circular buffer of size "size"
 *
 * @param cb   circular buffer
 * @param size size of circular buffer
 * @return 0 on success, -1 on fail
 */
int
circ_buffer_init(circ_buffer_t* cb, int size)
{
  cb->buffer     = NULL;
  cb->size       = 0;
  cb->begin      = 0;
  cb->end        = 0;
  cb->data_size  = 0;

  cb->buffer = (uint8_t*)malloc(size);
  if(cb->buffer == NULL)
  {
    return -1;
  }

  cb->size       = size;
  return 0;
}

/*
 * de-initializes circular buffer
 * frees buffer used by circular buffer
 *
 * @param cb   circular buffer
 */
void
circ_buffer_deinit(circ_buffer_t* cb)
{
  if(cb->buffer != NULL)
  {
    free(cb->buffer);
    cb->buffer = NULL;
  }
}

/*
 * adds data to circular buffer
 * if the whole size data cannot be added to the circular buffer
 * error is returned
 *
 * @param cb   circular buffer
 * @param buf data buffer
 * @param size size of data
 * @return 0 on success, -1 on error
 */
int
circ_buffer_put(circ_buffer_t* cb, uint8_t* buf, int size)
{
  if(cb->data_size + size > cb->size)
  {
    return -1;
  }

  if(cb->end + size > cb->size)
  {
    int begin_len = cb->size - cb->end;

    memcpy(&cb->buffer[cb->end], buf, begin_len);
    memcpy(&cb->buffer[0], &buf[begin_len], size - begin_len);
  }
  else
  {
    memcpy(&cb->buffer[cb->end], buf, size);
  }

  cb->end = (cb->end  + size) % cb->size;
  cb->data_size += size;
  return 0;
}

/*
 * removes data from circular buffer
 * if the whole size data cannot be removed from the circular buffer
 * error is returned
 *
 * @param cb   circular buffer
 * @param buf buffer to copy data from circular buffer
 * @param size size of data requested
 * @return 0 on success, -1 on error
 */
int
circ_buffer_get(circ_buffer_t* cb, uint8_t* buf, int size)
{
  if(cb->data_size - size < 0)
  {
    return -1;
  }

  if(cb->begin + size <= cb->size)
  {
    memcpy(buf, &cb->buffer[cb->begin], size);
  }
  else
  {
    int begin_len = cb->size - cb->begin;

    memcpy(buf, &cb->buffer[cb->begin], begin_len);
    memcpy(&buf[begin_len], &cb->buffer[0], size - begin_len);
  }

  cb->begin = (cb->begin + size) % cb->size;
  cb->data_size -= size;
  return 0;
}

/*
 * removes data from circular but the data is not copied to
 * user buffer
 * if the whole size data cannot be copied from the circular buffer
 * error is returned
 *
 * @param cb   circular buffer
 * @param size size of data requested
 * @return 0 on success, -1 on error
 */
int
circ_buffer_advance(circ_buffer_t* cb, int size)
{
  if(cb->data_size - size < 0)
  {
    return -1;
  }

  cb->begin = (cb->begin + size) % cb->size;
  cb->data_size -= size;
  return 0;
}

/*
 * gets data from circular but the data still remains in circular buffer
 * if the whole size data cannot be copied from the circular buffer
 * error is returned
 *
 * @param cb circular buffer
 * @param buf user buffer to copy data to
 * @param size size of data requested
 * @return 0 on success, -1 on error
 */
int
circ_buffer_peek(circ_buffer_t* cb, uint8_t* buf, int size)
{
  if(cb->data_size - size < 0)
  {
    return -1;
  }

  if(cb->begin + size <= cb->size)
  {
    memcpy(buf, &cb->buffer[cb->begin], size);
  }
  else
  {
    int begin_len = cb->size - cb->begin;

    memcpy(buf, &cb->buffer[cb->begin], begin_len);
    memcpy(&buf[begin_len], &cb->buffer[0], size - begin_len);
  }
  return 0;
}
