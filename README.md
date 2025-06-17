# Power-Monitoring-Energy-Meter-Projects-with-ESP32-MQTT-Home-Assistant
Author : Hidayatur rahman

## **Table of Contents**  
1. [Introduction](#introduction)
2. [Features](#features)
3. [Hardware Requirements](#hardware-requirements)  
4. [Firmware Setup](#firmware-setup)  
5. [Home Assistant Integration](#home-assistant-integration)  
6. [Calibration](#calibration)  
7. [Troubleshooting](#troubleshooting)  
8. [License](#license)
   
## **Introduction** <a name="introduction"></a>  
This my first documented Arduino IoT project is both exciting and challenging. I'll be building A Smart Power meter using an ESP32, Pzem for data collection, MQTT for communication, and Home Assistant for visualization. This project will help understand core IoT concepts like sensor data collection, wireless connectivity, and real-time monitoring.

## **Features** <a name="features"></a> 
1. Real-Time Energy Monitoring. Measures voltage (V), current (A), power (W), energy (kWh), frequency (Hz), and power factor with the PZEM-004T sensor.
2. Wireless Data Transmission. Wifi and MQQT : Esp32 sends data to Home Assistant without cables.
3. Energy Accumlated Function. Accumulated energy (kWh), Useful for monthly billing tracking.
   
## **Hardware Requirements** <a name="hardware-requirements"></a>  
1. Hardware Component
   - ESP32 for data processing and WiFi connectivity.
   - Pzem004t to measure: Voltage (V), Current (A), Power (W), Energy (kWh), Frequency (Hz), Power Factor.
   - Lcd 20x4 for data display
   - Push Button for select menu lcd display
   - Wire
   - Box
   - Terminal Block
   - Power Supply 220 AC to 5V Dc for Arduino and Pzem
   - PCB
2. Hardware Configuration
   - Schematics
     
      ![image](https://github.com/user-attachments/assets/406f03c4-97b0-487b-ae78-fd535504a460)
     
   - Wiring 
     
      <img src="https://github.com/user-attachments/assets/8f447a4b-0fe4-4c89-9615-2d9e7c3dad05" width="915" alt="Untitled Design">

   - Implementaion
        
## **Firmware Setup** <a name="firmware-setup"></a>
1. Arduino Code
   
