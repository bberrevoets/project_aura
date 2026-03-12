#pragma once

#include "Arduino.h"

struct Bmp3xxTestState {
    bool ok = true;
    bool start_ok = true;
    bool pressure_valid = false;
    bool has_new_data = false;
    bool invalidate_called = false;
    float pressure = 0.0f;
    float temperature = 0.0f;
    uint32_t last_data_ms = 0;
};

class Bmp3xx {
public:
    enum class Variant : uint8_t {
        Unknown = 0,
        BMP388,
        BMP390
    };

    static Bmp3xxTestState &state() {
        static Bmp3xxTestState instance;
        return instance;
    }

    static Variant &variant_state() {
        static Variant instance = Variant::BMP390;
        return instance;
    }

    bool begin() { return true; }
    bool start() { return state().start_ok; }
    void poll() {}
    bool takeNewData(float &pressure_hpa, float &temperature_c) {
        if (!state().has_new_data) {
            return false;
        }
        pressure_hpa = state().pressure;
        temperature_c = state().temperature;
        state().has_new_data = false;
        state().pressure_valid = true;
        state().last_data_ms = millis();
        return true;
    }
    bool isOk() const { return state().ok; }
    bool isPressureValid() const { return state().pressure_valid; }
    uint32_t lastDataMs() const { return state().last_data_ms; }
    Variant variant() const { return variant_state(); }
    const char *variantLabel() const {
        switch (variant_state()) {
            case Variant::BMP388:
                return "BMP388";
            case Variant::BMP390:
                return "BMP390";
            default:
                return "BMP3xx";
        }
    }
    void invalidate() {
        state().pressure_valid = false;
        state().has_new_data = false;
        state().invalidate_called = true;
    }
};
