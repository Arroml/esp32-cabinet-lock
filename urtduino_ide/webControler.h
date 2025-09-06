#pragma once
#include <WiFi.h>

class WebController {
public:
    WebController();
    void displayServer(WiFiClient client);
    void handleServer(String currentLine);
};