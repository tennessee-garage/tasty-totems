//
// Created by Garth Webb on 6/23/23.
//

#ifndef TASTY_TOTEM_LEDSTRIP_H
#define TASTY_TOTEM_LEDSTRIP_H

#include "Arduino.h"
#include <Adafruit_NeoPixel.h>

#define UNSIGNED_LONG_MAX 4294967295

// How long to leave the LED on for the strobe effect
#define DEFAULT_STROBE_MICROS 1000

class LedStrip {
public:
    LedStrip(uint8_t strip_pin, int num_pixels);
    void setup();
    void update();

    void sync(unsigned long rotation_period, unsigned long trim_micros);

    // start a continuous strobe down the length of the LED strip
    void start_light_tracking(int start_idx, int end_idx);
    void stop_light_tracking();

    void set_frames_per_rotation(int frames);
    void set_rotations_per_drum(int rotations);
    void set_strobe_micros(unsigned long micros);
    void set_color(uint8_t r,  uint8_t g, uint8_t b);
    void set_pixel(int idx, uint8_t r,  uint8_t g, uint8_t b);
    void clear();
    void show();

private:
    void _handle_tracking();
    void _start_strobe();
    void _handle_end_strobe();
    void _update_number_of_frames();

    uint8_t _strip_pin;
    int _num_pixels;

    Adafruit_NeoPixel *_pixels;
    uint32_t _color;

    // Variables to manage the strobe effect a single flash per frame)
    unsigned long _strobe_duration_micros;
    unsigned long _strobe_end_micros;
    bool _is_strobe_active;

    // Variables to manage the light tracking effect, where each light in the strip
    // strobes on each frame as it passes

    // This sets intent to be tracking, though tracking may not yet have started
    bool _is_tracking_active;
    // This determines whether we've actually started the lights.  This will
    // be set in the 'sync' method so that we don't actually start the light
    // tracking until we're at the starting point.
    bool _is_tracking_started;

    int _tracking_start_idx;
    int _tracking_end_idx;

    // A floating point continuum between the start and end pixel that we round to the nearest
    // integer pixel for display (or maybe do something fancy like partially lighting pixels based
    // on the value.
    float _continuous_pixel_idx;

    // The time for one rotation in microseconds
    unsigned long _rotation_period;

    int _frames_per_rotation;
    int _frame_counter;
    int _rotation_counter;

    int _rotations_per_drum;
    int _number_of_frames;
    unsigned long _frame_micros;
    unsigned long _next_frame_start_micros;

    // How much farther into the strip should we light for each frame
    float _idx_incr_per_frame;
};

#endif //TASTY_TOTEM_LEDSTRIP_H
