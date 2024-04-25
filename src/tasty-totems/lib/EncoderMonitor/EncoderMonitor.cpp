#include "EncoderMonitor.h"

EncoderMonitor* EncoderMonitor::instance;

EncoderMonitor::EncoderMonitor(uint8_t encoder_pin_1, uint8_t encoder_pin_2, uint16_t trim) {
    _encoder_pin_1 = encoder_pin_1;
    _encoder_pin_2 = encoder_pin_2;
    _rise_counter = 0;
    _trim = trim;

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
        _direction = MotorDirection::CLOCKWISE;
        _rise_counter++;
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
        _direction = MotorDirection::COUNTER_CLOCKWISE;
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
        //Serial.print(">encoderDelta:");
        //Serial.println(_encoder_delta);
        return (uint16_t) (TIMING_CONSTANT / _encoder_delta);
    }
}

uint16_t EncoderMonitor::get_rise_counter() {
    return _rise_counter;
}

void EncoderMonitor::clear_rise_counter() {
    _rise_counter = 0;
}

float EncoderMonitor::rotation_progress() {
    uint16_t adjusted_counter = _rise_counter-_trim;

    // If our adjustment is less, than zero, wrap around the other direction
    if (adjusted_counter < 0) {
        adjusted_counter = ENCODER_STEPS - adjusted_counter;
    }

    return (1.0*(_rise_counter-_trim))/ENCODER_STEPS;
}