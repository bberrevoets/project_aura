#pragma once

// Project override for esp-lib-utils / ESP32_Display_Panel logging.
// Keep raw panel/touch spam out of serial and rely on our own aggregated
// diagnostics/recovery logs instead.

#define ESP_UTILS_CONF_LOG_LEVEL (ESP_UTILS_LOG_LEVEL_NONE)

#define ESP_UTILS_CONF_FILE_VERSION_MAJOR 1
#define ESP_UTILS_CONF_FILE_VERSION_MINOR 2
#define ESP_UTILS_CONF_FILE_VERSION_PATCH 0
