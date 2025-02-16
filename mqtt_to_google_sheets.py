import paho.mqtt.client as mqtt
import requests
import threading
import queue

# MQTT Broker details
MQTT_BROKER = "IPv4 Address"
MQTT_TOPIC = "sensor/ammonia"

# Google Apps Script URL
GOOGLE_SCRIPT_URL = "URL Google Apps Script"

# Queue to process messages asynchronously
message_queue = queue.Queue()

def send_to_google_sheets():
    while True:
        esp_id, value = message_queue.get()
        data = {"id": esp_id, "ammonia": value}
        
        try:
            response = requests.get(GOOGLE_SCRIPT_URL, params=data, timeout=5)
            print(f"✅ Sent {data} → Google Sheets, Response: {response.text}")
        except requests.exceptions.RequestException as e:
            print(f"❌ Error sending to Google Sheets: {e}")
        
        message_queue.task_done()

# Start a background thread for sending data
threading.Thread(target=send_to_google_sheets, daemon=True).start()

def on_message(client, userdata, message):
    payload = message.payload.decode("utf-8")
    
    if ":" in payload:
        esp_id, value = payload.split(":")
        message_queue.put((esp_id, value))
        print(f"📥 Queued {esp_id}: {value}")

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_message = on_message
client.connect(MQTT_BROKER, 1883)
client.subscribe(MQTT_TOPIC)

print("🚀 Listening for MQTT messages...")
client.loop_forever()
