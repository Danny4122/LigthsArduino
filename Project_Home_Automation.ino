#include "thingProperties.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "LAN(D) OF JERUSALEM";
const char* password = "@Isaiah65Eighteen**";

void setup() {
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  Serial.begin(9600);
  delay(5000);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(5000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize properties
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();

  if (WiFi.status() == WL_CONNECTED) {
    sendSwitchState();
  } else {
    Serial.println("WiFi not connected. Unable to send data to server");
  }
}

void sendSwitchState() {
  WiFiClient client;
  HTTPClient http;

  String url = "http://192.168.1.24:3000/api/switchevents"; // Ensure this is the correct IP and port
  Serial.println("Connecting to URL: " + url);

  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");

  // Get the current timestamp
  String timestamp = getTimestamp();

  // Properly format JSON data including switch1, switch2, state, and timestamp
  String json = "{\"switch\": \"" + String(switch1 ? "switch1" : "switch2") + "\", \"state\": \"" + String(switch1 ? (switch1 ? "on" : "off") : (switch2 ? "on" : "off")) + "\", \"timestamp\": \"" + timestamp + "\"}";

  Serial.println("JSON data: " + json);
  
  int httpResponseCode = http.POST(json);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    Serial.println("Response: " + response);
  } else {
    Serial.print("Error Sending data to server. HTTP Response code: ");
    Serial.println(httpResponseCode);
    Serial.println(http.errorToString(httpResponseCode));
  }

  http.end();
}

String getTimestamp() {
  // Get the current timestamp in milliseconds
  unsigned long currentMillis = millis(); // Assuming millis() provides the current time in milliseconds since the device started
  
  // Calculate the components of the timestamp
  unsigned long seconds = currentMillis / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  unsigned long days = hours / 24;
  
  // Format the timestamp in the desired format ("YYYY-MM-DDTHH:MM:SSZ")
  String timestamp = "";
  char formattedTime[20];
  snprintf(formattedTime, 20, "%04lu-%02lu-%02luT%02lu:%02lu:%02luZ",
           1970 + days / 365, 1 + (days % 365) / 30, 1 + (days % 365) % 30,
           hours % 24, minutes % 60, seconds % 60);
  timestamp = String(formattedTime);
  
  return timestamp;
}


void onSwitch1Change() {
  if (switch1) {
    digitalWrite(D0, LOW);
    Serial.println("Switch1 turned ON");
  } else {
    digitalWrite(D0, HIGH);
    Serial.println("Switch1 turned OFF");
  }
  sendSwitchState(); // Send state change to server
}

void onSwitch2Change() {
  if (switch2) {
    digitalWrite(D1, LOW);
    Serial.println("Switch2 turned ON");
  } else {
    digitalWrite(D1, HIGH);
    Serial.println("Switch2 turned OFF");
  }
  sendSwitchState(); // Send state change to server
}
