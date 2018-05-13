#ifndef __STREAM_DEF_H__
#define __STREAM_DEF_H__

#include "common_def.h"
#include "circ_buffer.h"
#include "io_driver.h"

struct __stream;
typedef struct __stream stream_t;

typedef enum
{
  stream_event_rx,        // just received data
  stream_event_eof,       // eof detect
  stream_event_err,       // error catched
  stream_event_tx,        // tx buffer available
} stream_event_t;

typedef void (*stream_callback)(stream_t* stream, stream_event_t evt);

struct __stream
{
  io_driver_watcher_t watcher;
  io_driver_t*        driver;
  uint8_t*            rx_buf;
  int                 rx_buf_size;
  int                 rx_data_len;
  stream_callback     cb;
  circ_buffer_t       tx_buf;
};

extern void stream_init_with_fd(io_driver_t* driver, stream_t* stream, int fd, uint8_t* rx_buf, int rx_buf_size, int tx_buf_size);
extern void stream_deinit(stream_t* stream);
extern void stream_start(stream_t* stream);
extern void stream_stop(stream_t* stream);
extern bool stream_write(stream_t* stream, uint8_t* data, int len);

#endif /* !__STREAM_DEF_H__ */
