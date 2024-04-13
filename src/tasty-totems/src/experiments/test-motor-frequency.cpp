#include "tasty-totems.h"
#include "EncoderMonitor.h"
#include "MotorControl.h"

#define TARGET_RPM 100

EncoderMonitor *ENCODER;
MotorControl *MOTOR;

void setup() {
    Serial.begin(115200);
    Serial.println("Setup begin");

    ENCODER = new EncoderMonitor(ENCODER_C1_PIN, ENCODER_C2_PIN);
    ENCODER->begin();

    MOTOR = new MotorControl(MOTOR_PIN_1, MOTOR_PIN_2, ENCODER);
    MOTOR->set_pwm_channel(PWM_CHANNEL);
    MOTOR->setup();

    MOTOR->start_motor(TARGET_RPM, 0.8);
}

void loop() {

}