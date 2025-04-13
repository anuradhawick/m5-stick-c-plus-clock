#include <M5StickCPlus.h>
#include "views/View.h"
#include "event.h"
#include "views/BrightnessView.h"
#include "data.h"

static const char *TAG = "BRIGHTNESS_VIEW";
extern Storage storage;

BrightnessView::BrightnessView()
{
    M5.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.setSwapBytes(false);

    // I have reverse engineered this code from the AXP192.h header file
    brightness = storage.get_brightness();
    rendering_brightness_value = brightness;
    last_rendered = millis();

    disp_buffer = new TFT_eSprite(&M5.Lcd);

    disp_buffer->setSwapBytes(false);
    disp_buffer->createSprite(240, 135);
    disp_buffer->fillRect(0, 0, 240, 135, BLACK);

    disp_buffer->setTextSize(1);
    disp_buffer->setTextFont(4);
    disp_buffer->setTextColor(TFT_WHITE, TFT_BLACK);

    header_message = "Adjust Brightness";

    disp_buffer->drawString(header_message,
                            M5.Lcd.width() / 2 - M5.Lcd.textWidth(header_message, 4) / 2,
                            5,
                            4);
    disp_buffer->fillTriangle(0,
                              M5.Lcd.height() - 25,
                              2.4 * brightness,
                              M5.Lcd.height() - 25,
                              2.4 * brightness,
                              (double)M5.Lcd.height() - 25 - 2.4 * (double)brightness * tan(PI / 10),
                              WHITE);
    disp_buffer->pushSprite(0, 0);
}

BrightnessView::~BrightnessView()
{
    ESP_LOGD(TAG, "Destructor called");
    disp_buffer->deleteSprite();
    delete disp_buffer;
    ESP_LOGD(TAG, "Destructor finished");
}

void BrightnessView::render()
{
    if (last_rendered + 10 < millis() && rendering_brightness_value != brightness)
    {
        disp_buffer->fillRect(0, 0, 240, 135, BLACK);
        disp_buffer->drawString(header_message,
                                M5.Lcd.width() / 2 - M5.Lcd.textWidth(header_message, 4) / 2,
                                5,
                                4);
        // screen is 240x135
        // 10 block brightness is 2.4 pixels
        // top right corner comes from tangent
        disp_buffer->fillTriangle(0,
                                  M5.Lcd.height() - 25,
                                  2.4 * brightness,
                                  M5.Lcd.height() - 25,
                                  2.4 * brightness,
                                  (double)M5.Lcd.height() - 25 - 2.4 * (double)brightness * tan(PI / 10),
                                  WHITE);
        disp_buffer->pushSprite(0, 0);
        last_rendered = millis();
        rendering_brightness_value = brightness;
    }
}

bool BrightnessView::receive_event(EVENTS::event event)
{
    switch (event)
    {
    case EVENTS::RESET_PRESSED:
        brightness -= 10;

        if (brightness < 10)
        {
            brightness = 10;
        }
        M5.Axp.ScreenBreath(brightness);
        storage.set_brightness(brightness);
        return true;
    case EVENTS::POWER_PRESSED:
        brightness += 10;

        if (brightness > 100)
        {
            brightness = 100;
        }
        M5.Axp.ScreenBreath(brightness);
        storage.set_brightness(brightness);
        return true;
    default:
        break;
    }
    return false;
}
