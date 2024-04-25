#include "tasty-totems.h"
#include "MotorControl.h"
#include "LedStrip.h"
#include "EncoderMonitor.h"
#include "PIDControl.h"
#include "RotationMonitor.h"
#include "DrumMontior.h"

// How long to pause before starting the strobe effect
#define START_PAUSE_MILLIS 2000

#define TRACK_START_IDX  1
#define TRACK_END_IDX   31

// An offset to adjust the strobe start
#define TRIM_STEPS 10

EncoderMonitor *ENCODER;
PIDControl *PID;
MotorControl *MOTOR;
LedStrip *STRIP;
RotationMonitor *ROTATION;
DrumMonitor *DRUM;

void setup() {
    Serial.begin(115200);
    Serial.println("Setup begin");

    Serial.print("Initalizing encoder ... ");
    ENCODER = new EncoderMonitor(ENCODER_C1_PIN, ENCODER_C2_PIN, TRIM_STEPS);
    ENCODER->begin();
    Serial.println("done");

    Serial.print("Initalizing rotation monitor ... ");
    ROTATION = new RotationMonitor(IR_DETECT_PIN, ENCODER);
    ROTATION->begin();
    Serial.println("done");

    DRUM = new DrumMonitor(ENCODER);

    Serial.print("Initalizing PID controller ... ");
    PID = new PIDControl(ENCODER);
    PID->set_constants(PROPORTIONAL_K, INTEGRAL_K, DERIVATIVE_K);
    Serial.println("done");

    Serial.print("Initalizing strip ... ");
    STRIP = new LedStrip(LED_STRIP_PIN, NUM_STRIP_PIXELS, DRUM);
    STRIP->set_color(255, 255, 255);
    STRIP->set_strobe_micros(STROBE_MICROS);
    STRIP->setup();
    Serial.println("done");

    Serial.print("Initalizing motor control ... ");
    MOTOR = new MotorControl(MOTOR_PIN_1, MOTOR_PIN_2, PID);
    MOTOR->set_pwm_channel(PWM_CHANNEL);
    MOTOR->setup();
    Serial.println("done");

    Serial.print("Starting motor ... ");
    MOTOR->start_motor(TARGET_RPM);
    Serial.println("done");
}

unsigned long pause_until = millis() + START_PAUSE_MILLIS;

// At the beginning we have two dependent conditions to handle.  First, we
// need to wait for the motor to spin up, so we delay for a bit to wait for that.
// Then, once its spinning, we need to wait for the current rotation to end, so that
// we start a the first frame, of the top row of animation.
bool in_delay_start = true;
bool in_wait_for_next_rotation = true;

bool strip_started = false;
void handle_strip() {
    if (in_delay_start) {
        // We are in the delay start while the current millis is less than the pause_until time
        if (millis() < pause_until) {
            return;
        }

        in_delay_start = false;
        ROTATION->reset_rotation_compete_flag();
    }

    // Send a sync to the strip on each rotation
    if (ROTATION->is_rotation_complete()) {
        ROTATION->reset_rotation_compete_flag();
        DRUM->increment_rotation();

        if (in_wait_for_next_rotation) {
            in_wait_for_next_rotation = false;
            DRUM->reset();
            STRIP->start_light_tracking(TRACK_START_IDX, TRACK_END_IDX);
        }
    }

    STRIP->update();
}

void loop() {
    MOTOR->update();
    handle_strip();
}
