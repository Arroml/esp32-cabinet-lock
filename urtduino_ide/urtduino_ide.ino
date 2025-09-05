#include <WiFi.h>
#include <time.h>
const char *ssid = "Home";
const char *password = "MeinAquarium1";

NetworkServer server(80);

int days[7] = { 13, 14, 15, 16, 17, 18, 19};
int special[2] = {21, 22};

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

bool isOpenFromServer[7] = {false};


void setup() {
  Serial.begin(115200);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  for (int i= 13; i<=19; i++){
    pinMode(i, OUTPUT);
  }
  pinMode(21, OUTPUT);
  pinMode(22, OUTPUT);

  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}
unsigned long lastUpdate =0;
const unsigned long upadeInterval = 10000;

void loop() {
  turnOnWday();



  NetworkClient client = server.accept();  // listen for incoming clients

  if (client) {                     // if you get a client,
    Serial.println("New Client.");  // print a message out the serial port
    String currentLine = "";        // make a String to hold incoming data from the client
    while (client.connected()) {    // loop while the client's connected
      if (client.available()) {     // if there's bytes to read from the client,
        char c = client.read();     // read a byte, then
        Serial.write(c);            // print it out the serial monitor
        if (c == '\n') {            // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            // the content of the HTTP response follows the header:
            displayServer(client);
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
        handleServer(currentLine);
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

void displayServer(NetworkClient client){
  client.print("Click <a href=\"/alH\">here</a> to open ALL. <br>");
  client.print("Click <a href=\"/alL\">here</a> to close ALL. <br>");
  client.print("<br>");
  client.print("Click <a href=\"/keyH\">here</a> to open KEY. <br>");
  client.print("Click <a href=\"/keyL\">here</a> to close KEY. <br>");
  client.print("<br>");
  client.print("Click <a href=\"/noH\">here</a> to open NOTFALL. <br>");
  client.print("Click <a href=\"/noL\">here</a> to close NOTFALL. <br>");
  client.print("<br>");
  client.print("Click <a href=\"/moH\">here</a> to open MONTAG. <br>");
  client.print("Click <a href=\"/moL\">here</a> to close MONTAG. <br>");
  client.print("<br>");
  client.print("Click <a href=\"/diH\">here</a> to open DIENSTAG. <br>");
  client.print("Click <a href=\"/diL\">here</a> to close DIENSTAG. <br>");
  client.print("<br>");
  client.print("Click <a href=\"/miH\">here</a> to open MITTWOCH. <br>");
  client.print("Click <a href=\"/miL\">here</a> to close MITTWOCH. <br>");
  client.print("<br>");
    client.print("Click <a href=\"/doH\">here</a> to open DONNERSTAG. <br>");
  client.print("Click <a href=\"/doL\">here</a> to close DONNERSTAG. <br>");
  client.print("<br>");
    client.print("Click <a href=\"/frH\">here</a> to open FREITAG. <br>");
  client.print("Click <a href=\"/frL\">here</a> to close FREITAG. <br>");
  client.print("<br>");
    client.print("Click <a href=\"/saH\">here</a> to open SAMSTAG. <br>");
  client.print("Click <a href=\"/saL\">here</a> to close SAMSTAG. <br>");
  client.print("<br>");
    client.print("Click <a href=\"/soH\">here</a> to open SONNTAG. <br>");
  client.print("Click <a href=\"/soL\">here</a> to close SONNTAG. <br>");
  client.print("<br>");

}

void handleServer(String currentLine){
  // Check to see if the client request was "GET /H" or "GET /L":
  if (currentLine.endsWith("GET /moH")) {
    digitalWrite(days[0], HIGH);  // GET /H turns the LED on
    isOpenFromServer[0] = true; 
  }
  if (currentLine.endsWith("GET /moL")) {
    digitalWrite(days[0], LOW);  // GET /L turns the LED off
    isOpenFromServer[0] = false;
  }
  if (currentLine.endsWith("GET /alH")) {
    for (int i =0; i<7; i++){
      digitalWrite(days[i], HIGH);
      isOpenFromServer[i] = true;
    }
    digitalWrite(special[0], HIGH);  // GET /H turns the LED on
    digitalWrite(special[1], HIGH);
  }
  if (currentLine.endsWith("GET /alL")) {
    for (int i =0; i<7; i++){
      digitalWrite(days[i], LOW);
      isOpenFromServer[i] = false;
    }
    digitalWrite(special[0], LOW);  // GET /H turns the LED on
    digitalWrite(special[1], LOW);
  }
  if (currentLine.endsWith("GET /noH")) {
    digitalWrite(special[1], HIGH);  // GET /H turns the LED on

  }
  if (currentLine.endsWith("GET /noL")) {
    digitalWrite(special[1], LOW);  // GET /L turns the LED off

  }
  if (currentLine.endsWith("GET /keyH")){
    digitalWrite(special[0], HIGH);
  }
  if (currentLine.endsWith("GET /keyL")){
    digitalWrite(special[0], LOW);
  }
  if (currentLine.endsWith("GET /diH")) {
    digitalWrite(days[1], HIGH);  // GET /H turns the LED on
    isOpenFromServer[1] = true;
  }
  if (currentLine.endsWith("GET /diL")) {
    digitalWrite(days[1], LOW);  // GET /L turns the LED off
    isOpenFromServer[1] = false;
  }
  if (currentLine.endsWith("GET /miH")) {
    digitalWrite(days[2], HIGH);  // GET /H turns the LED on
    isOpenFromServer[2] = true;
  }
  if (currentLine.endsWith("GET /miL")) {
    digitalWrite(days[2], LOW);  // GET /L turns the LED off
    isOpenFromServer[2] = false;
  }
  if (currentLine.endsWith("GET /doH")) {
    digitalWrite(days[3], HIGH);  // GET /H turns the LED on
    isOpenFromServer[3] = true;
  }
  if (currentLine.endsWith("GET /doL")) {
    digitalWrite(days[3], LOW);  // GET /L turns the LED off
    isOpenFromServer[3] = false;
  }
  if (currentLine.endsWith("GET /frH")) {
    digitalWrite(days[4], HIGH);  // GET /H turns the LED on
    isOpenFromServer[4] = true;
  }
  if (currentLine.endsWith("GET /frL")) {
    digitalWrite(days[4], LOW);  // GET /L turns the LED off
    isOpenFromServer[4] = false;
  }
  if (currentLine.endsWith("GET /saH")) {
    digitalWrite(days[5], HIGH);  // GET /H turns the LED on
    isOpenFromServer[5] = true;
  }
  if (currentLine.endsWith("GET /saL")) {
    digitalWrite(days[5], LOW);  // GET /L turns the LED off
    isOpenFromServer[5] = false;
  }
  if (currentLine.endsWith("GET /soH")) {
    digitalWrite(days[6], HIGH);  // GET /H turns the LED on
    isOpenFromServer[6] = true;
  }
  if (currentLine.endsWith("GET /soL")) {
    digitalWrite(days[6], LOW);  // GET /L turns the LED off
    isOpenFromServer[6] = false;
  }
}

void turnOnWday(){

  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    int wday = timeinfo.tm_wday;
    Serial.println(wday); // 0=Sonntag, 1=Montag, ..., 6=Samstag
    switch (wday){
      case 0: digitalWrite(days[6], HIGH);
      if (isOpenFromServer[5]==false){
        digitalWrite(days[5], LOW);
      }
      break;
      case 1: digitalWrite(days[0], HIGH);
      if (isOpenFromServer[6] == false){
        digitalWrite(days[6], LOW);
      }
      break;
      case 2: digitalWrite(days[1], HIGH);
      if (isOpenFromServer[0] == false){
        digitalWrite(days[0], LOW);
      }
      break;
      case 3: digitalWrite(days[2], HIGH);
      if (isOpenFromServer[1] == false){
        digitalWrite(days[1], LOW);
      }
      break;
      case 4: digitalWrite(days[3], HIGH);
      if (isOpenFromServer[2] == false){
        digitalWrite(days[2], LOW);
      }
      break;
      case 5: digitalWrite(days[4], HIGH);
      if (isOpenFromServer[3] == false){
        digitalWrite(days[3], LOW);
      }
      break;
      case 6: digitalWrite(days[5], HIGH);
      if (isOpenFromServer[4]== false){
        digitalWrite(days[4], LOW);
      }
      break;
    }
  }
}
