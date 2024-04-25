#include "LedStrip.h"

LedStrip::LedStrip(uint8_t strip_pin, int num_pixels, DrumMonitor *drum) {
    _strip_pin = strip_pin;
    _num_pixels = num_pixels;
    _drum = drum;

    _pixels = new Adafruit_NeoPixel(_num_pixels, _strip_pin, NEO_GRB + NEO_KHZ800);

    _strobe_duration_micros = DEFAULT_STROBE_MICROS;
    _strobe_end_micros = 0;
    _is_strobe_active = false;

    _is_tracking_active = false;

    _tracking_start_idx = 0;
    _tracking_end_idx = num_pixels - 1;

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
    // Initialize NeoPixel strip object
    _pixels->begin();
}

void LedStrip::update() {
    _handle_tracking();
    _handle_end_strobe();
}

// Access the strip directly, as a single continuos strip
void LedStrip::set_pixel(int idx, uint8_t r, uint8_t g, uint8_t b) {
    set_color(r, g, b);
    set_pixel(idx);
}

void LedStrip::set_pixel(int idx, uint32_t color) {
    _color = color;
    set_pixel(idx);
}

void LedStrip::set_pixel(int idx) {
    _pixels->setPixelColor(idx, _color);
    _pixels->show();
}

// Treat the two halves as a pair, with idx=0 at the top of the column
void LedStrip::set_dual_pixel(int idx, uint8_t r, uint8_t g, uint8_t b) {
    set_color(r, g, b);
    set_dual_pixel(idx);
}

void LedStrip::set_dual_pixel(int idx, uint32_t color) {
    _color = color;
    set_dual_pixel(idx);
}

void LedStrip::set_dual_pixel(int idx) {
    set_pixel((_num_pixels >> 1) - idx - 1);
    set_pixel((_num_pixels >> 1) + idx);
}

void LedStrip::clear() {
    _pixels->clear();
}

void LedStrip::show() {
    _pixels->show();
}

void LedStrip::_start_strobe() {
    int idx = (_drum->get_current_frame() * _idx_incr_per_frame) + _tracking_start_idx;
    set_dual_pixel(idx);

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

    _idx_incr_per_frame = (float) (end_idx - start_idx + 1) / (float) TOTAL_FRAMES;
}

void LedStrip::stop_light_tracking() {
    _is_tracking_active = false;
}

void LedStrip::_handle_tracking() {
    // Return if we're not currently tracking via the LED strip
    if (!_is_tracking_active)
        return;

    // Detect whether we've started a new frame, and if so, start a strobe
    uint8_t frame = _drum->get_current_frame();
    if (_last_frame != frame) {
        _last_frame = frame;
        _start_strobe();
    }
}
