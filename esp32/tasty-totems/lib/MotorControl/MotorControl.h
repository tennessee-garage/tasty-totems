/*
  MotorControl.h - Library for managing motor speed
*/
#ifndef MotorControl_h
#define MotorControl_h

#include "Arduino.h"

// Constants specific to the motor and gearbox used
#define DEFAULT_GEARING 100
#define DEFAULT_ENCODER_MULT 14

// Settings to manage PWM on the motor
#define DEFAULT_PWM_FREQ 80000
#define DEFAULT_PWM_CHANNEL 0
#define DEFAULT_PWM_RESOLUTION 13

// Conversion factors
#define MICROS_IN_SECOND 1000000
#define SECONDS_IN_MINUTE 60
#define MICROS_IN_MINUTE (MICROS_IN_SECOND * SECONDS_IN_MINUTE)

// PID constants
#define PROPORTIONAL_K 0.5

// How small the error has to be (0.0 to 1.0) before we declare we're up to speed and ready
#define MOTOR_READY_ERROR 0.1

#define PID_DELTA_MICROS 300

class MotorControl {
public:
    MotorControl(int motor_pin, unsigned long *encoder_delta);
    void setup();

    void start_motor(int target_rpm, float duty_cycle);
    void stop_motor();
    void update();
    void update_motor_speed() const;

    void set_gearing(int gearing);
    void set_encoder_multiplier(int mult);

    void set_pwm_channel(int channel);
    void set_pwm_freq(int freq);

    void set_pwm_duty_cycle(float duty_cycle);
    void set_pwm_duty_cycle(unsigned long duty_cycle);
    [[nodiscard]] unsigned long get_pwm_duty_cycle() const;

    void set_target_rpm(int rpm);
    [[nodiscard]] int get_target_rpm() const;
    [[nodiscard]] int get_current_rpm() const;

    float error_percent();
    bool is_ready();
private:
    int _pid_error();
    void _update_current_rpm();
    void _add_to_pwm_duty_cycle(long duty_cycle_delta);

    int _motor_pin;

    int _gearing;
    int _encoder_mult;
    unsigned long _timing_constant;
    unsigned long *_encoder_delta;

    int _pwm_freq;
    int _pwm_channel;
    int _pwm_resolution;

    // Duty cycle as an int between 0 and _pwm_resolution ** 2
    unsigned long _pwm_duty_cycle;
    //  _pwm_resolution ** 2
    unsigned long _max_duty_cycle;

    int _target_rpm;
    int _current_rpm;

    bool _is_motor_ready;

    unsigned long _next_pid_check_micros;
};

#endif
