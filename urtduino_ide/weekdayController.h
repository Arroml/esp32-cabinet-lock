#pragma once
#include <time.h>
#include <Arduino.h>

class WeekdayController {
private:
    int* days;
    bool* isOpenFromServer;

public:
    WeekdayController(int* daysArray, bool* openStatusArray);
    void turnOnWday();
};