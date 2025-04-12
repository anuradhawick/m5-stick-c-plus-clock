#pragma once

#include <EEPROM.h>
#include <M5StickCPlus.h>

class Storage
{
public:
    Storage();
    ~Storage();
    
    void init();
    void set_brightness(uint8_t brightness);
    uint8_t get_brightness();
};
