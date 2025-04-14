#include <Arduino.h>
#include <M5StickCPlus.h>
#include "time.h"
#include "views/View.h"
#include "views/TimeView.h"
#include "views/BTView.h"
#include "views/AnalogClock.h"
#include "views/BrightnessView.h"
#include "views/WiFiView.h"
#include "tasks.h"
#include "util.h"
#include "data.h"

static const char *TAG = "MAIN";

View *active_view;
uint8_t state = 1;
unsigned long active_time;

void event_loop(bool change_state = false);
bool change_stage();

void setup()
{
    M5.begin();
    M5.Imu.Init();
    storage.init();
    M5.Lcd.setRotation(3);
    M5.Lcd.setSwapBytes(true);
    M5.Axp.ScreenBreath(storage.get_brightness());

    active_view = new AnalogClock();
    active_time = millis();
}

void loop()
{
    M5.update();
    // render view
    active_view->render();

    // no dimming as long as in shake
    if (storage.get_imu())
    {
        dimmer.tick();
        
        if (imumgr.is_moved()) {
            dimmer.interrup();
        }
    }
    else
    {
        dimmer.interrup();
    }

    // capture events
    // Lower button
    if (M5.BtnB.wasReleased())
    {
        ESP_LOGD(TAG, "BtnB Pressed");
        !dimmer.interrup() && active_view->receive_event(EVENTS::RESET_PRESSED);
        if (storage.get_beep())
        {
            M5.Beep.beep();
        }
    }

    // Home button
    if (M5.BtnA.wasReleased())
    {
        ESP_LOGD(TAG, "BtnA Pressed");
        // if event not consumed - change page
        !dimmer.interrup() && !active_view->receive_event(EVENTS::HOME_PRESSED) && change_stage();
        if (storage.get_beep())
        {
            M5.Beep.beep();
        }
    }

    // Button on Top
    if (M5.Axp.GetBtnPress() == 0x02)
    {
        ESP_LOGD(TAG, "Power Btn Pressed");
        !dimmer.interrup() && active_view->receive_event(EVENTS::POWER_PRESSED);
        if (storage.get_beep())
        {
            M5.Beep.beep();
        }
    }
}

bool change_stage()
{
    state++;

    if (state > 5)
    {
        state = 1;
    }

    delete active_view;
    active_view = NULL;

    switch (state)
    {
    case 1:
        active_view = new AnalogClock();
        break;
    case 2:
        active_view = new TimeView();
        break;
    case 3:
        active_view = new BrightnessView();
        break;
    case 4:
        active_view = new BTView();
        break;
    case 5:
        active_view = new WiFiView();
        break;

    default:
        break;
    }

    return true;
}
