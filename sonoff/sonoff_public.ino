
#include <ESP8266WebServer.h>

#define LED 02  // 13 - sonoff LED, 2 - esp8266 onboard LED
#define REL 12

const char* ssid = "SSID";
const char* password = "PASSWORD";

int rstate = 0;

ESP8266WebServer server(80);

void connectWiFi()
{
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  bool state = HIGH;
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED, state);
    Serial.print(".");
    state = not state;
    delay(500);
  }
  digitalWrite(LED, HIGH);
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


void handleOn()
{
  digitalWrite(LED, LOW);
  digitalWrite(REL, HIGH);
  rstate = 1;
  server.send(200, "text/plain", "ON");
}


void handleOff()
{
  digitalWrite(LED, HIGH);
  digitalWrite(REL, LOW);
  rstate = 0;
  server.send(200, "text/plain", "OFF");
}


void handleToggle()
{
  if (rstate == 1) {
    handleOff();
  }
  else {
    handleOn();
  }
}


void handleShowStatus() {
  if (rstate == 1) {
    server.send(200, "text/plain", "ON");
  }
  else {
    server.send(200, "text/plain", "OFF");
  }
}


void handleNotFound(){
  String message = "Command not supported\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  message += "Supported commands are:\n";
  message += "  /on\n";
  message += "  /off\n";
  message += "  /toggle\n";
  message += "  /status\n";
  server.send(404, "text/plain", message);
}


void setup() 
{
  pinMode(LED, OUTPUT);
  pinMode(REL, OUTPUT);
  digitalWrite(LED, LOW);
  digitalWrite(REL, LOW);

  Serial.begin(115200);
  delay(10);
  Serial.println("Serial started");

  connectWiFi();

  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.on("/toggle", handleToggle);
  server.on("/status", handleShowStatus);
  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() 
{
  server.handleClient();
}
