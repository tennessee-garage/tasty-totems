#ifndef TASTY_TOTEM_MAIN_H
#define TASTY_TOTEM_MAIN_H

#include "Arduino.h"

// Motor & encoder constants
#define ENCODER_C1_PIN 15
#define ENCODER_C2_PIN  2
#define MOTOR_PIN_1    18
#define MOTOR_PIN_2    19
#define PWM_CHANNEL     0

// Led strip constants
#define LED_STRIP_PIN     5
#define NUM_STRIP_PIXELS 68

// Rotation control
#define IR_DETECT_PIN  21

// How many frames fit around the drum circumference
#define FRAMES_PER_ROTATION 16
#define ROTATIONS_PER_DRUM   8

#define TARGET_RPM            75
#define TARGET_ROTATIONS_PER_SECOND ((1.0*TARGET_RPM)/60.0)
#define SECONDS_PER_FRAME    (TARGET_ROTATIONS_PER_SECOND/FRAMES_PER_ROTATION)
#define MICROS_PER_FRAME     (SECONDS_PER_FRAME * 1000000)
#define STOBE_TIME_PERCENT   0.05
#define STROBE_MICROS        (MICROS_PER_FRAME * STOBE_TIME_PERCENT)

// PID constants
#define PROPORTIONAL_K 0.2
#define INTEGRAL_K 0.0
#define DERIVATIVE_K (0.3/10000)

#endif