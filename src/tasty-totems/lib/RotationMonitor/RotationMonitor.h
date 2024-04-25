#ifndef TASTY_TOTEM_ROTATIONMONITOR_H
#define TASTY_TOTEM_ROTATIONMONITOR_H

#include "Arduino.h"
#include "EncoderMonitor.h"

class RotationMonitor {
public:
    static RotationMonitor* instance;

    RotationMonitor(uint8_t ir_pin, EncoderMonitor *encoder);
    void begin();

    void handle_ir();
    static void IRAM_ATTR static_ir_handler();

    bool is_rotation_complete();
    void reset_rotation_compete_flag();
    unsigned long last_rotation_micros();


    unsigned long _last_rise_counter;

private:
    uint8_t _ir_pin;
    EncoderMonitor *_encoder;

    unsigned long _rotation_duration_micros;
    unsigned long _rotation_start_micros;
    bool _rotation_complete_flag;
};

#endif // TASTY_TOTEM_ROTATIONMONITOR_H