#pragma once

#include "esp_sntp.h"

namespace SntpMock {

void reset();
void setSyncStatus(sntp_sync_status_t status);
bool isEnabled();
bool wasConfigCalled();
const char *lastTimezone();

} // namespace SntpMock
