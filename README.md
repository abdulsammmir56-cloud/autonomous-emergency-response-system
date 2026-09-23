# 🛡️ Autonomous Emergency Response System

An ESP32-based multi-sensor emergency monitoring and response system designed to detect abnormal environmental conditions, classify risk levels, and automatically trigger visual, audible, and mechanical responses.

The system combines **gas detection, temperature/humidity monitoring, light-level sensing, motion detection, and distance measurement** into a single embedded control system.

> **Simulation project:** This system is developed and tested in Wokwi. The thresholds used for gas, temperature, and light detection are simulation values and are not certified real-world safety limits.

---

## 🚀 Project Overview

The **Autonomous Emergency Response System** continuously monitors its environment using multiple sensors connected to an ESP32.

Sensor readings are processed by a risk-classification algorithm that places the system into one of four operating states:

```text
SAFE → WARNING → CRITICAL → EMERGENCY
```

Each state produces a different response using:

* 🟢 Green LED
* 🟡 Yellow LED
* 🔴 Red LED
* 🔊 Buzzer
* ⚙️ Servo motor
* 📟 OLED display
* 💻 Serial monitoring

The project demonstrates concepts used in **embedded systems, sensor fusion, automation, state-machine design, and IoT-oriented monitoring systems**.

---

## ✨ Key Features

* Multi-sensor environmental monitoring
* Gas-level detection using MQ-2
* Temperature and humidity monitoring using DHT22
* Light-level monitoring using a photoresistor
* Motion detection using PIR
* Distance measurement using HC-SR04
* Four-level emergency state machine
* Automatic servo response
* Multi-level LED status indication
* Audible emergency alerts
* Real-time OLED dashboard
* Serial Monitor diagnostics
* Sensor fault handling
* Modular C++ program structure
* Wokwi-based simulation and testing

---

## 🧠 System Architecture

```text
                  ┌─────────────────────┐
                  │       Sensors       │
                  ├─────────────────────┤
                  │ DHT22               │
                  │ MQ-2                │
                  │ Photoresistor       │
                  │ PIR                 │
                  │ HC-SR04             │
                  └──────────┬──────────┘
                             │
                             ▼
                  ┌─────────────────────┐
                  │        ESP32        │
                  │                     │
                  │ Sensor Processing   │
                  │ Risk Classification │
                  │ State Machine       │
                  └──────────┬──────────┘
                             │
              ┌──────────────┼──────────────┐
              ▼              ▼              ▼
       ┌────────────┐ ┌────────────┐ ┌────────────┐
       │   Alerts   │ │   Display  │ │ Actuation  │
       │ LEDs/Buzzer│ │   OLED     │ │   Servo    │
       └────────────┘ └────────────┘ └────────────┘
```

---

## 🔄 Emergency State Machine

The system classifies sensor conditions into four states.

| State        | Condition                                 | Response                                |
| ------------ | ----------------------------------------- | --------------------------------------- |
| 🟢 SAFE      | Normal conditions                         | Green LED, servo at 0°, buzzer off      |
| 🟡 WARNING   | Elevated temperature or nearby object     | Yellow LED, servo at 45°, warning tone  |
| 🔴 CRITICAL  | High gas or possible fire/light condition | Red LED, servo at 90°, alarm tone       |
| 🚨 EMERGENCY | Multiple severe conditions detected       | Red LED, servo at 90°, continuous alarm |

### State Flow

```text
             Normal
                │
                ▼
             ┌──────┐
             │ SAFE │
             └──┬───┘
                │
        Abnormal condition
                │
                ▼
          ┌───────────┐
          │  WARNING  │
          └─────┬─────┘
                │
          Severe condition
                │
                ▼
          ┌───────────┐
          │ CRITICAL  │
          └─────┬─────┘
                │
       Multiple severe conditions
                │
                ▼
         ┌─────────────┐
         │ EMERGENCY   │
         └─────────────┘
```

---

## 🔧 Hardware Components

| Component         | Purpose                         |
| ----------------- | ------------------------------- |
| ESP32 DevKit C V4 | Main controller                 |
| DHT22             | Temperature and humidity        |
| MQ-2 Gas Sensor   | Gas/smoke level monitoring      |
| Photoresistor     | Light/fire-condition simulation |
| PIR Sensor        | Motion detection                |
| HC-SR04           | Distance measurement            |
| SG90 Servo        | Mechanical emergency response   |
| SSD1306 OLED      | Real-time system dashboard      |
| Green LED         | Safe indication                 |
| Yellow LED        | Warning indication              |
| Red LED           | Critical/emergency indication   |
| Buzzer            | Audible alert                   |
| 220Ω Resistors    | LED current limiting            |

---

## 📌 Pin Configuration

| Device        | Signal  | ESP32 Pin |
| ------------- | ------- | --------: |
| DHT22         | DATA    |    GPIO 4 |
| MQ-2          | AOUT    |   GPIO 34 |
| Photoresistor | AO      |   GPIO 35 |
| PIR           | OUT     |   GPIO 27 |
| HC-SR04       | TRIG    |   GPIO 18 |
| HC-SR04       | ECHO    |   GPIO 19 |
| Servo         | PWM     |   GPIO 13 |
| OLED          | SDA     |   GPIO 21 |
| OLED          | SCL     |   GPIO 22 |
| Green LED     | Control |   GPIO 25 |
| Yellow LED    | Control |   GPIO 26 |
| Red LED       | Control |    GPIO 2 |
| Buzzer        | Control |    GPIO 5 |

---

## ⚙️ Risk Detection Logic

The controller evaluates multiple sensor readings during every monitoring cycle.

### Temperature

```text
Temperature ≥ 40°C
        ↓
Temperature danger condition
```

### Gas

```text
Gas ADC reading ≥ 2000
        ↓
Gas danger condition
```

### Light / Fire Simulation

```text
Light ADC reading ≥ 3000
        ↓
Possible fire/light condition
```

### Distance

```text
Distance ≤ 30 cm
        ↓
Nearby object condition
```

The final system state is determined using combinations of these conditions rather than relying on a single sensor.

---

## 📟 OLED Dashboard

The OLED continuously displays important system information:

```text
EMERGENCY RESPONSE
Temp: 27.4 C
Gas : 812
Light: 1540
Dist: 48 cm     SAFE
```

During an emergency, the status changes automatically:

```text
EMERGENCY RESPONSE
Temp: 43.2 C
Gas : 2870
Light: 3450
Dist: 18 cm     ALARM
```

---

## 💻 Serial Monitor

The ESP32 also outputs detailed diagnostic information:

```text
----------------------------------------
AUTONOMOUS EMERGENCY RESPONSE SYSTEM
----------------------------------------
Temperature : 27.4 C
Humidity    : 56.8 %
Gas Level   : 812
Light Level : 1540
Motion      : CLEAR
Distance    : 48 cm
System State: SAFE
Servo Angle : 0 degrees
----------------------------------------
```

This makes it easier to observe sensor values and verify state transitions during simulation.

---

## 🧪 Testing Scenarios

The system can be tested by changing sensor inputs in Wokwi.

### Test 1 — Normal Environment

Expected result:

```text
State: SAFE
LED: Green
Servo: 0°
Buzzer: OFF
```

### Test 2 — Elevated Temperature

Expected result:

```text
State: WARNING
LED: Yellow
Servo: 45°
Buzzer: Warning tone
```

### Test 3 — High Gas Level

Expected result:

```text
State: CRITICAL
LED: Red
Servo: 90°
Buzzer: Alarm
```

### Test 4 — Severe Combined Condition

For example:

```text
High temperature
        +
High gas level
```

Expected result:

```text
State: EMERGENCY
LED: Red
Servo: 90°
Buzzer: Continuous alarm
```

---

## 🛠️ Software Structure

The main program is organized into separate functions for easier debugging and maintenance.

```text
setup()
   │
   ├── Initialize sensors
   ├── Initialize OLED
   ├── Initialize servo
   └── Configure GPIO
        │
        ▼
loop()
   │
   ├── readAllSensors()
   │
   ├── calculateRiskLevel()
   │
   ├── controlEmergencySystem()
   │
   ├── updateDisplay()
   │
   └── printSystemStatus()
```

This structure keeps **sensor acquisition, decision logic, actuator control, and user interface functions separated**.

---

## 📚 Libraries

The project uses:

* DHT sensor library
* Adafruit SSD1306
* Adafruit GFX Library
* ESP32Servo

These dependencies are listed in `libraries.txt`.

---

## 📁 Project Structure

```text
autonomous-emergency-response-system/
│
├── diagram.json       # Wokwi circuit configuration
├── libraries.txt      # Required Arduino libraries
├── sketch.ino         # ESP32 firmware
├── LICENSE            # Project license
└── README.md          # Project documentation
```

---

## ▶️ Running the Project

### 1. Clone the repository

```bash
git clone https://github.com/abdulsammmir56-cloud/autonomous-emergency-response-system.git
```

### 2. Open the project

Open the project files in your preferred Arduino/Wokwi environment.

### 3. Install dependencies

Install the libraries listed in:

```text
libraries.txt
```

### 4. Start the simulation

Import the circuit configuration from:

```text
diagram.json
```

### 5. Upload / run

Compile and run `sketch.ino` on the ESP32 simulation.

### 6. Monitor the system

Use:

* OLED display
* LEDs
* Buzzer
* Servo
* Serial Monitor

to observe the system response.

---

## 🌐 Wokwi Simulation

The project is designed to run in the **Wokwi ESP32 simulator**.

**Wokwi project:**
*Add the project link here after creating/saving the simulation.*

---

## 📊 Current Status

**Status: Functional simulation prototype**

Implemented:

* [x] ESP32 control system
* [x] Multi-sensor monitoring
* [x] Risk classification
* [x] Emergency state machine
* [x] OLED dashboard
* [x] LED status system
* [x] Buzzer alerts
* [x] Servo actuation
* [x] Serial diagnostics
* [x] Wokwi simulation

---

## 🔮 Future Improvements

Possible next development stages include:

* IoT cloud dashboard
* Wi-Fi-based emergency notifications
* Mobile application integration
* Data logging
* Sensor calibration
* Historical event tracking
* Battery monitoring
* Watchdog-based fault recovery
* More advanced sensor-fusion algorithms
* Real flame sensor integration
* Physical hardware prototype
* MQTT communication
* Remote emergency status monitoring

---

## ⚠️ Safety & Simulation Note

This project is an **educational embedded-systems prototype**.

The Wokwi simulation uses configurable sensor values and simplified thresholds. The photoresistor is used as a **simulated light/fire-condition detector**, not as a certified flame sensor.

The system should **not be used as a real fire alarm, gas detector, or life-safety device** without appropriate certified hardware, engineering validation, calibration, redundancy, and regulatory compliance.

---

## 👨‍💻 Author

**Abdul Sammir Bashirudeen**

Embedded Systems | ESP32 | C/C++ | IoT | Automation

---

## 📄 License

This project is released under the license included in the repository.

---

⭐ If you found this project useful, feel free to explore the code, simulation, and system architecture.
