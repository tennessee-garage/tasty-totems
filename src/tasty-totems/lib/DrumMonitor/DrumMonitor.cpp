#include "DrumMontior.h"

DrumMonitor::DrumMonitor(EncoderMonitor *encoder) {
    _encoder = encoder;
    reset();
}

unsigned long DrumMonitor::micros_per_frame() {
    uint16_t rpm = _encoder->get_current_rpm();
    float rotations_per_sec = rpm/60.0;

    return (rotations_per_sec/FRAMES_PER_ROTATION) * 1000000;
}

void DrumMonitor::increment_rotation() {
    _rotation_counter = (_rotation_counter + 1) % ROTATIONS_PER_DRUM;
}

uint8_t DrumMonitor::get_current_frame() {
    // This is zero based indexing; in the event `rotation_progress` is exactly 1, `frame_in_rotation` will
    // actually represent the first frame in the next rotation, which is fine since callers shouldn't care
    // what rotation we're on.  However if this is also the last rotation, we need to make sure this counter
    // wraps around to zero.  Thus the modulo `% TOTAL_FRAMES` below.
    int frame_in_rotation = _encoder->rotation_progress() * FRAMES_PER_ROTATION;
    return (frame_in_rotation + (_rotation_counter * FRAMES_PER_ROTATION)) % TOTAL_FRAMES;
}

void DrumMonitor::reset() {
    _rotation_counter = 0;
}
