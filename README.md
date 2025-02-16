
# Tugas 1 IoT Ammonia

Sistem Deteksi MQ-2 Secara Realtime di Spreadsheet


## Tech Stack

OLD CODE = HTTP

ESP 1 & ESP 2 = MQTT Broker https://mosquitto.org/
## Run Code

Clone the project

Go to the project directory

Install dependencies

```bash
pip install paho-mqtt requests
```

Start Mosquitto Broker

```bash
mosquitto
```

Run Python Subscriber Script

```bash
python mqtt_to_google_sheets.py
```

Run ESP_1 & ESP_2
