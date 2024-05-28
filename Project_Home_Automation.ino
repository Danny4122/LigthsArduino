#include "thingProperties.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "LAN(D) OF JERUSALEM";
const char* password = "@Isaiah65Eighteen**";

bool lastSwitch1State = false;
bool lastSwitch2State = false;
unsigned long lastDebounceTimeSwitch1 = 0;
unsigned long lastDebounceTimeSwitch2 = 0;
const unsigned long debounceDelay = 200; // 200 ms debounce time

void setup() {
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  Serial.begin(9600);
  delay(500);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
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

  // Debounce and check switch 1 state
  if ((millis() - lastDebounceTimeSwitch1) > debounceDelay) {
    if (switch1 != lastSwitch1State) {
      lastSwitch1State = switch1;
      onSwitch1Change();
      lastDebounceTimeSwitch1 = millis();
    }
  }

  // Debounce and check switch 2 state
  if ((millis() - lastDebounceTimeSwitch2) > debounceDelay) {
    if (switch2 != lastSwitch2State) {
      lastSwitch2State = switch2;
      onSwitch2Change();
      lastDebounceTimeSwitch2 = millis();
    }
  }

  // If WiFi is not connected, inform the user
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected. Unable to send data to server");
  }
}

void sendSwitchState(String switchName, String state) {
  if (WiFi.status() == WL_CONNECTED) {
    static String lastSentStateSwitch1 = ""; // Add static variables to store last sent state
    static String lastSentStateSwitch2 = "";

    // Check if the state has changed before sending
    if ((switchName == "switch1" && state != lastSentStateSwitch1) ||
        (switchName == "switch2" && state != lastSentStateSwitch2)) {
      WiFiClient client;
      HTTPClient http;

      String url = "http://192.168.1.24:3000/api/switchevents"; // Ensure this is the correct IP and port
      Serial.println("Connecting to URL: " + url);

      http.begin(client, url);
      http.addHeader("Content-Type", "application/json");

      // Get the current timestamp
      String timestamp = getTimestamp();

      // Properly format JSON data including switch name, state, and timestamp
      String json = "{\"switch\": \"" + switchName + "\", \"state\": \"" + state + "\", \"timestamp\": \"" + timestamp + "\"}";

      Serial.println("JSON data: " + json);

      int httpResponseCode = http.POST(json);

      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("HTTP Response code: " + String(httpResponseCode));
        Serial.println("Response: " + response);
        
        // Update last sent state
        if (switchName == "switch1") {
          lastSentStateSwitch1 = state;
        } else if (switchName == "switch2") {
          lastSentStateSwitch2 = state;
        }
      } else {
        Serial.print("Error Sending data to server. HTTP Response code: ");
        Serial.println(httpResponseCode);
        Serial.println(http.errorToString(httpResponseCode));
      }

      http.end();
    }
  } else {
    Serial.println("WiFi not connected. Unable to send data to server");
  }
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
           2024 + days / 365, 29 + (days % 365) / 30, 29 + (days % 365) % 30,
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
  sendSwitchState("switch1", switch1 ? "on" : "off"); // Send state change to server
}

void onSwitch2Change() {
  if (switch2) {
    digitalWrite(D1, LOW);
    Serial.println("Switch2 turned ON");
  } else {
    digitalWrite(D1, HIGH);
    Serial.println("Switch2 turned OFF");
  }
  sendSwitchState("switch2", switch2 ? "on" : "off"); // Send state change to server
}
