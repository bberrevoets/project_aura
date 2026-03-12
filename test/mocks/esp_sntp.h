#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SNTP_SYNC_STATUS_RESET = 0,
    SNTP_SYNC_STATUS_IN_PROGRESS = 1,
    SNTP_SYNC_STATUS_COMPLETED = 2
} sntp_sync_status_t;

void sntp_set_sync_status(sntp_sync_status_t sync_status);
sntp_sync_status_t sntp_get_sync_status(void);
bool esp_sntp_enabled(void);
void esp_sntp_stop(void);
void configTzTime(const char *tz, const char *server1, const char *server2, const char *server3);

#ifdef __cplusplus
}
#endif
