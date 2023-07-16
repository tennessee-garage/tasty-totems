#include <Arduino.h>

#include <Adafruit_NeoPixel.h>
#include "MotorControl.h"
#include "LedStrip.h"

// How long to pause before starting the strobe effect
#define START_PAUSE_MILLIS 2000

#define TRACK_START_IDX  2
#define TRACK_END_IDX   32

// Motor constants
#define GEARING       100
#define ENCODER_MULT   14
#define ENCODER_C1_PIN 23
#define ENCODER_C2_PIN 22
#define MOTOR_PIN      18
#define PWM_CHANNEL     0
#define TARGET_RPM    100

// Led strip constants
#define LED_STRIP_PIN     5
#define NUM_STRIP_PIXELS 35
#define STROBE_MICROS  1000

// Rotation control
#define IR_DETECT_PIN  21

// How many frames fit around the drum circumference
#define FRAMES_PER_ROTATION 16
#define ROTATIONS_PER_DRUM   8

// Trim the time per frame in micros to get properly synced
unsigned long trim_micros = 20000;

// Track the IR detector that looks for a full rotation of the drum
unsigned long rotation_start_micros = 0;
unsigned long rotation_duration_micros = 0;
unsigned long tmp_micros = 0;
bool is_rotation_complete = false;

void IRAM_ATTR rotation_reset_point() {
    // Ignore high frequency bouncing as the detector is uncovered
    // I couldn't catch this on the scope, but this was detecting a fall at exactly the width
    // of the pulse generated when the tab covers and uncovers the IR detector.  The rational is 
    // that there are sometimes high frequency high/low bouncers and if the pin is already high
    // by the time we handle the interrupt, its probably not real.  This seems to work in practice.
    if (digitalRead(IR_DETECT_PIN) == HIGH)
      return;

    tmp_micros = micros();
    rotation_duration_micros = tmp_micros - rotation_start_micros;
    rotation_start_micros = tmp_micros;
    is_rotation_complete = true;
}

// These variables are all used by the interrupt method and can't make use of
// an instance of the MotorControl class
volatile static unsigned long c1_rise_time, c2_rise_time;
static unsigned long encoder_delta;
volatile static int direction;

// Track the first encoder signal, to compare its offset to the second encoder signal
void IRAM_ATTR handle_encoder_c1() {
    if (digitalRead(ENCODER_C2_PIN) == HIGH) {
        // We're lagging.  Calculate the delta and store
        encoder_delta = micros() - c2_rise_time;
        direction = 1;
    } else {
        // We're ahead.  Store the start time
        c1_rise_time = micros();
    }
}

// Track the second encoder signal, to compare its offset to the first encoder signal
void IRAM_ATTR handle_encoder_c2() {
    if (digitalRead(ENCODER_C1_PIN) == HIGH) {
        // We're lagging.  Calculate the delta and store
        encoder_delta = micros() - c1_rise_time;
        direction = 0;
    } else {
        // We're ahead.  Store the start time
        c2_rise_time = micros();
    }
}

MotorControl motor(MOTOR_PIN, &encoder_delta);
LedStrip strip(LED_STRIP_PIN, NUM_STRIP_PIXELS);

void setup() {
    Serial.begin(115200);
    Serial.println("Setup begin");

    pinMode(ENCODER_C1_PIN, INPUT);
    pinMode(ENCODER_C2_PIN, INPUT);
    pinMode(IR_DETECT_PIN, INPUT_PULLUP);

    Serial.println("-- Encoder pins set");

    attachInterrupt(digitalPinToInterrupt(ENCODER_C1_PIN), handle_encoder_c1, RISING);
    attachInterrupt(digitalPinToInterrupt(ENCODER_C2_PIN), handle_encoder_c2, RISING);

    Serial.println("-- Interrupts registered");

    strip.set_color(200, 0, 0);
    strip.set_frames_per_rotation(FRAMES_PER_ROTATION);
    strip.set_rotations_per_drum(ROTATIONS_PER_DRUM);
    strip.set_strobe_micros(STROBE_MICROS);
    strip.setup();

    Serial.println("-- Strip setup");
    
    motor.set_encoder_multiplier(ENCODER_MULT);
    motor.set_gearing(GEARING);
    motor.set_pwm_channel(PWM_CHANNEL);
    motor.setup();

    Serial.println("-- Motor setup");

    attachInterrupt(digitalPinToInterrupt (IR_DETECT_PIN), rotation_reset_point, FALLING);

    Serial.println("-- IR setup");

    motor.start_motor(TARGET_RPM, 0.8);

    Serial.println("-- Motor started");
    Serial.print("-- Letting motor come up to speed ");
}

unsigned long pause_until = millis() + START_PAUSE_MILLIS;
bool strip_started = false;
void handle_strip() {
    // Skip strip updates if the motor isn't up to speed
    if (!strip_started) {
      if (millis() > pause_until) {
        strip_started = true;
        strip.start_light_tracking(TRACK_START_IDX, TRACK_END_IDX);
      }
      
      return;
    }

    // Send a sync to the strip on each rotation
    if (is_rotation_complete) {
        is_rotation_complete = false;
        Serial.print(":sync: rotation_duration_micros=");
        Serial.println(rotation_duration_micros);
        strip.sync(rotation_duration_micros, trim_micros);
    }

    strip.update();
}

unsigned long next = 0;
unsigned long wait_period = 500;
void loop() {
  /*
  if (next < millis()) {
    Serial.print(": error_percent=");
    Serial.print(motor.error_percent());
    Serial.println();
    next = millis() + wait_period;
  }
  */

  if (Serial.available() > 0) {
    // read the incoming byte:
    trim_micros = Serial.parseInt();
  }

  motor.update();
  handle_strip();
}
