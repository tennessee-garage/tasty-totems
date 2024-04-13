#include "EncoderMonitor.h"

EncoderMonitor* EncoderMonitor::instance;

EncoderMonitor::EncoderMonitor(uint8_t encoder_pin_1, uint8_t encoder_pin_2) {
    _encoder_pin_1 = encoder_pin_1;
    _encoder_pin_2 = encoder_pin_2;

    pinMode(_encoder_pin_1, INPUT);
    pinMode(_encoder_pin_2, INPUT);
}

void EncoderMonitor::begin() {
    attachInterrupt(digitalPinToInterrupt(_encoder_pin_1), static_encoder_c1_handler, RISING);
    attachInterrupt(digitalPinToInterrupt(_encoder_pin_2), static_encoder_c2_handler, RISING);
    instance = this;
}

void IRAM_ATTR EncoderMonitor::static_encoder_c1_handler() {
    // This ensures we do nothing until we've actually called "begin" above
    if (instance != nullptr) {
        instance->handle_encoder_c1();
    }
}

void IRAM_ATTR EncoderMonitor::static_encoder_c2_handler() {
    // This ensures we do nothing until we've actually called "begin" above
    if (instance != nullptr) {
        instance->handle_encoder_c2();
    }
}

// Track the first encoder signal, to compare its offset to the second encoder signal
void EncoderMonitor::handle_encoder_c1() {
    if (digitalRead(_encoder_pin_2) == HIGH) {
        // Pin 2 is alreaedy high while we (c1) are rising, c1 is lagging.  Calculate the delta and store
        _encoder_delta = micros() - _c2_rise_time;
        _direction = MotorDirection::FORWARD;
    } else {
        // We're ahead.  Store the start time
        _c1_rise_time = micros();
    }
}

// Track the second encoder signal, to compare its offset to the first encoder signal
void EncoderMonitor::handle_encoder_c2() {
    if (digitalRead(_encoder_pin_1) == HIGH) {
        // Pin 1 is alreaedy high while we (c2) are rising, c1 is lagging.  Calculate the delta and store
        _encoder_delta = micros() - _c1_rise_time;
        _direction = MotorDirection::BACKWARD;
    } else {
        // We're ahead.  Store the start time
        _c2_rise_time = micros();
    }
}

unsigned long EncoderMonitor::get_current_delta() {
    return _encoder_delta;
}

MotorDirection EncoderMonitor::get_current_direction() {
    return _direction;
}

uint16_t EncoderMonitor::get_current_rpm() {
    if (_encoder_delta == 0) {
        return 0;
    } else {
        return (uint16_t) (TIMING_CONSTANT / _encoder_delta);
    }
}