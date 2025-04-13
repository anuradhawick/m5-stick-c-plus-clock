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

TASKS::Dimmer dimmer;
TASKS::IMUManager imumgr;

void event_loop(bool change_state = false);
bool change_stage();
void init_sequence();

void setup()
{
    M5.begin();
    M5.Imu.Init();
    storage.init();
    M5.Lcd.setRotation(3);
    M5.Lcd.setSwapBytes(true);
    M5.Axp.ScreenBreath(storage.get_brightness());

    // init_sequence();
    active_view = new BrightnessView();
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
        if (storage.get_beep())
        {
            M5.Beep.beep();
        }
        active_view->receive_event(EVENTS::RESET_PRESSED);
    }

    // Home button
    if (M5.BtnA.wasReleased())
    {
        ESP_LOGD(TAG, "BtnA Pressed");
        if (storage.get_beep())
        {
            M5.Beep.beep();
        }
        // if event not consumed - change page
        !active_view->receive_event(EVENTS::HOME_PRESSED) && change_stage();
    }

    // Button on Top
    if (M5.Axp.GetBtnPress() == 0x02)
    {
        ESP_LOGD(TAG, "Power Btn Pressed");
        if (storage.get_beep())
        {
            M5.Beep.beep();
        }
        active_view->receive_event(EVENTS::POWER_PRESSED);
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

void init_sequence()
{
    // showing progress
    TFT_eSprite *disp_buffer = new TFT_eSprite(&M5.Lcd);

    disp_buffer->setSwapBytes(true);
    disp_buffer->createSprite(240, 135);
    disp_buffer->fillRect(0, 0, 240, 135, BLACK);
    disp_buffer->pushSprite(0, 0);

    disp_buffer->setTextSize(1);
    disp_buffer->setTextColor(TFT_WHITE, TFT_BLACK);

    // attempting to connect to wifi
    disp_buffer->fillRect(0, 0, 240, 135, BLACK);
    disp_buffer->setCursor(0, 15);
    disp_buffer->setTextFont(0);
    disp_buffer->drawString("WIFI Linking", 10, 10, 4);
    disp_buffer->pushSprite(0, 0);
    delay(1000);

    if (connect_to_wifi(storage.get_wifi_ssid(), storage.get_wifi_password()))
    {
        disp_buffer->fillRect(0, 0, 240, 135, BLACK);
        disp_buffer->setCursor(0, 15);
        disp_buffer->setTextFont(0);
        disp_buffer->drawString("WIFI Connected", 10, 10, 4);
        disp_buffer->drawString("Updating Time", 10, 50, 4);
        disp_buffer->pushSprite(0, 0);
        ESP_LOGI(TAG, "Connected to WiFi");
        update_time_date();
        delay(2000);
    }
    else
    {
        disp_buffer->fillRect(0, 0, 240, 135, BLACK);
        disp_buffer->setCursor(0, 15);
        disp_buffer->setTextFont(0);
        disp_buffer->drawString("WIFI Failed", 10, 10, 4);
        disp_buffer->drawString("Update Settings", 10, 50, 4);
        disp_buffer->pushSprite(0, 0);
        ESP_LOGE(TAG, "Failed to connect to WiFi");
        delay(2000);
    }

    delete disp_buffer;
}