/*
  MotorControl.cpp - Library for managing motor speed
*/

#include "Arduino.h"
#include "MotorControl.h"

MotorControl::MotorControl(int motor_pin, unsigned long *encoder_delta) {
    _motor_pin = motor_pin;

    _encoder_delta = encoder_delta;

    _gearing = DEFAULT_GEARING;
    _encoder_mult = DEFAULT_ENCODER_MULT;
    _pwm_freq = DEFAULT_PWM_FREQ;
    _pwm_channel = DEFAULT_PWM_CHANNEL;
    _pwm_resolution = DEFAULT_PWM_RESOLUTION;

    _timing_constant = 1.0;
    _pwm_duty_cycle = 0;
    _max_duty_cycle = (_pwm_resolution << 2) - 1;

    _target_rpm = 0;
    _current_rpm = 0;

    _is_motor_ready = false;
    _next_pid_check_micros = micros() + PID_DELTA_MICROS;
}

void MotorControl::setup() {
    pinMode(_motor_pin, OUTPUT);

    // configure the PWM channel for our motor via the LEDC methods
    ledcSetup(_pwm_channel, _pwm_freq, _pwm_resolution);

    // attach the channel to the GPIO to be controlled
    ledcAttachPin(_motor_pin, _pwm_channel);

    // Precalculate multiplier for the timing we find between encoder pulses, to determine RPM
    _timing_constant = (MICROS_IN_MINUTE / (_encoder_mult * _gearing));
    // Establish our max value
    _max_duty_cycle = (_pwm_resolution << 2) - 1;
}

void MotorControl::start_motor(int target_rpm, float duty_cycle) {
    set_pwm_duty_cycle(duty_cycle);
    _target_rpm = target_rpm;

    update_motor_speed();
}

void MotorControl::stop_motor() {
    set_pwm_duty_cycle((unsigned long) 0);
    _target_rpm = 0;

    update_motor_speed();
}

bool MotorControl::is_ready() {
    // Exit early if we've already determined we're ready
    if (_is_motor_ready) {
        return true;
    }

    // Find the error percentage and compare it to our acceptable threshold
    if (error_percent() > MOTOR_READY_ERROR) {
        return false;
    } else {
        _is_motor_ready = true;
        return true;
    }
}

float MotorControl::error_percent() {
    int error;

    // Get the absolute error
    error = _pid_error();
    if (error < 0) {
        error *= -1;
    }

    return (float) error / (float) _target_rpm;
}

int MotorControl::_pid_error() {
    // Get the current RPM based on the latest encoder timing
    _update_current_rpm();

    return _target_rpm - _current_rpm;
}

void MotorControl::_update_current_rpm() {
    if (*_encoder_delta == 0) {
        _current_rpm = 0;
    } else {
        _current_rpm = (int) (_timing_constant / *_encoder_delta);
    }
}

// PID adjust motor speed toward target RPM
void MotorControl::update() {
    if (micros() < _next_pid_check_micros)
        return;

    _next_pid_check_micros = micros() + PID_DELTA_MICROS;

    // Get the error between the current and desired RPM
    long p_delta = PROPORTIONAL_K * _pid_error();

    _add_to_pwm_duty_cycle(p_delta);
}

void MotorControl::update_motor_speed() const {
    ledcWrite(_pwm_channel, int(_pwm_duty_cycle));
}

void MotorControl::set_gearing(int gearing) {
    _gearing = gearing;
}

void MotorControl::set_encoder_multiplier(int mult) {
    _encoder_mult = mult;
}

void MotorControl::set_pwm_channel(int channel) {
    _pwm_channel = channel;
}

void MotorControl::set_pwm_freq(int freq) {
    _pwm_freq = freq;
}

// Set the duty cycle as float between 0.0 and 1.0
void MotorControl::set_pwm_duty_cycle(float duty_cycle) {
    _pwm_duty_cycle = (unsigned long)(duty_cycle * float(2 << (_pwm_resolution-1)));
}

// Note that the unsigned long type forces a lower bound of zero
void MotorControl::set_pwm_duty_cycle(unsigned long duty_cycle) {
    if (duty_cycle > _max_duty_cycle) {
        duty_cycle = _max_duty_cycle;
    }

    _pwm_duty_cycle = duty_cycle;
}

void MotorControl::_add_to_pwm_duty_cycle(long duty_cycle_delta) {
    set_pwm_duty_cycle(duty_cycle_delta + _pwm_duty_cycle);
}

unsigned long MotorControl::get_pwm_duty_cycle() const {
    return _pwm_duty_cycle;
}

void MotorControl::set_target_rpm(int rpm) {
    _target_rpm = rpm;
}

int MotorControl::get_target_rpm() const {
    return _target_rpm;
}

int MotorControl::get_current_rpm() const {
    return _current_rpm;
}