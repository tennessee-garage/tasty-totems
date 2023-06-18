#include <Adafruit_NeoPixel.h>
#include <math.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define STRIP_PIN 5

// Number of pixels in the strip
#define NUM_PIXELS 35

#define GEARING     100
#define ENCODER_MULT 14

// Motor encoder variables
const int encoder_c1_pin = 23;
const int encoder_c2_pin = 22;
const int ir_detect_pin = 21;

unsigned long c1_rise_time, c2_rise_time;
unsigned long encoder_delta;
int direction = 0;

// Motor speed/PWM variables
const double freq = 80000;
const int led_channel = 0;
const int resolution = 13;
const int motor_pin = 18;
float speed = 6400.0;
const float pK = 0.50;
// Time in ms to wait between speed adjustments
unsigned long adjust_ttl = 300;
unsigned long adjust_reset_ms = millis();

// NeoPixel strip variables
// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUM_PIXELS, STRIP_PIN, NEO_GRB + NEO_KHZ800);
const int num_frames_per_rotation = 16;
unsigned long rotation_micros = 0;
unsigned long last_rotation_micros = 0;
unsigned long micros_per_frame = 0;
unsigned long micros_led_on = 1000;
unsigned long frame_micro_count = 0;

int speed_initialized = 0;
int rotations = 0;


int num_frames = 128;
int current_frame = 0;
int start_strip_idx = 3;
int end_strip_idx = 34;
int led_index = start_strip_idx;
float increment_step = num_frames/(end_strip_idx-start_strip_idx);
float progress = 0;

// Trim the time per frame in micros to get properly synced
unsigned long trim_micros = -2000;


void IRAM_ATTR encoder_c1() {
  if (digitalRead(encoder_c2_pin) == HIGH) {
    // We're laggging.  Calculate the delta and store
    encoder_delta = micros() - c2_rise_time;
    direction = 1;
  } else {
    // We're ahead.  Store the start time
    c1_rise_time = micros();
  }
}

void IRAM_ATTR encoder_c2() {
  if (digitalRead(encoder_c1_pin) == HIGH) {
    // We're laggging.  Calculate the delta and store
    encoder_delta = micros() - c1_rise_time;
    direction = 0;
  } else {
    // We're ahead.  Store the start time
    c2_rise_time = micros();
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(encoder_c1_pin, INPUT);
  pinMode(encoder_c2_pin, INPUT);
  pinMode(ir_detect_pin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt (encoder_c1_pin), encoder_c1, RISING);
  attachInterrupt(digitalPinToInterrupt (encoder_c2_pin), encoder_c2, RISING);

  // configure LED PWM functionalitites
  ledcSetup(led_channel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(motor_pin, led_channel);

  Serial.begin(115200);

  // Initialize NeoPixel strip object
  pixels.begin();

  ledcWrite(led_channel, int(speed));
}

void set_speed() {
  float freq, rev;
  float desired_rpm = 100;
  float change = 0.0;

  freq = 1.0 / encoder_delta;
  freq *= 1000000;
  freq *= 60;
  rev = freq / GEARING;
  rev /= ENCODER_MULT;

  if (desired_rpm > rev) {
    change = pK*(desired_rpm - rev);
  } else if (desired_rpm < rev) {
    change = pK*(rev-desired_rpm)*-1.0;
  }

  speed += change;

  if (speed > 8191) {
    speed = 8191;
  }
  if (speed < 0) {
    speed = 0;
  }
  ledcWrite(led_channel, int(speed));

/*
  Serial.print("target_rpm:");
  Serial.print(desired_rpm);
  Serial.print(",current_rpm:");
  Serial.print(rev);
  Serial.print(",change:");
  Serial.println(change);
  */
}


void loop() {
  float freq, rev;

  // Detect when millis has looped around
  if (adjust_reset_ms > millis()) {
    adjust_reset_ms = millis();
  }

  // See if we've hit the TTL for speed control adjustments
  if ((millis() - adjust_reset_ms) >= adjust_ttl) {
    set_speed();
    adjust_reset_ms = millis();
  }

  if (speed_initialized) {
    // See if its time to turn on the LED for the next frame
    if (micros() - frame_micro_count > micros_per_frame) {
      if (current_frame > round(progress+increment_step)) {
        progress += increment_step;
        led_index++;
      }

      if (current_frame >= num_frames) {
        progress = 0;
        led_index = start_strip_idx;
        current_frame = 0;
      }

      pixels.setPixelColor(35-led_index, pixels.Color(150, 150, 150));
      pixels.show();

      frame_micro_count = micros();
      current_frame++;

      Serial.print("num_frames: ");
      Serial.print(num_frames);
      Serial.print(" / progress: ");
      Serial.println(progress);
    }

    // Turn off the LED after a period of time to get the strobe effect
    if (micros() - frame_micro_count > micros_led_on) {
      pixels.clear();
      pixels.show();
    }
  }

  if (digitalRead(ir_detect_pin) == LOW) {
    // protect this against overflow at some point
    last_rotation_micros = micros() - rotation_micros;
    rotation_micros = micros();
    micros_per_frame = (last_rotation_micros/num_frames_per_rotation) + trim_micros;
    rotations++;

    // Reset all the strobe tracking
    frame_micro_count = micros();

    // Wait here until it goes high again
    while (digitalRead(ir_detect_pin) == LOW) {}
  }
 
  // Wait at least 2 rotations for speed to settle and vars to be set
  if (rotations > 2) {
    speed_initialized = 1;
    if (rotations == 3) {
      pixels.setPixelColor(led_index, pixels.Color(150, 150, 150));
      pixels.show();
    }
  }
}
