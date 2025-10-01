#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include "secrets.h"
#include "config.h"

WiFiServer server(80);
// --- Zustand ---
int buttonPressCount = 0;
const int maxButtonPresses = 10;
unsigned long lastResetTime = 0;
const unsigned long resetInterval = 5 * 60 * 1000; // 5 Minuten

bool isActivated = false;
unsigned long lastActivationTime = 0;
const unsigned long holdTime = 100; // 500 ms

// --- Funktionserklärungen ---
void checkButton();
void turnOnWday();
void turnOffAll();
void turnOnAll();
void resetButton();
void handleTimeout();
void displayServer(WiFiClient client);
void handleWebRequests(String currentLine);

void setup() {
  Serial.begin(115200);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  pinMode(buttonPin, INPUT_PULLUP);
  for (int i = 0; i < 7; i++) pinMode(days[i], OUTPUT);
  for (int i = 0; i < 2; i++) pinMode(special[i], OUTPUT);

  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  resetButton();
  checkButton();
  turnOnWday();
  handleTimeout();

  WiFiClient client = server.accept();

  if (client) {
    Serial.println("New Client.");
    String currentLine = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);

        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            displayServer(client);
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        handleWebRequests(currentLine);
      }
    }

    client.stop();
    Serial.println("Client Disconnected.");
  }
}

void resetButton() {
  if (millis() - lastResetTime > resetInterval) {
    buttonPressCount = 0;
    lastResetTime = millis();
    Serial.println("Button-Zähler zurückgesetzt.");
  }
}

void checkButton() {
  if (digitalRead(buttonPin) == LOW && !isActivated) {
    if (buttonPressCount < maxButtonPresses) {
      Serial.println("Button gedrückt.");
      isActivated = true;
      lastActivationTime = millis();
      buttonPressCount++;
    } else {
      Serial.println("Maximale Anzahl an Öffnungen erreicht.");
    }
  }
}

void turnOnWday() {
  if (!isActivated) return;

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return;

  int wday = timeinfo.tm_wday;
  int currentIndex = (wday + 6) % 7;
  int previousIndex = (currentIndex + 6) % 7;

  digitalWrite(days[currentIndex], HIGH);
  digitalWrite(days[previousIndex], LOW);
}

void handleTimeout() {
  if (isActivated && millis() - lastActivationTime > holdTime) {
    turnOffAll();
    isActivated = false;
  }
}

void turnOnAll() {
  for (int i = 0; i < 7; i++) digitalWrite(days[i], HIGH);
  for (int i = 0; i < 2; i++) digitalWrite(special[i], HIGH);
  isActivated = true;
  lastActivationTime = millis();
}

void turnOffAll() {
  for (int i = 0; i < 7; i++) digitalWrite(days[i], LOW);
  for (int i = 0; i < 2; i++) digitalWrite(special[i], LOW);
}

void handleWebRequests(String currentLine) {
  const char* dayCommands[] = { "mo", "di", "mi", "do", "fr", "sa", "so" };

  for (int i = 0; i < 7; i++) {
    if (currentLine.endsWith("GET /" + String(dayCommands[i]) + "H")) {
      digitalWrite(days[i], HIGH);
      isActivated = true;
      lastActivationTime = millis();
      return;
    }
  }

  if (currentLine.endsWith("GET /keyH")) {
    digitalWrite(special[0], HIGH);
    isActivated = true;
    lastActivationTime = millis();
    return;
  }

  if (currentLine.endsWith("GET /noH")) {
    digitalWrite(special[1], HIGH);
    isActivated = true;
    lastActivationTime = millis();
    return;
  }

  if (currentLine.endsWith("GET /alH")) {
    turnOnAll(); // setzt isActivated selbst
    return;
  }
}

void displayServer(WiFiClient client) {
  struct Link { const char* path; const char* label; };
  Link links[] = {
    {"/alH", "Alle öffnen"}, {"/keyH", "Key"}, {"/noH", "Notfall"},
    {"/moH", "Montag"}, {"/diH", "Dienstag"}, {"/miH", "Mittwoch"},
    {"/doH", "Donnerstag"}, {"/frH", "Freitag"}, {"/saH", "Samstag"}, {"/soH", "Sonntag"}
  };

  client.println("<html><head><style>");
  client.println("body { font-family: Arial; padding: 20px; }");
  client.println("button { margin: 5px; padding: 10px 20px; background-color: #007BFF; color: white; border: none; border-radius: 5px; cursor: pointer; }");
  client.println("button:hover { background-color: #0056b3; }");
  client.println("</style></head><body><h2>Schranksteuerung</h2>");

  for (Link link : links) {
    client.print("<form action=\""); client.print(link.path);
    client.println("\" method=\"GET\" style=\"display:inline\">");
    client.print("<button type=\"submit\">");
    client.print(link.label);
    client.println("</button></form>");
  }

  client.println("</body></html>");
}
