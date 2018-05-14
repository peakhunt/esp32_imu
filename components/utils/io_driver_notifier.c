#include "io_driver_notifier.h"

static const char* TAG = "io_driver_notifier";

///////////////////////////////////////////////////////////////////////////////
//
// utilities
//
///////////////////////////////////////////////////////////////////////////////
static int
pipe_tcp_socket(void)
{
  return socket(AF_INET, SOCK_STREAM, 0);
}

static int
create_pipe(int filedes[2])
{
	struct sockaddr_in addr = { 0 };
	socklen_t addr_size = sizeof (addr);
	struct sockaddr_in adr2;
	socklen_t adr2_size = sizeof (adr2);
	int listener;
	int sock [2] = { -1, -1 };

	if ((listener = pipe_tcp_socket ()) == -1)
		return -1;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl (INADDR_LOOPBACK);
	addr.sin_port = 0;

	if (bind (listener, (struct sockaddr *)&addr, addr_size))
		goto fail;

	if (getsockname (listener, (struct sockaddr *)&addr, &addr_size))
		goto fail; 

	if (listen (listener, 1))
		goto fail;

	if ((sock [0] = pipe_tcp_socket ()) == -1)
		goto fail;

	if (connect (sock [0], (struct sockaddr *)&addr, addr_size))
		goto fail;

	if ((sock [1] = accept (listener, 0, 0)) == -1)
		goto fail;

	if (getpeername (sock [0], (struct sockaddr *)&addr, &addr_size))
		goto fail;

	if (getsockname (sock [1], (struct sockaddr *)&adr2, &adr2_size))
		goto fail;

	if (addr_size != adr2_size
			|| addr.sin_addr.s_addr != adr2.sin_addr.s_addr 
			|| addr.sin_port        != adr2.sin_port)
		goto fail;

	close(listener);

	filedes [0] = sock [0];
	filedes [1] = sock [1];
	return 0;

fail:
  ESP_LOGE(TAG, "failed to create pipe");
	close (listener);

	if (sock [0] != -1) close(sock [0]);
	if (sock [1] != -1) close(sock [1]);

	return -1;
}

///////////////////////////////////////////////////////////////////////////////
//
// watcher callback
//
///////////////////////////////////////////////////////////////////////////////
static void
__io_driver_notifier__watcher_callback(io_driver_watcher_t*  watcher, io_driver_event evt)
{
  uint8_t   cmd;

  io_driver_notifier_t* notifier = container_of(watcher, io_driver_notifier_t, watcher);

  if(recv(notifier->pipe[0], &cmd, 1, 0) != 1)
  {
    ESP_LOGE(TAG, "failed to recv. stop watching io event");
    io_driver_no_watch(notifier->driver, &notifier->watcher, IO_DRIVER_EVENT_RX);
    return;
  }

  notifier->cb(notifier, cmd);
}

///////////////////////////////////////////////////////////////////////////////
//
// public interfaces
//
///////////////////////////////////////////////////////////////////////////////
void
io_driver_notifier_init(io_driver_t* driver, io_driver_notifier_t* notifier)
{
  notifier->driver = driver;

  create_pipe(notifier->pipe);
  notifier->watcher.fd = notifier->pipe[0];
  notifier->watcher.callback = __io_driver_notifier__watcher_callback;

  io_driver_watch(driver, &notifier->watcher, IO_DRIVER_EVENT_RX);
}

void
io_driver_notifier_deinit(io_driver_notifier_t* notifier)
{
  io_driver_no_watch(notifier->driver, &notifier->watcher, IO_DRIVER_EVENT_RX);

  close(notifier->pipe[0]);
  close(notifier->pipe[1]);
}

void
io_driver_notifier_notify(io_driver_notifier_t* notifier, uint8_t cmd)
{
  if(send(notifier->pipe[1], &cmd, 1, 0) != 1)
  {
    // FIXME error handling
    ESP_LOGE(TAG, "failed to send");
  }
}
