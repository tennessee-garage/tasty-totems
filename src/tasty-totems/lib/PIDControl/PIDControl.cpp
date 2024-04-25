#include "PIDControl.h"

PIDControl::PIDControl(EncoderMonitor *encoder) {
    _encoder = encoder;
    _pk = DEFAULT_PROPORTIONAL_K;
    _ik = DEFAULT_INTEGRAL_K;
    _dk = DEFAULT_DERIVATIVE_K;

    _target_rpm = 0;
    _last_pid_check_micros = 0;
    _cummulative_error = 0;
}

void PIDControl::set_target_rpm(int target) {
    _target_rpm = target;
}

int PIDControl::get_target_rpm() {
    return _target_rpm;
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

void PIDControl::set_constants(float pk, float ik, float dk) {
    _pk = pk;
    _ik = ik;
    _dk = dk;
}

float PIDControl::error_percent() {
    return (float) abs(get_error()) / (float) _target_rpm;
}

int PIDControl::get_error() {
    _last_rpm = _encoder->get_current_rpm();
    return _target_rpm - _last_rpm;
}

float PIDControl::get_response() {
    return _last_response;
}

float PIDControl::calc_response() {
    float delta_t = _get_delta_t();
    int error = get_error();

    // This is the only term that needs a persisted update
    _cummulative_error += error * delta_t;

    _last_response = (
        _calc_proportial_response(error) +
        _calc_integral_response(error, delta_t) +
        _calc_derivative_response(error, delta_t)
    );

    _last_pid_check_micros = micros();
    _last_error = error;

    return _last_response;
}

int PIDControl::get_rpm() {
    return _last_rpm;
}

float PIDControl::get_proportial_response() {
    return _last_proportional_response;
}

float PIDControl::_calc_proportial_response(int error) {
    _last_proportional_response = _pk * error;
    return _last_proportional_response;
}

float PIDControl::get_integral_response() {
    return _last_integral_response;
}

float PIDControl::_calc_integral_response(int error, float delta_t) {
    _last_integral_response = _ik * _cummulative_error;
    return _last_integral_response;
}

float PIDControl::get_derivative_response() {
    return _last_derivative_response;
}

float PIDControl::_calc_derivative_response(int error, float delta_t) {
    _last_derivative_response = _dk * ((_last_error - error)/delta_t);
    return _last_derivative_response;
}

float PIDControl::_get_delta_t() {
    unsigned long ts = micros();
    return (ts - _last_pid_check_micros)/(1.0 * MICROS_PER_SECOND);
}