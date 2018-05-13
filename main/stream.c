#include <errno.h>
#include "stream.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

static const char* TAG = "stream";

static void
stream_handle_tx_event(stream_t* stream)
{
  uint8_t   buffer[128];
  int       data_size,
            ret,
            len;

  if(circ_buffer_is_empty(&stream->tx_buf))
  {
    ESP_LOGI(TAG, "disabling TX event. circ buffer empty now");
    io_driver_no_watch(stream->driver, &stream->watcher, IO_DRIVER_EVENT_TX);
    return;
  }

  // XXX
  // looping would be more efficient but
  // on the other hand, this has better time sharing feature
  //
  data_size = circ_buffer_get_data_size(&stream->tx_buf);
  len = data_size < 128 ? data_size : 128;
  circ_buffer_peek(&stream->tx_buf, buffer, len);

  ret = write(stream->watcher.fd, buffer, len);
  if(ret <= 0)
  {
    // definitely stream got into a trouble
    io_driver_no_watch(stream->driver, &stream->watcher, IO_DRIVER_EVENT_TX);

    ESP_LOGI(TAG, "XXXXXXXX this should not happen");
    CRASH();
    return;
  }

  circ_buffer_advance(&stream->tx_buf, ret);

  if(circ_buffer_is_empty(&stream->tx_buf))
  {
    io_driver_no_watch(stream->driver, &stream->watcher, IO_DRIVER_EVENT_TX);
  }

  return;
}

static void
stream_watcher_callback(io_driver_watcher_t* watcher, io_driver_event evt)
{
  int             ret;
  stream_t*       stream = container_of(watcher, stream_t, watcher);
  stream_event_t  sevt = 0;

  //log_write("watch event: %d\n", evt);
  switch(evt)
  {
  case IO_DRIVER_EVENT_RX:
    ret = read(watcher->fd, stream->rx_buf, stream->rx_buf_size);
    if(ret > 0)
    {
      sevt = stream_event_rx;
      stream->rx_data_len = ret;
    }
    else if(ret == 0)
    {
      sevt = stream_event_eof;
    }
    else
    {
      sevt = stream_event_err;
    }
    break;

  case IO_DRIVER_EVENT_TX:
    stream_handle_tx_event(stream);
    return;

  case IO_DRIVER_EVENT_EX:
    sevt = stream_event_err;
    break;
  }

  stream->cb(stream, sevt);
}


void
stream_init_with_fd(io_driver_t* driver, stream_t* stream, int fd, uint8_t* rx_buf, int rx_buf_size, int tx_buf_size)
{
  // XXX return value check
  circ_buffer_init(&stream->tx_buf, tx_buf_size);

  stream->rx_buf      = rx_buf;
  stream->rx_buf_size = rx_buf_size;

  io_driver_watcher_init(&stream->watcher);

  stream->driver = driver;
  stream->watcher.fd = fd;
  stream->watcher.callback = stream_watcher_callback;

  io_driver_watch(stream->driver, &stream->watcher, IO_DRIVER_EVENT_RX);
  io_driver_watch(stream->driver, &stream->watcher, IO_DRIVER_EVENT_EX);
}

void
stream_deinit(stream_t* stream)
{
  circ_buffer_deinit(&stream->tx_buf);
  stream_stop(stream);
  close(stream->watcher.fd);
}

void
stream_start(stream_t* stream)
{
}

void
stream_stop(stream_t* stream)
{
  io_driver_no_watch(stream->driver, &stream->watcher, IO_DRIVER_EVENT_RX);
  io_driver_no_watch(stream->driver, &stream->watcher, IO_DRIVER_EVENT_TX);
  io_driver_no_watch(stream->driver, &stream->watcher, IO_DRIVER_EVENT_EX);
}

bool
stream_write(stream_t* stream, uint8_t* data, int len)
{
  int ret;

  if(circ_buffer_is_empty(&stream->tx_buf) == FALSE)
  {
    // something got queued up due to socket buffer full
    // tx watcher is alreadt started in this case
    return circ_buffer_put(&stream->tx_buf, data, len) == 0;
  }
  else
  {
    ret = write(stream->watcher.fd, data, len);
    if(ret == len)
    {
      return TRUE;
    }

    if(ret < 0)
    {
      if(!(errno == EWOULDBLOCK || errno == EAGAIN))
      {
        ESP_LOGI(TAG, "stream tx error other than EWOULDBLOCK");
        return FALSE;
      }
      ret = 0;
    }

    // start tx watcher and queue remaining data
    if(circ_buffer_put(&stream->tx_buf, &data[ret], len - ret) == FALSE)
    {
      ESP_LOGI(TAG, "can't TX.. circ buffer overflow");
      return FALSE;
    }

    ESP_LOGI(TAG, "enabling TX event");
    io_driver_watch(stream->driver, &stream->watcher, IO_DRIVER_EVENT_TX);
    return TRUE;
  }
}
