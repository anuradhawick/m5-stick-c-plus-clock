#pragma once

#include <M5StickCPlus.h>
#include <BluetoothSerial.h>
#include "View.h"
#include "animations/loader.h"
#include "util.h"
#include "data.h"

class WiFiView : public View
{
public:
    WiFiView();
    ~WiFiView();

    void render();
    bool receive_event(EVENTS::event event) { return false; };

private:
    void render_loader();
    void render_text();

    unsigned long loader_last_rendered;
    unsigned long text_last_rendered;
    unsigned long inited_time;
    uint8_t wifi_started = 0; // 0 - not started, 1 - connected, 2 - unable to connect

    uint16_t loader_frame;
    TFT_eSprite *disp_buffer;
    String ip_address;
};
