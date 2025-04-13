#pragma once

#include <Preferences.h>
#include <M5StickCPlus.h>

class Storage
{
public:
    Storage();
    ~Storage();

    void init();
    void set_brightness(uint8_t brightness);
    uint8_t get_brightness();

    void set_wifi_ssid(String ssid);
    String get_wifi_ssid();

    void set_wifi_password(String password);
    String get_wifi_password();

    bool get_beep();
    void set_beep(bool beep);

private:
    bool started = false;
    Preferences prefs;
};

extern Storage storage;