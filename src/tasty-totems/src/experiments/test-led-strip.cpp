#include "tasty-totems.h"
#include "LedStrip.h"

LedStrip *STRIP;
Adafruit_NeoPixel *pixels;

void setup() {
    Serial.begin(115200);
    Serial.println("Setup begin");

    Serial.print("Initalizing strip ... ");
    STRIP = new LedStrip(LED_STRIP_PIN, NUM_STRIP_PIXELS);
    STRIP->set_color(200, 0, 0);
    STRIP->set_frames_per_rotation(FRAMES_PER_ROTATION);
    STRIP->set_rotations_per_drum(ROTATIONS_PER_DRUM);
    STRIP->set_strobe_micros(500);
    STRIP->setup();
    Serial.println("done");

    STRIP->set_dual_pixel(20);

   /*
    pixels = new Adafruit_NeoPixel(NUM_STRIP_PIXELS*2, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);
    uint32_t color = Adafruit_NeoPixel::Color(128, 0, 0);

    pixels->clear();
    pixels->setPixelColor(20, color);
    pixels->show();
    */
}

void loop() {
    uint32_t color = Adafruit_NeoPixel::Color(128, 0, 0);

    for (int i=0; i<(NUM_STRIP_PIXELS>>1); i++) {
        STRIP->clear();
        STRIP->set_dual_pixel(i);
        delay(20);
    }
}
