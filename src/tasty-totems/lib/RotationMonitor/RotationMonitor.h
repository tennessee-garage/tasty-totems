#ifndef TASTY_TOTEM_ROTATIONMONITOR_H
#define TASTY_TOTEM_ROTATIONMONITOR_H

#include "Arduino.h"

class RotationMonitor {
public:
    static RotationMonitor* instance;

    RotationMonitor(uint8_t ir_pin);
    void begin();

    void handle_ir();
    static void IRAM_ATTR static_ir_handler();

    bool is_rotation_complete();
    void reset_rotation_compete_flag();
    unsigned long last_rotation_micros();

private:
    uint8_t _ir_pin;

    unsigned long _rotation_duration_micros;
    unsigned long _rotation_start_micros;
    bool _rotation_complete_flag;
};

#endif // TASTY_TOTEM_ROTATIONMONITOR_H