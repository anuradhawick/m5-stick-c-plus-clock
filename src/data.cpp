#include "data.h"

String days[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;

Storage::Storage()
{
}

void Storage::init()
{
    if (EEPROM.begin(32))
    {
        ESP_LOGD("EEPROM", "EEPROM initialized");
    }
    else
    {
        ESP_LOGD("EEPROM", "EEPROM initialization failed");
    }
}

Storage::~Storage()
{
    EEPROM.commit();
    ESP_LOGD("EEPROM", "EEPROM deinitialized");
}

void Storage::set_brightness(uint8_t brightness)
{
    EEPROM.writeUChar(0, brightness);
}

uint8_t Storage::get_brightness()
{
    uint8_t brightness = EEPROM.readUChar(0);
    ESP_LOGD("EEPROM", "Brightness read from EEPROM: %d", brightness);
    if (brightness < 10)
    {
        EEPROM.writeUChar(0, 10);
        return 10;
    }
    else if (brightness > 100)
    {
        EEPROM.writeUChar(0, 100);
        return 100;
    }
    else
    {
        return brightness;
    }
}

Storage storage;