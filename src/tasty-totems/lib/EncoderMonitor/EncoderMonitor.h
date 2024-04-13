#ifndef TASTY_TOTEM_ENCODERMONITOR_H
#define TASTY_TOTEM_ENCODERMONITOR_H

#include "Arduino.h"

#define ENCODER_MULT 14
#define GEARING      100

// Conversion factors
#define MICROS_IN_SECOND 1000000
#define SECONDS_IN_MINUTE 60
#define MICROS_IN_MINUTE (MICROS_IN_SECOND * SECONDS_IN_MINUTE)

#define TIMING_CONSTANT (MICROS_IN_MINUTE / (ENCODER_MULT * GEARING))

enum class MotorDirection {
    FORWARD,
    BACKWARD
};

class EncoderMonitor {
public:
    static EncoderMonitor* instance;

    EncoderMonitor(uint8_t encoder_pin_1, uint8_t encoder_pin_2);
    void begin();

    void handle_encoder_c1();
    void handle_encoder_c2();

    static void IRAM_ATTR static_encoder_c1_handler();
    static void IRAM_ATTR static_encoder_c2_handler();

    unsigned long get_current_delta();
    MotorDirection get_current_direction();
    uint16_t get_current_rpm();

private:
    uint8_t _encoder_pin_1;
    uint8_t _encoder_pin_2;

    unsigned long _encoder_delta;
    // Test whether volatile is really needed here
    volatile unsigned long _c1_rise_time, _c2_rise_time;
    volatile MotorDirection _direction;
};

#endif // TASTY_TOTEM_ENCODERMONITOR_H