# ⚡ ESP32 Power Monitoring System

A comprehensive real-time electrical power monitoring system using the **ESP32**, **PZEM-004T v3.0**, and a **20x4 I2C LCD**. Designed for integration with **Home Assistant**, **Node-RED**, or any MQTT dashboard, the system also supports **OTA updates**, **NTP time synchronization**, and **multi-core ESP32 capabilities**.

---

## 📚 Table of Contents

1. [Project Description](#project-description)
2. [Hardware Components](#hardware-components)
3. [System Features](#system-features)
4. [Hardware Connections](#hardware-connections)
5. [Software Architecture](#software-architecture)
6. [Menu System](#menu-system)
7. [Electrical Calculations](#electrical-calculations)
8. [MQTT Integration](#mqtt-integration)
9. [OTA Updates](#ota-updates)
10. [Time Synchronization (NTP)](#time-synchronization-ntp)
11. [Getting Started](#getting-started)
12. [Configuration](#configuration)
13. [Future Improvements](#future-improvements)
14. [License](#license)

---

## 📘 Project Description

This project measures and monitors AC electrical parameters like:

- Voltage (V)
- Current (A)
- Active Power (W)
- Apparent Power (VA)
- Reactive Power (VAR)
- Power Factor (CosΦ)
- Frequency (Hz)
- Energy Usage (kWh)

It displays the values on a **20x4 LCD** and sends data via **MQTT**. Three buttons allow users to navigate an on-device menu. OTA and NTP functionality are also included.

---

## 🔩 Hardware Components

| Component          | Description                                  |
|-------------------|----------------------------------------------|
| ESP32 Dev Board   | Dual-core MCU with WiFi                      |
| PZEM004Tv30       | Power monitoring module (AC only)            |
| 20x4 I2C LCD      | Data display module                          |
| 3x Push Buttons   | Menu navigation (DOWN, OK, BACK)             |
| Power Source      | 5V USB or regulator to power ESP32           |
| Capacitor (calc.) | Displayed as required for PF correction      |

---

## ⚙️ System Features

| Feature              | Description                                             |
|---------------------|---------------------------------------------------------|
| Real-Time Monitoring| Reads data every second                                 |
| Dual-Core Operation | Core 0 handles UI; Core 1 handles OTA/MQTT/NTP          |
| MQTT Publish        | Sends JSON-formatted data every 3 seconds               |
| MQTT Subscribe      | Receives daily/monthly energy updates                   |
| LCD Menu Interface  | Interactive UI using buttons and display                |
| Power Factor Calc   | Includes apparent/reactive power & capacitor estimate   |
| OTA Update Support  | Remote firmware updates over WiFi                       |
| NTP Time Sync       | Time sync using pool.ntp.org                            |

---

## 🔌 Hardware Connections

### ESP32 Wiring

| ESP32 GPIO | Function          | Connection              |
|------------|-------------------|--------------------------|
| GPIO 33    | UART RX2 (PZEM Tx)| PZEM004T Tx             |
| GPIO 32    | UART TX2 (PZEM Rx)| PZEM004T Rx             |
| GPIO 25    | BACK Button       | Tactile switch to GND   |
| GPIO 26    | DOWN Button       | Tactile switch to GND   |
| GPIO 27    | OK Button         | Tactile switch to GND   |
| GPIO 21/22 | I2C SDA/SCL       | LCD I2C module          |

---

## 🧠 Software Architecture

- **Main Loop (Core 0)**
  - Handles button input
  - Updates LCD menu
  - Reads data from sensor
  - Performs calculations

- **Core 1 Task**
  - Handles OTA server
  - Maintains MQTT connection
  - Publishes JSON telemetry
  - Syncs time using NTP

### Loop Timing

| Task           | Interval     |
|----------------|--------------|
| Sensor Read    | 1 second     |
| MQTT Publish   | 3 seconds    |
| Button Polling | every loop   |

---

## 🖥️ Menu System

The LCD interface provides an intuitive navigation system:

### Main Menu

1. Power Monitoring
2. Electrical Parameters
3. Energy Usage

### Page Details

#### 1. Power Monitoring
- Voltage
- Current
- Power (Active)
- Power Factor
- Frequency
- Daily Energy

#### 2. Electrical Parameters
- P (Active Power)
- Q (Reactive Power)
- S (Apparent Power)
- CosΦ
- Capacitor (µF)
- Current

#### 3. Energy Usage
- Daily kWh
- Monthly kWh
- Total Energy (from PZEM)

---

## 🧮 Electrical Calculations

| Parameter           | Formula                                               |
|---------------------|--------------------------------------------------------|
| Apparent Power (S)  | S = V × I                                              |
| Reactive Power (Q)  | Q = √(S² - P²)                                         |
| Phase Angle         | θ = acos(PF)                                           |
| Capacitor Value (µF)| C = ((I × √(1 - PF²)) / (V × 2πf)) × 10⁶               |

---

## 📡 MQTT Integration

### Published Topic: `home/sensor/allpower`
Every 3 seconds, payload:
```json
{
  "voltage": 220.1,
  "current": 1.23,
  "power": 271,
  "energykWh": 2.12,
  "frequency": 50.0,
  "pf": 0.95,
  "capacitorCalculate": 7.3,
  "apparentPower": 280.0
}
```

### Subscribed Topic: `home/sensor/energy`
Expected payload:
```json
{
  "daily_energy": 1.25,
  "monthly_energy": 34.6
}
```

---

## 🔧 OTA Updates

OTA is automatically enabled when connected to WiFi.
To update:
1. Open Arduino IDE
2. Select `ESP32_EnergyMonitoring` port (network)
3. Upload sketch via OTA

Progress is shown via Serial Monitor.

---

## 🌐 Time Synchronization (NTP)

- Syncs time from `pool.ntp.org`
- Adjusts for GMT+8 timezone
- Useful for logging or timestamp-based resets (planned)

---

## 🚀 Getting Started

1. Clone or download the repository.
2. Open the project in Arduino IDE.
3. Install the required libraries:
   - `PZEM004Tv30`
   - `LiquidCrystal_I2C`
   - `ArduinoJson`
   - `PubSubClient`
   - `ArduinoOTA`
   - `NTPClient`
4. Modify WiFi and MQTT settings in the code.
5. Upload to ESP32 via USB.
6. Open Serial Monitor for debug info.

---

## ⚙️ Configuration

Edit these variables to match your environment:
```cpp
char ssid[] = "YourWiFi";
const char* mqttServer = "192.168.1.100";
const char* mqttTopic1 = "home/sensor/allpower";
const char* mqttTopic2 = "home/sensor/energy";
```

---

## 🧱 Future Improvements

- ✅ EEPROM/SPIFFS data logging
- ✅ Midnight auto-reset of daily energy
- ✅ Web-based dashboard (ESPAsyncWebServer)
- ✅ Overcurrent or voltage alarms
- ✅ WiFiManager support

---

## 📜 License

This project is open-source under the MIT License.