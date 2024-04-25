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

// Worked this out empirically; not sure if this can be derived from the above constants
#define ENCODER_STEPS 693

enum class MotorDirection {
    CLOCKWISE,
    COUNTER_CLOCKWISE
};

class EncoderMonitor {
public:
    static EncoderMonitor* instance;

    EncoderMonitor(uint8_t encoder_pin_1, uint8_t encoder_pin_2, uint16_t trim);
    void begin();

    void handle_encoder_c1();
    void handle_encoder_c2();

    static void IRAM_ATTR static_encoder_c1_handler();
    static void IRAM_ATTR static_encoder_c2_handler();

    unsigned long get_current_delta();
    MotorDirection get_current_direction();
    uint16_t get_current_rpm();

    uint16_t get_rise_counter();
    void clear_rise_counter();
    float rotation_progress();

private:
    uint8_t _encoder_pin_1;
    uint8_t _encoder_pin_2;
    uint16_t _trim;

    unsigned long _encoder_delta;
    // TODO: Test whether volatile is really needed here
    volatile unsigned long _c1_rise_time, _c2_rise_time;
    volatile MotorDirection _direction;
    volatile uint16_t _rise_counter;
};

#endif // TASTY_TOTEM_ENCODERMONITOR_H