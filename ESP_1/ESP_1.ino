#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <math.h>

const char* ssid = "wifi";
const char* password = "pw";
const char* mqtt_server = "IPv4 Address";

WiFiClient espClient;
PubSubClient client(espClient);

String GAS_SENSOR_ID = "ESP-1";

// MQ-2 Sensor Constants
const int mq2Pin = A0;
const float RL = 10.0;  // Load resistor value (kΩ)
const float R0 = 9.83;  // R0 (calibrated in clean air)
const float m = -0.58;  // Slope (MQ-2 datasheet)
const float b = 1.58;   // Intercept (MQ-2 datasheet)

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi!");

    client.setServer(mqtt_server, 1883);
    while (!client.connected()) {
        Serial.print("Connecting to MQTT...");
        if (client.connect(GAS_SENSOR_ID.c_str())) {
            Serial.println("Connected!");
        } else {
            Serial.print("Failed, rc=");
            Serial.print(client.state());
            Serial.println(" retrying in 5 seconds...");
            delay(5000);
        }
    }
}

void loop() {
    int gasValue = analogRead(mq2Pin);
    
    // Convert ADC value to voltage
    float sensorVoltage = (gasValue / 1023.0) * 3.3;  
    
    // Calculate sensor resistance (RS)
    float RS = ((3.3 - sensorVoltage) / sensorVoltage) * RL;

    // Compute RS/R0 ratio
    float ratio = RS / R0;

    // Calculate NH3 PPM using MQ-2 datasheet formula
    float ammoniaPPM = pow(10, ((log10(ratio) - b) / m));

    // Create MQTT message (send PPM, not raw ADC)
    String payload = GAS_SENSOR_ID + ":" + String(ammoniaPPM, 2);
    client.publish("sensor/ammonia", payload.c_str());

    Serial.print("Sent to MQTT: "); 
    Serial.println(payload);
    
    delay(5000);
}
