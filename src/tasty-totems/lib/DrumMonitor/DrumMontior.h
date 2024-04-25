#ifndef TASTY_TOTEM_DRUMMONITOR_H
#define TASTY_TOTEM_DRUMMONITOR_H

#include "Arduino.h"
#include "EncoderMonitor.h"

// How many frames fit around the drum circumference
#define FRAMES_PER_ROTATION 16
#define ROTATIONS_PER_DRUM   8
#define TOTAL_FRAMES (FRAMES_PER_ROTATION * ROTATIONS_PER_DRUM)

#define TARGET_ROTATIONS_PER_SECOND ((1.0*TARGET_RPM)/60.0)
#define SECONDS_PER_FRAME    (TARGET_ROTATIONS_PER_SECOND/FRAMES_PER_ROTATION)
#define MICROS_PER_FRAME     (SECONDS_PER_FRAME * 1000000)
#define STOBE_TIME_PERCENT   0.05
#define STROBE_MICROS        (MICROS_PER_FRAME * STOBE_TIME_PERCENT)

class DrumMonitor {
public:
    DrumMonitor(EncoderMonitor *encoder);
   
    unsigned long micros_per_frame();
    uint8_t get_current_frame();
    void increment_rotation();
    void reset();
private:
    EncoderMonitor *_encoder;
    uint8_t _rotation_counter;
};

#endif // TASTY_TOTEM_DRUMMONITOR_H