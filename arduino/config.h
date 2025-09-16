#pragma once

#include <Arduino.h>

const int days[7] = { 13, 14, 15, 16, 17, 18, 19 };
const int special[2] = { 21, 22 };
const int buttonPin = 26;

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

