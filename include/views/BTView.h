#pragma once

#include <M5StickCPlus.h>
#include <BluetoothSerial.h>
#include "View.h"

class BTView : public View
{
public:
    BTView();
    ~BTView();

    void render();
    bool receive_event(EVENTS::event event){ return false; };

private:
    void bt_loop();
    void render_loader();
    void render_text();
    void bt_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
    static void static_bt_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);

    unsigned long loader_last_rendered;
    unsigned long text_last_rendered;

    uint16_t loader_frame;
    String bt_incoming;
    String text;

    TFT_eSprite *disp_buffer;
    uint8_t main_menu = 0;

    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;

    uint8_t bt_started = 0; // 0 - not started, 1 - started, 2 - connected
    unsigned long inited_time;
    static BTView *instance;
    static BluetoothSerial *btSerial;
};
