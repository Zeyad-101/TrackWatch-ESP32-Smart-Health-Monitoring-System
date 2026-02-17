# 🔌 Hardware Configuration

This folder documents the physical connections used in the TrackWatch system.

---

## 🧠 Microcontroller
**ESP32 Dev Module**

All sensors communicate with the ESP32 using I2C and digital input pins.

---

## 📟 OLED Display (I2C)

| OLED Pin | ESP32 Pin |
|----------|-----------|
VCC        | 3.3V      |
GND        | GND       |
SDA        | GPIO 21   |
SCL        | GPIO 22   |

---

## ❤️ MAX30102 Heart Rate Sensor (I2C)

| MAX30102 Pin | ESP32 Pin |
|--------------|-----------|
VCC            | 3.3V      |
GND            | GND       |
SDA            | GPIO 21   |
SCL            | GPIO 22   |

(Note: Shares the same I2C bus as OLED.)

---

## 🏃 MPU6050 Accelerometer (I2C)

| MPU6050 Pin | ESP32 Pin |
|-------------|-----------|
VCC           | 3.3V      |
GND           | GND       |
SDA           | GPIO 21   |
SCL           | GPIO 22   |

---

## 🌡️ DS18B20 Temperature Sensor (OneWire)

| DS18B20 Pin | ESP32 Pin |
|-------------|-----------|
VCC           | 3.3V      |
GND           | GND       |
DATA          | GPIO 4    |

A **4.7kΩ pull-up resistor** is connected between DATA and VCC.

---

## 🔗 Communication Protocols Used

- **I2C Bus** → MAX30102, MPU6050, OLED  
- **OneWire** → DS18B20 Temperature Sensor

---

## ⚡ Power

All components are powered directly from the ESP32 3.3V rail.

---

## 🛠️ Notes

- Ensure common ground between all modules.
- Keep I2C wires short to avoid signal noise.
- Use 3.3V only — 5V may damage ESP32.
