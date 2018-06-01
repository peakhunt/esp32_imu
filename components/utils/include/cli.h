#ifndef __CLI_DEF_H__
#define __CLI_DEF_H__

#include "common_def.h"
#include "generic_list.h"
#include "io_driver.h"

#define CLI_COMMAND_MAX_ARGS          32
#define CLI_RX_BUFFER_LENGTH          252
#define CLI_MAX_COMMAND_LEN           252

#define CLI_EOL                           "\r\n"

typedef struct __cli_intf cli_intf_t;

struct __cli_intf
{
  uint8_t       cmd_buffer_ndx;
  char          cmd_buffer[CLI_MAX_COMMAND_LEN];

  void          (*put_tx_data)(cli_intf_t* intf, const char* data, int len);
  void          (*print_status)(cli_intf_t* intf);

  struct list_head      le;
};

typedef void (*cli_command_handler)(cli_intf_t* intf, int argc, const char** argv);
typedef struct
{
  const char*         command;
  const char*         help_str;
  cli_command_handler handler;
} cli_command_t;

extern void cli_init(cli_command_t* cmds, int num_cmds, int port);
extern void cli_handle_rx(cli_intf_t* intf, uint8_t* data, int len);
extern void cli_intf_register(cli_intf_t* intf);
extern void cli_intf_unregister(cli_intf_t* intf);

extern void cli_printf(cli_intf_t* intf, const char* fmt, ...);

extern io_driver_t* cli_io_driver(void);

#endif /* !__CLI_DEF_H__ */
