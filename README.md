# Smart Nursery Incubator System

A smart nursery incubator project using **Arduino Mega or Mega 2560**, designed to monitor and control environmental conditions and baby vital signs such as temperature, humidity, heart rate, and noise levels.

---

## 🔧 Hardware Used

| Component               | Description                               |
| ----------------------- | ----------------------------------------- |
| Arduino Mega            | Main microcontroller                      |
| DHT22 Sensor            | Measures ambient temperature and humidity |
| LM35 Temperature Sensor | Measures baby’s body temperature          |
| Pulse Sensor            | Measures baby's heart rate                |
| Sound Sensor (optional) | Detects high noise levels                 |
| I2C LCD Display (0x27)  | Shows heart rate                          |
| I2C LCD Display (0x3F)  | Shows environmental and system status     |
| Heater (via Relay)      | Controls incubator temperature            |
| Fan (PWM controlled)    | Controls airflow (two speeds)             |
| Buzzer                  | Alarm in case of error                    |
| LEDs                    | Status indication (system, heater, fan)   |
| Manual Button           | Allows manual heater/fan activation       |
| System Switch           | Turns the entire system ON/OFF            |

---

## ⚙️ Pin Connections

### Inputs:

* `DHT22` → **Digital Pin 2**
* `LM35`  → **Analog Pin A0**
* `Sound Sensor` → **Analog Pin A1**
* `Pulse Sensor` → **Analog Pin A2**
* `Manual Button` → **Digital Pin 7**
* `System ON/OFF Switch` → **Digital Pin 8**

### Outputs:

* `Heater Relay` → **Digital Pin 3**
* `Fan (PWM)` → **Digital Pin 5**
* `Buzzer` → **Digital Pin 9**
* `System LED` → **Digital Pin 10**
* `Heater LED` → **Digital Pin 11**
* `Fan LED` → **Digital Pin 12**
* `LCD 1 (Heart Rate)` → **I2C Address 0x27**
* `LCD 2 (Status)` → **I2C Address 0x3F**

---

## 💡 System Behavior

* **System Power**: Controlled via ON/OFF switch.
* **Heart Rate**: Continuously monitored.
* **Other Sensors**: Read every 5 seconds.
* **Heater Activation**: If ambient temp < `36.5°C`.
* **Fan Speeds**:

  * Low speed: always on if system is active.
  * High speed: activated if temp > `38°C`.
* **Manual Mode**: Overrides sensor control for heater/fan.
* **Error Handling**:

  * LCD2 shows error message
  * LED\_SYS blinks
  * Buzzer sounds
  * Serial monitor logs the error

---

## 🧪 Sensor Thresholds

| Parameter        | Normal Range         |
| ---------------- | -------------------- |
| Ambient Temp     | 36.5°C – 38.0°C      |
| Baby Temp (LM35) | 36.5°C – 37.5°C      |
| Heart Rate       | 100 – 160 BPM        |
| Sound Level      | < 600 (Analog Value) |

---

## 📷 Screenshots

*Add system photos or wiring diagrams here (optional)*

---

## ✅ Requirements

* **Arduino IDE** with libraries:

  * `DHT` sensor library
  * `LiquidCrystal_I2C`
  * `PulseSensorPlayground`

---

## 🚀 Getting Started

1. Open `SmartNursery.ino` in Arduino IDE
2. Select board: `Arduino Mega 2560`
3. Choose the correct COM port
4. Upload and observe the serial monitor & LCDs

---

## 👨‍👩‍👧 Use Cases

* Smart Baby Incubators
* Pediatric Monitoring Systems
* DIY Educational Projects

---

## 📄 License

Open-source for educational use. You’re welcome to adapt and build upon this system!

---

Developed by: *Your Name Here*

*Feel free to update this README once you upload the code to GitHub!*

