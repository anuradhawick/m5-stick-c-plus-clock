#include "util.h"

// WIFI Creds
static const char *TAG = "WIFI";

// Time
RTC_TimeTypeDef TimeStruct;
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 9 * 3600 + 1800; // ACST = UTC+9:30 (34200 sec)
const int daylightOffset_sec = 0;

bool connect_to_wifi(String wifi_ssid, String wifi_pass)
{
    ESP_LOGI(TAG, "Connecting to WiFi...\n\tSSID: %s\n\tPassword: %s", wifi_ssid, wifi_pass);

    // try to connect to WiFi
    M5.begin();
    WiFi.begin(wifi_ssid, wifi_pass);
    int counter = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        counter++;
        if (counter > 20)
        {
            return false;
        }
    }
    return true;
}

bool disconnect_wifi()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        WiFi.disconnect();
        WiFi.mode(WIFI_OFF);
        return true;
    } else {
        WiFi.mode(WIFI_OFF);
    }
    return false;
}

bool update_time_date()
{
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm timeinfo;
    
    if (!getLocalTime(&timeinfo))
    {
        return false;
    }
    // Convert and set RTC time
    RTC_TimeTypeDef timeStruct;
    RTC_DateTypeDef dateStruct;

    timeStruct.Hours = timeinfo.tm_hour;
    timeStruct.Minutes = timeinfo.tm_min;
    timeStruct.Seconds = timeinfo.tm_sec;

    dateStruct.WeekDay = timeinfo.tm_wday;
    dateStruct.Month = timeinfo.tm_mon + 1; // tm_mon is 0-11
    dateStruct.Date = timeinfo.tm_mday;
    dateStruct.Year = timeinfo.tm_year + 1900; // tm_year is years since 1900

    M5.Rtc.SetTime(&timeStruct);
    M5.Rtc.SetDate(&dateStruct);

    ESP_LOGI(TAG, "Year: %d Month: %d Day: %d", dateStruct.Year, dateStruct.Month, dateStruct.Date);

    return true;
}
