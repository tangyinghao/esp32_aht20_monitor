# Room Temperature & Humidity Monitor

This project was started from me noticing that my room becomes warmer around 2-3 AM, which has caused difficulty sleeping and night sweats, despite lowering the temperature shown on the air conditioning remote. I've been unable to prove it to my parents to convince actual action towards fixing this issue, so hopefully this works as evidence to support my currently anecdotal statements.

This project uses an **ESP32** and an **AHT20 temperature/humidity sensor** to 
monitor room conditions in real time. The ESP32:

- Reads temperature & humidity every few seconds  
- Gets accurate timestamps using NTP (Internet time)  
- Hosts a simple **local WiFi dashboard** you can view from your phone or laptop  
- Prints time-stamped readings to the Serial Monitor for logging and graphing  

---

## Hardware used

- Expressif ESP32 Dev Board
- AHT20 Temperature/Humidity Sensor
- Male to Female Jumper wires
- USB-A to Micro-USB cable

### Wiring (I²C)

| AHT20 Pin | ESP32 Pin |
|-----------|-----------|
| VCC       | 3.3V      |
| GND       | GND       |
| SDA       | GPIO 21   |
| SCL       | GPIO 22   |

All ESP32 GND pins work.  
Make sure the sensor is powered at **3.3V only**.

---

## 🛠️ Software Setup (PlatformIO)

### 1. Install PlatformIO
Install the **PlatformIO IDE extension** inside VS Code  
(or install `platformio-core` via pip).

### 2. Clone this repository

```bash
git clone https://github.com/tangyinghao/esp32_aht20_monitor.git
cd esp32_aht20_monitor

### 3. Add your WiFi credentials (`secrets.h`)

Create the file:

```
include/secrets.h
```

Add the following:

```cpp
#pragma once
#define WIFI_SSID "YourWifiName"
#define WIFI_PASS "YourWifiPassword"
```

### 4. Build and Upload the Firmware

Upload the firmware to the ESP32:

```bash
pio run -t upload
```

Note: Some ESP32 boards require holding the BOOT button on it during upload. Release it when “Connecting…” appears. Mine required this to work whenever I needed to upload after changing any code.

### 5. Open the Serial Monitor

```bash
pio device monitor -b 115200
```

When you want to want to refresh the board, press the EN/RESET button on your ESP32.
A successful setup will show the following message in your terminal:

```
Connecting to WiFi...
Connected! IP: 192.168.x.x
AHT20 detected.
2025-12-06 02:15:30 | Temp: 28.5 °C | Humidity: 73.2 %
```

Once connected, the Serial Monitor prints a local IP address, open this URL in any browser on the same WiFi network:
IP address: 192.168.x.x

You will see a simple dashboard showing temperature & humidity, auto-refreshing every few seconds.


### 6. Access the Web Dashboard

Once connected, the Serial Monitor prints a local IP address.

```
IP address: 192.168.x.x
```

Hold ctrl and click to open this URL in any browser on the **same WiFi network**.
You will see a simple dashboard showing temperature & humidity, auto-refreshing every few seconds.

---