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
#define NUM_STRIP_PIXELS 35

// Rotation control
#define IR_DETECT_PIN  21

// How many frames fit around the drum circumference
#define FRAMES_PER_ROTATION 16
#define ROTATIONS_PER_DRUM   8

// PID constants
#define PROPORTIONAL_K 0.2
#define INTEGRAL_K 0.0
#define DERIVATIVE_K (0.3/10000)

#endif