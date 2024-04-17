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
    void set_constants(float pk, float ik, float dk);

    void set_target_rpm(int target);
    int get_target_rpm();

    int get_measured_rpm();

    float error_percent();

    int get_error();

    // Calculate and return the PID response for this point in time
    float calc_response();
    
    // Access calculated results of the response and each response term
    float get_response();
    float get_proportial_response();
    float get_integral_response();
    float get_derivative_response();

private:
    EncoderMonitor *_encoder;

    float _pk, _ik, _dk;
    int _target_rpm;

    float _last_response;
    float _last_proportional_response, _last_integral_response, _last_derivative_response;

    // The timestamp of our last check
    unsigned long _last_pid_check_micros;

    // The last error value captured
    int _last_error;

    // The cumulative error since startup
    float _cummulative_error;

    // Accessors that pass error and delta_t to make sure they are
    // all aligned on the same value at the same time, for use in `get_response`
    float _calc_proportial_response(int error);
    float _calc_integral_response(int error, float detla_t);
    float _calc_derivative_response(int error, float detla_t);

    float _get_delta_t();
};

#endif
