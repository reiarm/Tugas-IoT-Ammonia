#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

// Wi-Fi credentials
const char* ssid = "wifi";
const char* password = "pw";

// Google Sheets API details
const char* googleScriptURL = "Google Script URL";

// MQ2 Sensor Pin
const int mq2Pin = A0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  delay(2000);
}


void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    int sensorValue = analogRead(mq2Pin);
    float voltage = sensorValue * (3.3 / 1023.0);
    float ammoniaConcentration = map(sensorValue, 0, 1023, 0, 100);

    sendDataToGoogleSheets(ammoniaConcentration);
  }
  
  delay(5000);
}

void sendDataToGoogleSheets(float ammoniaConcentration) {
  WiFiClientSecure client;
  client.setInsecure();  // Ignore SSL validation
  
  HTTPClient http;
  String url = String(googleScriptURL) + "?ammonia=" + String(ammoniaConcentration);
  
  http.begin(client, url);  
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  if (httpResponseCode == 200) {
  Serial.println("✅ Data successfully sent to Google Sheets!");
  } else {
    Serial.print("❌ Failed to send data. Response code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}
