# ⌚ TrackWatch – ESP32 Smart Health Monitoring System

TrackWatch is a wearable IoT prototype built using ESP32 that monitors
heart rate, body temperature, and physical movement in real time.

It demonstrates how embedded systems can be used to create low-cost
health-monitoring solutions.

---

## 🚀 Features

- ❤️ Heart Rate Monitoring (MAX30102)
- 🌡️ Temperature Measurement (DS18B20)
- 🏃 Motion Tracking (MPU6050 Accelerometer)
- 📟 Real-Time OLED Display Output
- ⚡ Powered by ESP32 Microcontroller
- 🔌 I2C Sensor Integration

---

## 🛠️ Hardware Used

| Component | Purpose |
|----------|---------|
ESP32 | Main controller |
MAX30102 | Heart-rate sensor |
MPU6050 | Motion detection |
DS18B20 | Temperature sensing |
OLED Display | Data visualization |

---

## 📂 Project Structure

code/ → Arduino source code
hardware/ → Wiring & pin configuration
docs/ → Project report & presentation
media/ → Demo video/images

---

## ▶️ How to Run

1. Open `code/sketch_smart.watch1.ino` in Arduino IDE.
2. Install required libraries:
   - Adafruit GFX
   - MAX30105
   - DallasTemperature
   - MPU6050
3. Select **ESP32 Dev Module**
4. Upload the code.
5. Connect sensors as shown in documentation.

---

## 🎯 Project Goal

To design a compact, affordable wearable device capable of monitoring
basic health metrics using embedded and IoT technologies.

---

## 📸 Demo

See `/media` folder for working demonstration.

---

## 👨‍💻 Author

**Zeyad**
