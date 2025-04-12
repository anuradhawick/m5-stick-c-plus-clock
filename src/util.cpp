#include "util.h"
#include "secrets.h"

// WIFI Creds
static const char *TAG = "WIFI";


// Time
RTC_TimeTypeDef TimeStruct;
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 9 * 3600 + 1800; // ACST = UTC+9:30 (34200 sec)
const int daylightOffset_sec = 0;

bool connect_to_wifi()
{
    ESP_LOGI(TAG, "Connecting to WiFi...\n\tSSID: %s\n\tPassword: %s", WIFI_SSID, WIFI_PASS);
    M5.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.setSwapBytes(false);

    // try to connect to WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    int counter = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        M5.Lcd.print(".");
        counter++;
        if (counter > 20)
        {
            M5.Lcd.println("\nTimeout!");
            return false;
        }
    }
    M5.Lcd.println("\nConnected!");
    M5.Lcd.print("IP: ");
    M5.Lcd.println(WiFi.localIP());

    return true;
}
bool update_time_date()
{
    M5.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.setSwapBytes(false);

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        M5.Lcd.println("Failed to obtain NTP time");
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

    M5.Lcd.print("Time: ");
    M5.Lcd.print(timeStruct.Hours);
    M5.Lcd.print(":");
    M5.Lcd.print(timeStruct.Minutes);
    M5.Lcd.print(":");
    M5.Lcd.print(timeStruct.Seconds);
    M5.Lcd.print("\nDate: ");
    M5.Lcd.print(dateStruct.Year);
    ESP_LOGI(TAG, "Year: %d Month: %d Day: %d", dateStruct.Year, dateStruct.Month, dateStruct.Date);

    return true;
}
