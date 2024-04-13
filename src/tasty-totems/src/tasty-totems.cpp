#include "tasty-totems.h"
#include "MotorControl.h"
#include "LedStrip.h"
#include "EncoderMonitor.h"
#include "RotationMonitor.h"

// How long to pause before starting the strobe effect
#define START_PAUSE_MILLIS 2000

#define TRACK_START_IDX  2
#define TRACK_END_IDX   32

#define TARGET_RPM    100
#define STROBE_MICROS 1000

// Trim the time per frame in micros to get properly synced
unsigned long trim_micros = 20000;

EncoderMonitor *ENCODER;
MotorControl *MOTOR;
LedStrip *STRIP;
RotationMonitor *ROTATION;

void setup() {
    Serial.begin(115200);
    Serial.println("Setup begin");

    STRIP = new LedStrip(LED_STRIP_PIN, NUM_STRIP_PIXELS);
    STRIP->set_color(200, 0, 0);
    STRIP->set_frames_per_rotation(FRAMES_PER_ROTATION);
    STRIP->set_rotations_per_drum(ROTATIONS_PER_DRUM);
    STRIP->set_strobe_micros(STROBE_MICROS);
    STRIP->setup();

    Serial.println("-- Strip setup");
    
    ROTATION = new RotationMonitor(IR_DETECT_PIN);
    ROTATION->begin();

    Serial.println("-- IR setup");

    ENCODER = new EncoderMonitor(ENCODER_C1_PIN, ENCODER_C2_PIN);
    ENCODER->begin();

    MOTOR = new MotorControl(MOTOR_PIN_1, MOTOR_PIN_2, ENCODER);
    MOTOR->set_pwm_channel(PWM_CHANNEL);
    MOTOR->setup();

    MOTOR->start_motor(TARGET_RPM, 0.8);

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
            STRIP->start_light_tracking(TRACK_START_IDX, TRACK_END_IDX);
        }
        
        return;
    }

    // Send a sync to the strip on each rotation
    if (ROTATION->is_rotation_complete()) {
        ROTATION->reset_rotation_compete_flag();
        Serial.print(":sync: rotation_duration_micros=");
        Serial.println(ROTATION->last_rotation_micros());
        STRIP->sync(ROTATION->last_rotation_micros(), trim_micros);
    }

    STRIP->update();
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

    MOTOR->update();
    handle_strip();
}
