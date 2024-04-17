#include "PIDControl.h"

PIDControl::PIDControl(EncoderMonitor *encoder) {
    _encoder = encoder;
    _pk = PROPORTIONAL_K;
    _ik = INTEGRAL_K;
    _dk = DERIVATIVE_K;

    _target_rpm = 0;
}

void PIDControl::set_target_rpm(int target) {
    _target_rpm = target;
}

int PIDControl::get_measured_rpm() {
    return _encoder->get_current_rpm();
}

void PIDControl::set_pk(float pk) {
    _pk = pk;
}

void PIDControl::set_ik(float ik) {
    _ik = ik;
}

void PIDControl::set_dk(float dk) {
    _dk = dk;
}

float PIDControl::error_percent() {
    return (float) abs(get_error()) / (float) _target_rpm;
}

int PIDControl::get_error() {
    return _target_rpm - _encoder->get_current_rpm();
}

float PIDControl::get_response() {
    float delta_t = _get_delta_t();
    int error = get_error();

    // This is the only term that needs a persisted update
    _cummulative_error += error * delta_t;

    return get_proportial_response(error)
        + get_integral_response(error, delta_t)
        + get_derivative_response(error, delta_t);
}

float PIDControl::get_proportial_response() {
    return get_proportial_response(get_error());
}

float PIDControl::get_proportial_response(int error) {
    return _pk * error;
}


float PIDControl::get_integral_response() {
    return get_integral_response(get_error(), _get_delta_t());
}

float PIDControl::get_integral_response(int error, float delta_t) {
    return _ik * _cummulative_error;
}

float PIDControl::get_derivative_response() {
    return get_derivative_response(get_error(), _get_delta_t());
}

float PIDControl::get_derivative_response(int error, float delta_t) {
    return _dk * ((_last_pid_error - error)/delta_t);
}

float PIDControl::_get_delta_t() {
    unsigned long ts = micros();
    return (ts - _last_pid_check_micros)/(1.0 * MICROS_PER_SECOND);
}