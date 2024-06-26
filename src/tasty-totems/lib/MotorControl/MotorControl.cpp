/*
  MotorControl.cpp - Library for managing motor speed
*/

#include "Arduino.h"
#include "MotorControl.h"

MotorControl::MotorControl(uint8_t motor_pin_1, uint8_t motor_pin_2, PIDControl *pid) {
    _motor_pin_1 = motor_pin_1;
    _motor_pin_2 = motor_pin_2;
    _pid = pid;

    // Establish our max value
    _pwm_duty_cycle = 0;
    _max_duty_cycle = (2 << (_pwm_resolution-1)) - 1;

    _is_motor_ready = false;

    _next_pid_check_micros = micros() + PID_DELTA_MICROS;
}

void MotorControl::setup() {
    pinMode(_motor_pin_1, OUTPUT);
 
    // Make sure the other motor pin is grouned
    pinMode(_motor_pin_2, OUTPUT);
    digitalWrite(_motor_pin_2, 0);

    // Configure PWM parameters needed to control the motor via the ledc library
    _configure_pwm();

    // attach the channel to the GPIO to be controlled
    ledcAttachPin(_motor_pin_1, _pwm_channel);
}

void MotorControl::_configure_pwm() {
    // configure the PWM channel for our motor via the LEDC methods
    ledcSetup(_pwm_channel, _pwm_freq, _pwm_resolution);
}

// With the PID controller, the caller shouldn't have to specify a starting duty cycle.
void MotorControl::start_motor(int target_rpm) {
    start_motor(target_rpm, 0.5);
}

void MotorControl::start_motor(int target_rpm, float duty_cycle) {
    set_pwm_duty_cycle(duty_cycle);
    _pid->set_target_rpm(target_rpm);

    update_motor_speed();
}

void MotorControl::stop_motor() {
    set_pwm_duty_cycle((unsigned long) 0);
    _pid->set_target_rpm(0);

    update_motor_speed();
}

bool MotorControl::is_ready() {
    // Exit early if we've already determined we're ready
    if (_is_motor_ready) {
        return true;
    }

    // Find the error percentage and compare it to our acceptable threshold
    if (_pid->error_percent() > MOTOR_READY_ERROR) {
        return false;
    } else {
        _is_motor_ready = true;
        return true;
    }
}

// PID adjust motor speed toward target RPM
void MotorControl::update() {
    unsigned long ts = micros();
    if (ts < _next_pid_check_micros)
        return;

    long p_delta = (long) _pid->calc_response();
    _add_to_pwm_duty_cycle(p_delta);

    update_motor_speed();

    _next_pid_check_micros = ts + PID_DELTA_MICROS;
}

void MotorControl::update_motor_speed() const {
    ledcWrite(_pwm_channel, int(_pwm_duty_cycle));
}

void MotorControl::set_pwm_channel(int channel) {
    _pwm_channel = channel;
}

void MotorControl::set_pwm_freq(int freq) {
    _pwm_freq = freq;

    _configure_pwm();
}

// Set the duty cycle as float between 0.0 and 1.0
void MotorControl::set_pwm_duty_cycle(float duty_cycle) {
    _pwm_duty_cycle = (unsigned long)(duty_cycle * float(_max_duty_cycle));
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
    _pid->set_target_rpm(rpm);
}

int MotorControl::get_target_rpm() const {
    return _pid->get_target_rpm();
}
