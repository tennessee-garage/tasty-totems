#include "RotationMonitor.h"

RotationMonitor* RotationMonitor::instance;

RotationMonitor::RotationMonitor(uint8_t ir_pin, EncoderMonitor *encoder) {
    _ir_pin = ir_pin;
    _encoder = encoder;
    _last_rise_counter = 0;
    reset_rotation_compete_flag();

    pinMode(_ir_pin, INPUT_PULLUP);
}

void RotationMonitor::begin() {
    attachInterrupt(digitalPinToInterrupt(_ir_pin), static_ir_handler, FALLING);
    instance = this;
}

void IRAM_ATTR RotationMonitor::static_ir_handler() {

    // This ensures we do nothing until we've actually called "begin" above
    if (instance != nullptr) {
        instance->handle_ir();
    }
}

void RotationMonitor::handle_ir() {
    // Ignore high frequency bouncing as the detector is uncovered
    // I couldn't catch this on the scope, but this was detecting a fall at exactly the width
    // of the pulse generated when the tab covers and uncovers the IR detector.  The rational is 
    // that there are sometimes high frequency high/low bouncers and if the pin is already high
    // by the time we handle the interrupt, its probably not real.  This seems to work in practice.
    if (digitalRead(_ir_pin) == HIGH)
      return;

    _last_rise_counter = _encoder->get_rise_counter();
    _encoder->clear_rise_counter();
    unsigned long tmp_micros = micros();
    _rotation_duration_micros = tmp_micros - _rotation_start_micros;
    _rotation_start_micros = tmp_micros;
    _rotation_complete_flag = true;
}

bool RotationMonitor::is_rotation_complete() {
    return _rotation_complete_flag;
}

void RotationMonitor::reset_rotation_compete_flag() {
    _rotation_complete_flag = false;
}

unsigned long RotationMonitor::last_rotation_micros() {
    return _rotation_duration_micros;
}