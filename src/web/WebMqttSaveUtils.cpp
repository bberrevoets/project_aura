// SPDX-FileCopyrightText: 2025-2026 Volodymyr Papush (21CNCStudio)
// SPDX-License-Identifier: GPL-3.0-or-later
// GPL-3.0-or-later: https://www.gnu.org/licenses/gpl-3.0.html
// Want to use this code in a commercial product while keeping modifications proprietary?
// Purchase a Commercial License: see COMMERCIAL_LICENSE_SUMMARY.md

#include "web/WebMqttSaveUtils.h"

#include <ctype.h>
#include <string.h>

#include "config/AppConfig.h"
#include "web/WebInputValidation.h"
#include "web/WebTextUtils.h"

namespace WebMqttSaveUtils {

namespace {

bool string_is_empty(const String &value) {
    return value.length() == 0;
}

String trim_copy(const String &value) {
    const char *raw = value.c_str();
    if (!raw) {
        return String();
    }

    const char *start = raw;
    while (*start != '\0' && isspace(static_cast<unsigned char>(*start))) {
        ++start;
    }

    const char *end = raw + strlen(raw);
    while (end > start && isspace(static_cast<unsigned char>(*(end - 1)))) {
        --end;
    }

    String out;
    while (start < end) {
        out += *start++;
    }
    return out;
}

bool ends_with_char(const String &value, char ch) {
    const size_t len = value.length();
    if (len == 0) {
        return false;
    }
    const char *raw = value.c_str();
    return raw && raw[len - 1] == ch;
}

bool contains_text(const String &value, const char *needle) {
    const char *raw = value.c_str();
    return raw && needle && strstr(raw, needle) != nullptr;
}

void remove_last_char(String &value) {
    if (value.length() == 0) {
        return;
    }
#ifdef UNIT_TEST
    value.erase(value.length() - 1);
#else
    value.remove(value.length() - 1);
#endif
}

String normalize_pem_text(const String &value) {
    String normalized;
    normalized.reserve(value.length());

    const char *raw = value.c_str();
    if (!raw) {
        return normalized;
    }

    for (size_t i = 0; raw[i] != '\0'; ++i) {
        if (raw[i] == '\r') {
            if (raw[i + 1] == '\n') {
                continue;
            }
            normalized += '\n';
        } else {
            normalized += raw[i];
        }
    }

    return trim_copy(normalized);
}

} // namespace

ParseResult parseSaveInput(const SaveInput &input, const CurrentCredentials &current_credentials) {
    ParseResult result{};

    SaveUpdate update{};
    update.host = input.host;
    update.user = input.user;
    update.pass = input.pass;
    update.device_name = input.device_name;
    update.base_topic = input.base_topic;
    update.ca_cert_pem = normalize_pem_text(input.ca_cert_pem);
    update.discovery = input.discovery;
    update.anonymous = input.anonymous;
    update.tls_enabled = input.tls_enabled;

    update.host = trim_copy(update.host);
    const String port_str = trim_copy(input.port);
    update.device_name = trim_copy(update.device_name);
    update.base_topic = trim_copy(update.base_topic);

    if (string_is_empty(update.host)) {
        result.status_code = 400;
        result.error_message = "Broker address required";
        return result;
    }

    if (string_is_empty(update.device_name)) {
        result.status_code = 400;
        result.error_message = "Device name required";
        return result;
    }

    if (string_is_empty(update.base_topic)) {
        result.status_code = 400;
        result.error_message = "Base topic required";
        return result;
    }

    if (WebTextUtils::hasControlChars(update.host) ||
        WebTextUtils::hasControlChars(update.user) ||
        WebTextUtils::hasControlChars(update.pass) ||
        WebTextUtils::hasControlChars(update.device_name) ||
        WebTextUtils::hasControlChars(update.base_topic)) {
        result.status_code = 400;
        result.error_message = "Fields contain unsupported control characters";
        return result;
    }

    if (WebTextUtils::mqttTopicHasWildcards(update.base_topic)) {
        result.status_code = 400;
        result.error_message = "Base topic must not include MQTT wildcards (+ or #)";
        return result;
    }

    if (update.ca_cert_pem.length() > Config::MQTT_CA_CERT_MAX_BYTES) {
        result.status_code = 400;
        result.error_message = "CA certificate is too large";
        return result;
    }

    if (update.tls_enabled) {
        if (string_is_empty(update.ca_cert_pem)) {
            result.status_code = 400;
            result.error_message = "CA certificate is required when TLS is enabled";
            return result;
        }
        if (!contains_text(update.ca_cert_pem, "-----BEGIN CERTIFICATE-----") ||
            !contains_text(update.ca_cert_pem, "-----END CERTIFICATE-----")) {
            result.status_code = 400;
            result.error_message = "CA certificate must be a PEM certificate";
            return result;
        }
    }

    const uint16_t default_port = update.tls_enabled ? Config::MQTT_TLS_DEFAULT_PORT
                                                     : Config::MQTT_DEFAULT_PORT;
    if (!WebInputValidation::parsePortOrDefault(port_str, default_port, update.port)) {
        result.status_code = 400;
        result.error_message = "Port must be in range 1-65535";
        return result;
    }

    if (!update.anonymous && (string_is_empty(update.user) || string_is_empty(update.pass))) {
        result.status_code = 400;
        result.error_message =
            "Username and password are required when anonymous mode is disabled";
        return result;
    }

    if (update.anonymous) {
        if (string_is_empty(update.user)) {
            update.user = current_credentials.user;
        }
        if (string_is_empty(update.pass)) {
            update.pass = current_credentials.pass;
        }
    }

    if (ends_with_char(update.base_topic, '/')) {
        remove_last_char(update.base_topic);
    }

    result.success = true;
    result.status_code = 200;
    result.update = update;
    return result;
}

} // namespace WebMqttSaveUtils
