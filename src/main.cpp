#include <Arduino.h>
#include <M5StickCPlus.h>
#include "time.h"
#include "views/View.h"
#include "views/TimeView.h"
#include "views/BTView.h"
#include "views/AnalogClock.h"
#include "views/BrightnessView.h"
#include "tasks.h"
#include "util.h"
#include "data.h"

static const char *TAG = "MAIN";

View *active_view;
uint8_t state = 1;

TASKS::Dimmer dimmer;
TASKS::IMUManager imumgr;
extern Storage storage;

void event_loop(bool change_state = false);
bool change_stage();

void setup()
{
    // connect_to_wifi();
    // delay(2000);
    // update_time_date();
    // delay(2000);

    M5.begin();
    M5.Imu.Init();
    storage.init();
    M5.Axp.ScreenBreath(storage.get_brightness());

    // active_view = new AnalogClock();
    active_view = new BTView();
}

void loop()
{
    M5.update();
    // render view
    active_view->render();

    // // no dimming as long as in shake
    // if (imumgr.is_moved())
    // {
    //     dimmer.recover();
    //     active_time = millis();
    // }

    // // auto dimming timed
    // if (active_time + 5000 < millis())
    // {
    //     dimmer.go_dim();
    // }

    // // auto screenoff timed
    // if (active_time + 10000 < millis())
    // {
    //     dimmer.go_dark();
    // }

    // capture events
    // Lower button
    if (M5.BtnB.wasReleased())
    {
        ESP_LOGD(TAG, "BtnB Pressed");
        active_view->receive_event(EVENTS::RESET_PRESSED);
    }

    // Home button
    if (M5.BtnA.wasReleased())
    {
        ESP_LOGD(TAG, "BtnA Pressed");
        // if event not consumed - change page
        !active_view->receive_event(EVENTS::HOME_PRESSED) && change_stage();
    }

    // Button on Top
    if (M5.Axp.GetBtnPress() == 0x02)
    {
        ESP_LOGD(TAG, "Power Btn Pressed");
        active_view->receive_event(EVENTS::POWER_PRESSED);
    }
}

bool change_stage()
{
    state++;

    if (state > 4)
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

    default:
        break;
    }

    return true;
}
