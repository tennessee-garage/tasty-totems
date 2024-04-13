//
// Created by Garth Webb on 6/23/23.
//

#include "LedStrip.h"

LedStrip::LedStrip(uint8_t strip_pin, int num_pixels) {
    _strip_pin = strip_pin;
    _num_pixels = num_pixels;

    _pixels = new Adafruit_NeoPixel(_num_pixels, _strip_pin, NEO_GRB + NEO_KHZ800);

    _strobe_duration_micros = DEFAULT_STROBE_MICROS;
    _strobe_end_micros = 0;
    _is_strobe_active = false;

    _is_tracking_active = false;
    _is_tracking_started = false;

    _tracking_start_idx = 0;
    _tracking_end_idx = num_pixels - 1;

    _continuous_pixel_idx = 0;

    _rotation_period = UNSIGNED_LONG_MAX;
    _frames_per_rotation = 1;
    _frame_counter = 0;
    _rotation_counter = 0;
    _rotations_per_drum = 1;

    _number_of_frames = 0;
    _update_number_of_frames();

    _frame_micros = 1;
    _next_frame_start_micros = 0;
    _idx_incr_per_frame = 1;

    // Pick a bright visible color as default in case the caller forgets to set
    _color = 0;
    set_color(255, 0, 0);
}

void LedStrip::set_strobe_micros(unsigned long micros) {
    _strobe_duration_micros = micros;
}

void LedStrip::set_color(uint8_t r,  uint8_t g, uint8_t b) {
    _color = Adafruit_NeoPixel::Color(r, g, b);
}

void LedStrip::setup() {
    _update_number_of_frames();

    // Initialize NeoPixel strip object
    _pixels->begin();
}

void LedStrip::update() {
    _handle_tracking();
    _handle_end_strobe();
}

// This method will be called every time the main program confirms we've made one rotation
// It will pass in the time the last rotation took as 'rotation_period'.
void LedStrip::sync(unsigned long rotation_period, unsigned long trim_micros) {
    unsigned long sync_start_micros = micros();
    _rotation_period = rotation_period;
    _next_frame_start_micros = sync_start_micros + trim_micros;

    // How many micros each frame is in view
    _frame_micros = _rotation_period / _frames_per_rotation;

    // If tracking hasn't started, reset and start it here and now
    if (!_is_tracking_started) {
        _is_tracking_started = true;
        _rotation_counter = 0;
        _frame_counter = 0;
        _continuous_pixel_idx = (float) _tracking_start_idx;
        return;
    }

    Serial.print("== frame_micros: ");
    Serial.print(_frame_micros);
    Serial.print(" / trim_micros: ");
    Serial.println(trim_micros);

    Serial.print(" PRE: rotation_counter: ");
    Serial.print(_rotation_counter);
    Serial.print(" / frame_counter: ");
    Serial.print(_frame_counter);
    Serial.print(" / continuous_pixel_idx: ");
    Serial.println(_continuous_pixel_idx);

    // Increment the rotation we're on
    _rotation_counter++;
    if (_rotation_counter >= _rotations_per_drum) {
        _rotation_counter = 0;
    }

    // Based on the rotation number, force the frame we're on to sync us to the right frame
    _frame_counter = _rotation_counter * _frames_per_rotation;
    // Make sure we loop back to zero
    if (_frame_counter > _number_of_frames) {
        _frame_counter = 0;
    }
    // Force the continuous pixel index based on the updated frame counter
    _continuous_pixel_idx = (float) _tracking_start_idx + (_idx_incr_per_frame * _frame_counter);

    Serial.print("POST: rotation_counter: ");
    Serial.print(_rotation_counter);
    Serial.print(" / frame_counter: ");
    Serial.print(_frame_counter);
    Serial.print(" / continuous_pixel_idx: ");
    Serial.println(_continuous_pixel_idx);

}

void LedStrip::set_pixel(int idx, uint8_t r, uint8_t g, uint8_t b) {
    set_color(r, g, b);
    _pixels->setPixelColor((int) ((float) _num_pixels - _continuous_pixel_idx), _color);
    _pixels->show();
}

void LedStrip::clear() {
    _pixels->clear();
}

void LedStrip::show() {
    _pixels->show();
}

void LedStrip::_start_strobe() {
    _pixels->setPixelColor((int) ((float) _num_pixels - _continuous_pixel_idx), _color);
    _pixels->show();

    _strobe_end_micros = micros() + _strobe_duration_micros;
    _is_strobe_active = true;
}

void LedStrip::_handle_end_strobe() {
    // Skip if we're not handling a strobe
    if (!_is_strobe_active)
        return;

    // Return immediately if we aren't ready to end the strobe
    // TODO: Figure out how to handle loop around of unsigned long
    if (micros() < _strobe_end_micros)
        return;

    _pixels->clear();
    _pixels->show();

    _is_strobe_active = false;
}

void LedStrip::start_light_tracking(int start_idx, int end_idx) {
    _tracking_start_idx = start_idx;
    _tracking_end_idx = end_idx;

    _is_tracking_active = true;
    _is_tracking_started = false;

     _idx_incr_per_frame = (float) (end_idx - start_idx + 1) / (float) _number_of_frames;
}

void LedStrip::stop_light_tracking() {
    _is_tracking_active = false;
}

void LedStrip::_handle_tracking() {
    // Return if we're not currently tracking via the LED strip
    if (!_is_tracking_active)
        return;

    if (!_is_tracking_started)
        return;

    // Don't keep strobing past the number of frames we're meant to have per rotation
    // This addresses any edge cases around a very quick rotation causing a double strobe
    // in quick succession at the next sync
    if (_frame_counter >= _number_of_frames)
        return;

    // Return if it's not time to start the next frame
    if (micros() < _next_frame_start_micros)
        return;

    _next_frame_start_micros += _frame_micros;
    _continuous_pixel_idx += _idx_incr_per_frame;
    _frame_counter++;

    _start_strobe();
}

void LedStrip::set_frames_per_rotation(int frames) {
    _frames_per_rotation = frames;
}

void LedStrip::set_rotations_per_drum(int rotations) {
    _rotations_per_drum = rotations;
}

void LedStrip::_update_number_of_frames() {
    _number_of_frames = _frames_per_rotation * _rotations_per_drum;
}
