#include "SntpMock.h"

#include <cstring>

namespace {

sntp_sync_status_t g_status = SNTP_SYNC_STATUS_RESET;
bool g_enabled = false;
bool g_config_called = false;
char g_last_tz[32] = {0};

} // namespace

namespace SntpMock {

void reset() {
    g_status = SNTP_SYNC_STATUS_RESET;
    g_enabled = false;
    g_config_called = false;
    g_last_tz[0] = '\0';
}

void setSyncStatus(sntp_sync_status_t status) {
    g_status = status;
}

bool isEnabled() {
    return g_enabled;
}

bool wasConfigCalled() {
    return g_config_called;
}

const char *lastTimezone() {
    return g_last_tz;
}

} // namespace SntpMock

extern "C" {

void sntp_set_sync_status(sntp_sync_status_t sync_status) {
    g_status = sync_status;
}

sntp_sync_status_t sntp_get_sync_status(void) {
    return g_status;
}

bool esp_sntp_enabled(void) {
    return g_enabled;
}

void esp_sntp_stop(void) {
    g_enabled = false;
}

void configTzTime(const char *tz, const char *, const char *, const char *) {
    g_enabled = true;
    g_config_called = true;
    if (!tz) {
        g_last_tz[0] = '\0';
        return;
    }
    std::strncpy(g_last_tz, tz, sizeof(g_last_tz) - 1);
    g_last_tz[sizeof(g_last_tz) - 1] = '\0';
}

} // extern "C"
