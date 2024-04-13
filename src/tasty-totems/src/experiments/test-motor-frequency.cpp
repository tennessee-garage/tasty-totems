#include "tasty-totems.h"
#include "EncoderMonitor.h"
#include "MotorControl.h"

#define ANALOG_PIN 20

#define TARGET_RPM 100

#define MIN_PWM_FREQ   1000
#define MAX_PWM_FREQ 100000

#define MIN_ANALOG_VALUE 0.0
#define MAX_ANALOG_VALUE 1.0

#define PWM_STEP ((MAX_PWM_FREQ-MIN_PWM_FREQ)/(MAX_ANALOG_VALUE-MIN_ANALOG_VALUE))
#define PWM_CHANGE_THRESHOLD 10

#define STATUS_UPDATE_DELAY_MS 500

EncoderMonitor *ENCODER;
MotorControl *MOTOR;
float analog_value = 0.0;
uint32_t new_pwm = DEFAULT_PWM_FREQ;
uint32_t old_pwm = DEFAULT_PWM_FREQ;

unsigned long last_status_update_ms = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("Setup begin");

    pinMode(ANALOG_PIN, INPUT);

    ENCODER = new EncoderMonitor(ENCODER_C1_PIN, ENCODER_C2_PIN);
    ENCODER->begin();

    MOTOR = new MotorControl(MOTOR_PIN_1, MOTOR_PIN_2, ENCODER);
    MOTOR->set_pwm_channel(PWM_CHANNEL);
    MOTOR->setup();

    MOTOR->start_motor(TARGET_RPM, 0.8);
}

void loop() {
    analog_value = analogRead(ANALOG_PIN);
    new_pwm = MIN_PWM_FREQ + (analog_value * PWM_STEP);

    // Only change if the difference in PWM freq changes by more than PWM_CHANGE_THRESHOLD
    if (abs((int) (new_pwm - old_pwm)) > PWM_CHANGE_THRESHOLD) {
        MOTOR->set_pwm_freq(MIN_PWM_FREQ + (analog_value * PWM_STEP));
        Serial.print("PWM freq: ");
        Serial.print(old_pwm);
        Serial.print(" -> ");
        Serial.println(new_pwm);
    }

    if (millis() > last_status_update_ms + STATUS_UPDATE_DELAY_MS) {
        Serial.print("RPM: ");
        Serial.println(ENCODER->get_current_rpm());
        last_status_update_ms = millis();
    }
}
