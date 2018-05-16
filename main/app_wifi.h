#ifndef __APP_WIFI_DEF_H__
#define __APP_WIFI_DEF_H__

#include "common_def.h"
#include "tcpip_adapter.h"

extern void app_wifi_init(void);
extern void app_wifi_get_config(tcpip_adapter_ip_info_t* info, char ssid[32], bool* is_configured);

#endif /* !__APP_WIFI_DEF_H__ */
