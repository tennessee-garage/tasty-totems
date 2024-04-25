//
// Created by Garth Webb on 6/23/23.
//

#ifndef TASTY_TOTEM_LEDSTRIP_H
#define TASTY_TOTEM_LEDSTRIP_H

#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include "DrumMontior.h"

// How long to leave the LED on for the strobe effect
#define DEFAULT_STROBE_MICROS 25000

class LedStrip {
public:
    LedStrip(uint8_t strip_pin, int num_pixels, DrumMonitor *drum);
    void setup();
    void update();

    // start a continuous strobe down the length of the LED strip
    void start_light_tracking(int start_idx, int end_idx);
    void stop_light_tracking();

    void set_frames_per_rotation(int frames);
    void set_rotations_per_drum(int rotations);
    void set_strobe_micros(unsigned long micros);
    void set_color(uint8_t r,  uint8_t g, uint8_t b);
    void set_pixel(int idx, uint8_t r,  uint8_t g, uint8_t b);
    void set_pixel(int idx, uint32_t _color);
    void set_pixel(int idx);
    void set_dual_pixel(int idx, uint8_t r, uint8_t g, uint8_t b);
    void set_dual_pixel(int idx, uint32_t _color);
    void set_dual_pixel(int idx);
    void clear();
    void show();

private:
    void _handle_tracking();
    void _start_strobe();
    void _handle_end_strobe();

    uint8_t _strip_pin;
    int _num_pixels;

    DrumMonitor *_drum;
    Adafruit_NeoPixel *_pixels;
    uint32_t _color;

    // Track the last frame we acted on
    uint8_t _last_frame;

    // Variables to manage the strobe effect a single flash per frame)
    unsigned long _strobe_duration_micros;
    unsigned long _strobe_end_micros;
    bool _is_strobe_active;


    // This determines whether we've started the strip
    bool _is_tracking_active;

    int _tracking_start_idx;
    int _tracking_end_idx;

    // How much farther into the strip should we light for each frame
    float _idx_incr_per_frame;
};

#endif //TASTY_TOTEM_LEDSTRIP_H
