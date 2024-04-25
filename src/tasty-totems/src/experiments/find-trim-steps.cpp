#include "tasty-totems.h"

#include "EncoderMonitor.h"
#include "RotationMonitor.h"
#include "MotorControl.h"
#include "PIDControl.h"

EncoderMonitor *ENCODER;
PIDControl *PID;
MotorControl *MOTOR;
RotationMonitor *ROTATION;

void setup() {
    Serial.begin(115200);
    Serial.println("Setup begin");

    Serial.print("Initalizing encoder ... ");
    ENCODER = new EncoderMonitor(ENCODER_C1_PIN, ENCODER_C2_PIN);
    ENCODER->begin();
    Serial.println("done");

    Serial.print("Initalizing rotation monitor ... ");
    ROTATION = new RotationMonitor(IR_DETECT_PIN, ENCODER);
    ROTATION->begin();
    Serial.println("done");

    Serial.print("Initalizing PID controller ... ");
    PID = new PIDControl(ENCODER);
    PID->set_constants(1, 0, 0);
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

uint16_t last_counter = 0;
void loop() {
    // Stop once we've cleared one rotation
    if (ROTATION->is_rotation_complete()) {
         MOTOR->stop_motor();
    }

    uint16_t counter = ENCODER->get_rise_counter();
    if (counter != last_counter) {
        Serial.print("== Rise counter: ");
        Serial.println(counter);
        last_counter = counter;
    }
}
