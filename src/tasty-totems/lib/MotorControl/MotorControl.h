/*
  MotorControl.h - Library for managing motor speed
*/
#ifndef MotorControl_h
#define MotorControl_h

#include <Arduino.h>
#include "EncoderMonitor.h"

#define MILLIS_PER_SECOND 1000
#define MICROS_PER_SECOND (MILLIS_PER_SECOND * 1000)

// Settings to manage PWM on the motor.  The relationship between the frequency & resolution is:
//   freq = APB_CLK / (PWM_MAX * Prescalar) == APB_CLK / (2**resolution * Prescalar)
// For a APB_CLK of 80 Mhz for the esp32 and a default prescalar of 1, it becomes:
//   freq = 80_000_000 / 2**resolution
#define DEFAULT_PWM_FREQ 100
#define DEFAULT_PWM_RESOLUTION 12
#define DEFAULT_PWM_CHANNEL 0

// PID constants
#define PROPORTIONAL_K 0.2
#define DERIVATIVE_K (0.3/10000)

// How small the error has to be (0.0 to 1.0) before we declare we're up to speed and ready
#define MOTOR_READY_ERROR 0.1

#define PID_DELTA_MICROS 500

class MotorControl {
public:
    MotorControl(uint8_t motor_pin_1, uint8_t motor_pin_2, EncoderMonitor *encoder);
    void setup();

    void start_motor(int target_rpm, float duty_cycle);
    void stop_motor();
    void update();
    void update_motor_speed() const;

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

    float get_integral_error();
    float get_derivative_error();

    int _pid_error();
private:

    void _update_current_rpm();
    void _add_to_pwm_duty_cycle(long duty_cycle_delta);
    void _configure_pwm();

    int _motor_pin_1;
    int _motor_pin_2;
    EncoderMonitor *_encoder;

    int _pwm_freq = DEFAULT_PWM_FREQ;
    int _pwm_channel = DEFAULT_PWM_CHANNEL;
    int _pwm_resolution = DEFAULT_PWM_RESOLUTION;

    // Duty cycle as an int between 0 and _pwm_resolution ** 2
    unsigned long _pwm_duty_cycle;
    //  _pwm_resolution ** 2
    unsigned long _max_duty_cycle;

    int _target_rpm;

    bool _is_motor_ready;

    // We keep this one (vs adding PID_DELTA_MICROS to _last_pid_check_micros) to quickly check whether we
    // should take another measurement w/o an addition.
    unsigned long _next_pid_check_micros;
    // The timestamp of our last check
    unsigned long _last_pid_check_micros;

    int _last_pid_error;
    float _pid_error_delta;

    float _cummulative_error;
};

#endif
