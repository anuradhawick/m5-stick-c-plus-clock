#include "data.h"

static const char *TAG = "STORAGE";
String days[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;

Storage::Storage()
{
}

void Storage::init()
{
    if (prefs.begin("settings", false))
    {
        ESP_LOGD(TAG, "Preferences initialized");
    }
    else
    {
        ESP_LOGD(TAG, "Preferences initialization failed");
    }
}

Storage::~Storage()
{
    prefs.end();
    ESP_LOGD(TAG, "Preferences deinitialized");
}

void Storage::set_brightness(uint8_t brightness)
{
    prefs.putUChar("brightness", brightness);
    ESP_LOGD(TAG, "Brightness saved to preferences: %d", brightness);
}

uint8_t Storage::get_brightness()
{
    uint8_t brightness = prefs.getUChar("brightness");
    ESP_LOGD(TAG, "Brightness read from preferences: %d", brightness);
    if (brightness < 10 || brightness > 100)
    {
        prefs.putUChar("brightness", 50);
        return 50;
    }
    else
    {
        return brightness;
    }
}

void Storage::set_wifi_ssid(String ssid)
{
    prefs.putString("wifi_ssid", ssid);
}

String Storage::get_wifi_ssid()
{
    String ssid = prefs.getString("wifi_ssid");
    ESP_LOGD(TAG, "SSID read from preferences: %s", ssid.c_str());
    return ssid;
}

void Storage::set_wifi_password(String password)
{
    prefs.putString("wifi_password", password);
}

String Storage::get_wifi_password()
{
    String password = prefs.getString("wifi_password");
    ESP_LOGD(TAG, "Password read from preferences: %s", password.c_str());
    return password;
}

bool Storage::get_beep()
{
    bool beep = prefs.getBool("beep");
    ESP_LOGD(TAG, "Beep read from preferences: %d", beep);
    return beep;
}

void Storage::set_beep(bool beep)
{
    prefs.putBool("beep", beep);
    ESP_LOGD(TAG, "Beep saved to preferences: %d", beep);
}

Storage storage;