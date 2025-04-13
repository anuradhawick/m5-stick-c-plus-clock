#include "views/BTView.h"

static const char *TAG = "BT_VIEW";
BluetoothSerial *BTView::btSerial = nullptr;
BTView *BTView::instance = nullptr;

void BTView::static_bt_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    if (BTView::instance)
    {
        BTView::instance->bt_callback(event, param);
    }
}

BTView::BTView()
{
    instance = this;
    M5.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.setSwapBytes(true);

    disp_buffer = new TFT_eSprite(&M5.Lcd);

    disp_buffer->setSwapBytes(true);
    disp_buffer->createSprite(240, 135);
    disp_buffer->fillRect(0, 0, 240, 135, BLACK);
    disp_buffer->pushSprite(0, 0);

    disp_buffer->setTextSize(1);
    disp_buffer->setTextColor(TFT_WHITE, TFT_BLACK);

    loader_last_rendered = 0;
    text_last_rendered = 0;
    loader_frame = 0;

    inited_time = millis();

    render_text();
    render_loader();
}

BTView::~BTView()
{
    ESP_LOGD(TAG, "Destructor called");
    if (bt_started > 0)
    {
        BTView::btSerial->end();
        delete BTView::btSerial;
        BTView::btSerial = NULL;
    }

    disp_buffer->deleteSprite();
    delete disp_buffer;

    ESP_LOGD(TAG, "Destructor finished");
}

void BTView::write_to_bt(const char *data)
{
    if (bt_started > 0)
    {
        btSerial->write((uint8_t *)data, strlen(data));
    }
}

void BTView::render_loader()
{
    disp_buffer->pushImage(240 - loader_ani_width,
                           67 - loader_ani_height / 2,
                           loader_ani_width,
                           loader_ani_height,
                           loader_ani_imgs[loader_frame]);
    loader_frame++;

    if (loader_frame == loader_ani_frames)
    {
        loader_frame = 0;
    }
    disp_buffer->pushSprite(0, 0);
}

void BTView::render_text()
{
    switch (bt_started)
    {
    case 0:
        disp_buffer->fillRect(0, 0, 140, 135, BLACK);
        disp_buffer->setCursor(0, 15);
        disp_buffer->setTextFont(0);
        disp_buffer->drawString("BT Loading", 10, 10, 4);
        disp_buffer->drawString("Wait " + String(5 - (millis() - inited_time) / 1000) + "s", 10, 50, 2);
        disp_buffer->drawString("to Continue", 10, 70, 2);
        disp_buffer->pushSprite(0, 0);
        break;
    case 1:
        disp_buffer->fillRect(0, 0, 140, 135, BLACK);
        disp_buffer->setCursor(0, 15);
        disp_buffer->setTextFont(0);
        disp_buffer->drawString("BT Active", 10, 10, 4);
        disp_buffer->drawString("Connect with your", 10, 50, 2);
        disp_buffer->drawString("BT Serial App", 10, 70, 2);
        disp_buffer->drawString("to Continue", 10, 90, 2);
        disp_buffer->pushSprite(0, 0);
        break;
    case 2:
        disp_buffer->fillRect(0, 0, 140, 135, BLACK);
        disp_buffer->setCursor(0, 15);
        disp_buffer->setTextFont(0);
        disp_buffer->drawString("BT Linked", 10, 10, 4);
        disp_buffer->drawString("Device Connected", 10, 50, 2);
        disp_buffer->drawString("Follow the Terminal", 10, 70, 2);
        disp_buffer->drawString("Instructions", 10, 90, 2);
        disp_buffer->pushSprite(0, 0);
        break;
    }
}

void BTView::render()
{
    if (millis() > loader_last_rendered + 100)
    {
        loader_last_rendered = millis();
        render_loader();
    }

    if (millis() > text_last_rendered + 1000)
    {
        text_last_rendered = millis();
        render_text();
    }

    if (bt_started > 0)
    {
        bt_loop();
    }
    else if (!bt_started && inited_time + 5000 < millis())
    {
        BTView::btSerial = new BluetoothSerial();
        BTView::btSerial->register_callback(static_bt_callback);
        BTView::btSerial->begin("M5Stick");
        ESP_LOGD(TAG, "BT Serial Started");
        bt_started = 1;
    }
}

void BTView::bt_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    if (event == ESP_SPP_SRV_OPEN_EVT)
    {
        ESP_LOGD(TAG, "Client Connected");
        write_to_bt(main_menu_text.c_str());
        main_menu = 0;
        bt_started = 2;
    }

    if (event == ESP_SPP_CLOSE_EVT)
    {
        ESP_LOGD(TAG, "Client disconnected");
        main_menu = 0;
        bt_started = 1;
    }
}

void BTView::bt_loop()
{
    if (!BTView::btSerial->available())
    {
        return;
    }

    bt_incoming = BTView::btSerial->readString();
    bt_incoming.trim();
    ESP_LOGD(TAG, "Incoming from BT Serial - %s", bt_incoming.c_str());

    switch (main_menu)
    {
    case 0:
        // just reusing the variable
        main_menu = (uint8_t)bt_incoming.toInt();
        text = "";

        switch (main_menu)
        {
        case 1:
            text = "Enter WIFI SSID\n";
            break;
        case 2:
            text = "Enter time as HH:MM\n";
            break;
        case 3:
            text = "Enter date as DD/MM/YYYY/D, D=1 for Sunday\n";
            break;
        case 4:
            storage.set_beep(!storage.get_beep());
            if (storage.get_beep())
            {
                text = "Beep enabled\n";
                M5.Beep.beep();
            }
            else
            {
                text = "Beep disabled\n";
            }
            break;
        case 5:
            text = "Created by:\n Anuradha Wickramarachchi\n anuradhawick.com\n hello@anuradhawick.com\n";
            break;
        default:
            break;
        }
        break;
    case 1:
        ESP_LOGD(TAG, "Update ssid or password");
        if (ssid.length() == 0)
        {
            ssid = bt_incoming.c_str();
            text = "SSID successfully updated\nNow enter the WIFI password\n";
            ESP_LOGD(TAG, "Update ssid to \"%s\"", ssid.c_str());
        }
        else
        {
            password = bt_incoming.c_str();
            text = "Password successfully updated\nProceed to WiFi screen to confirm functionality\n";
            write_to_bt(text.c_str());
            ESP_LOGD(TAG, "Update password to \"%s\"", password.c_str());
            storage.set_wifi_ssid(ssid);
            storage.set_wifi_password(password);
            ssid = "";
            password = "";
            main_menu = 0;
            text = main_menu_text;
        }
        break;
    case 2:
        RTC_TimeStruct.Hours = (uint8_t)bt_incoming.substring(0, 2).toInt();
        RTC_TimeStruct.Minutes = (uint8_t)bt_incoming.substring(3, 5).toInt();

        ESP_LOGD(TAG, "Update time to %d:%d",
                 RTC_TimeStruct.Hours,
                 RTC_TimeStruct.Minutes,
                 bt_incoming.substring(0, 2),
                 bt_incoming.substring(3, 5));

        M5.Rtc.SetTime(&RTC_TimeStruct);

        text = "Time successfully updated\n";

        break;
    case 3:
        RTC_DateStruct.Date = (uint8_t)bt_incoming.substring(0, 2).toInt();
        RTC_DateStruct.Month = (uint8_t)bt_incoming.substring(3, 5).toInt();
        RTC_DateStruct.Year = (uint16_t)bt_incoming.substring(6, 10).toInt();
        RTC_DateStruct.WeekDay = (uint16_t)bt_incoming.substring(11, 12).toInt() - 1;

        ESP_LOGD(TAG, "Update date to %d/%d/%d/%d",
                 RTC_DateStruct.Date,
                 RTC_DateStruct.Month,
                 RTC_DateStruct.Year,
                 RTC_DateStruct.WeekDay);

        M5.Rtc.SetDate(&RTC_DateStruct);

        text = "Date successfully updated\n";

        break;
    default:
        text = "Invalid option\n";
        break;
    }

    write_to_bt(text.c_str());
}