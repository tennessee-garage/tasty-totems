#ifndef TASTY_TOTEM_PIDCONTROL_H
#define TASTY_TOTEM_PIDCONTROL_H

#include <Arduino.h>
#include "EncoderMonitor.h"

#define MILLIS_PER_SECOND 1000
#define MICROS_PER_SECOND (MILLIS_PER_SECOND * 1000)

// PID constants
#define PROPORTIONAL_K 0.2
#define INTEGRAL_K 0
#define DERIVATIVE_K (0.3/10000)

#define PID_DELTA_MICROS 500

class PIDControl {
public:
    PIDControl(EncoderMonitor *encoder);

    void set_pk(float pk);
    void set_ik(float ik);
    void set_dk(float dk);
    void set_target_rpm(int target);

    int get_measured_rpm();

    float error_percent();

    int get_error();
    float get_response();

    // One off accessors to these partial results
    float get_proportial_response();
    float get_integral_response();
    float get_derivative_response();

    // Accessors that pass error and delta_t to make sure they are
    // all aligned on the same value at the same time, for use in `get_response`
    float get_proportial_response(int error);
    float get_integral_response(int error, float detla_t);
    float get_derivative_response(int error, float detla_t);

private:
    EncoderMonitor *_encoder;

    float _pk, _ik, _dk;
    int _target_rpm;

    // We keep this one (vs adding PID_DELTA_MICROS to _last_pid_check_micros) to quickly check whether we
    // should take another measurement w/o an addition.
    unsigned long _next_pid_check_micros;
    // The timestamp of our last check
    unsigned long _last_pid_check_micros;

    // The last error value captured
    int _last_pid_error;

    // The delta between the current error value and the last error value
    float _pid_error_delta;

    // The cumulative error since startup
    float _cummulative_error;

    float PIDControl::_get_delta_t();
};

#endif
