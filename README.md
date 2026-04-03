# Smart Automatic Door System (IoT)

## Description
This project is an IoT-based smart automatic door system that operates based on real-time conditions such as time schedules, distance detection, and remote control via Blynk.

## Features
- Automatic door control using ultrasonic sensor (HC-SR04)
- Servo motor for opening/closing door
- Time-based operation using RTC (DS1307)
- Remote configuration via Blynk app (set open/close time)
- LCD display for system status
- Buzzer warning system for suspicious activity
- WiFi connectivity using ESP32
- Real-time notifications via Blynk

## Hardware Components
- ESP32
- Ultrasonic Sensor (HC-SR04)
- Servo Motor
- LCD 16x2 (I2C)
- RTC DS1307
- Buzzer
- Breadboard & jumper wires

## Working Principle
- The system connects to WiFi and Blynk for remote control
- Users can set opening and closing time via Blynk app
- During daytime:
  - Door opens automatically when a person is detected
- During nighttime:
  - Door remains closed
  - If motion is detected near the door → buzzer alarm + warning message
  - Sends notification to Blynk after a delay

- LCD displays:
  - System status (OPEN / CLOSED)
  - Warning messages
  - Time information

## Demo
(Add your video link here)

## Images
![z7654149909224_ca292329f723d71b148edd85dbb5486a](https://github.com/user-attachments/assets/2f5163c1-a9eb-44fb-b796-406279c94388)
