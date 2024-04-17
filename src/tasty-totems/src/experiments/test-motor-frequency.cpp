#include "tasty-totems.h"
#include "EncoderMonitor.h"
#include "MotorControl.h"

#define ANALOG_PIN 4 // 13

#define TARGET_RPM 100

#define MIN_PWM_FREQ  10
#define MAX_PWM_FREQ 500

#define MIN_ANALOG_VALUE 0
#define MAX_ANALOG_VALUE 4095

#define PWM_STEP (1.0*(MAX_PWM_FREQ-MIN_PWM_FREQ)/(MAX_ANALOG_VALUE-MIN_ANALOG_VALUE))
#define PWM_CHANGE_THRESHOLD 5

#define DEFAULT_DUTY_CYCLE 0.9

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

    //pinMode(ANALOG_PIN, INPUT);

    Serial.print("Initializing encoder ...  ");
    ENCODER = new EncoderMonitor(ENCODER_C1_PIN, ENCODER_C2_PIN);
    ENCODER->begin();
    Serial.println("done");

    Serial.print("Initializing motor ... ");
    MOTOR = new MotorControl(MOTOR_PIN_1, MOTOR_PIN_2, ENCODER);
    MOTOR->set_pwm_channel(PWM_CHANNEL);
    MOTOR->setup();
    Serial.println("done");

    Serial.print("Starting motor at ");
    Serial.print(DEFAULT_DUTY_CYCLE * 100);
    Serial.print("\% duty cycle ... ");
    MOTOR->start_motor(TARGET_RPM, 0.50);//DEFAULT_DUTY_CYCLE);
    Serial.println("done");
}

uint32_t read_pwm_freq() {
    analog_value = analogRead(ANALOG_PIN);

    Serial.print("\tAnalog val: ");
    Serial.println(analog_value);
    Serial.print("\tReturning ");
    Serial.print(MIN_PWM_FREQ);
    Serial.print(" + (");
    Serial.print(analog_value);
    Serial.print(" * ");
    Serial.print(PWM_STEP);
    Serial.print(") == ");
    Serial.println(MIN_PWM_FREQ + (analog_value * PWM_STEP));

    return MIN_PWM_FREQ + (analog_value * PWM_STEP);
}

bool past_change_threshold() {
    return abs((int) (new_pwm - old_pwm)) > PWM_CHANGE_THRESHOLD;
}

bool can_provide_rpm_update() {
    return millis() > last_status_update_ms + STATUS_UPDATE_DELAY_MS;
}

float accum = 0.0;
void loop() {

    //new_pwm = read_pwm_freq();

    //Serial.print("-- new_pwm: ");
    //Serial.println(new_pwm);

    // Only change if the difference in PWM freq changes by more than PWM_CHANGE_THRESHOLD
    if (0 || past_change_threshold()) {
        MOTOR->set_pwm_freq(MIN_PWM_FREQ + (analog_value * PWM_STEP));
        Serial.print("PWM freq - (delta:");
        Serial.print(abs((int)(old_pwm-new_pwm)));
        Serial.print(") ");
        Serial.print(old_pwm);
        Serial.print(" -> ");
        Serial.println(new_pwm);
        old_pwm = new_pwm;
    }

    if (can_provide_rpm_update()) {
        //MOTOR->set_pwm_duty_cycle((float)0.5);

        Serial.print(">pError:");
        Serial.println(MOTOR->_pid_error() * PROPORTIONAL_K);
        Serial.print(">rpm:");
        Serial.println(ENCODER->get_current_rpm());

        Serial.print(">dErrro:");
        Serial.println(MOTOR->get_derivative_error() * DERIVATIVE_K);
        Serial.print(">pidResponse:");
        Serial.println((MOTOR->_pid_error() * PROPORTIONAL_K) + (MOTOR->get_derivative_error() * DERIVATIVE_K));

        Serial.print(">iError:");
        Serial.println(MOTOR->get_integral_error());
        Serial.print(">dutyCyle:");
        Serial.println(MOTOR->get_pwm_duty_cycle());

        Serial.print(">errorPercent:");
        Serial.println(100*MOTOR->error_percent());

        //Serial.print(">pidError:");
        //Serial.println(MOTOR->_pid_error());
        last_status_update_ms = millis();
    }

    MOTOR->update();
    //delay(1000);
}
