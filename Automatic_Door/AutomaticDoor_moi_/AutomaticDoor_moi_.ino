#define BLYNK_TEMPLATE_ID "TMPL6LRXPoHDg"
#define BLYNK_TEMPLATE_NAME "DU AN IOT nhom Ngợi"
#define BLYNK_AUTH_TOKEN "1IZBlSr1feMob3I1KcnjTvtyS0D6k6ma"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include "RTClib.h"

char ssid[] = "free interlet";
char pass[] = "12345678";

// 🔧 CHÂN ESP32
#define TRIG_PIN 5
#define ECHO_PIN 18
#define BUZZER_PIN 15
#define SERVO_PIN 13

Servo door;  // Create a Servo object to control the door

// initialize the library with the numbers of the interface pins
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Declare RTC DS1307 object to get real-time data
RTC_DS1307 rtc;

// Array of days of the week in English
char daysOfTheWeek[7][12] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

// Kết nối với Blynk
char auth[] = BLYNK_AUTH_TOKEN;

BlynkTimer timer;

unsigned long timePreMillis = 0;
const long timeInterval = 1000;

unsigned long warningPreMillis = 0;
const long warningInterval = 10000;

bool isOpen = false;                 // Flag to check if the pillbox is open
bool showingDate = false;            // Flag to check if the date is displayed
bool blynkNotificationSent = false;  // Flag to track if Blynk notification has been sent

int setupDayHour = 22;      // Variable to store the hour set from Blynk
int setupDayMinute = 0;     // Variable to store the minute set from Blynk
int setupNightHour = 7;     // Variable to store the hour set from Blynk
int setupNightMinute = 30;  // Variable to store the minute set from Blynk

int pos = 0;  // Variable to store the servo position
bool isWarning;
long duration;
int ultrasoniTimer;

BLYNK_WRITE(V0) {
  setupDayHour = param.asInt();  // Get the hour from Blynk
  if (setupDayHour >= 0 && setupDayHour < 24) {
    Blynk.virtualWrite(V0, setupDayHour);  // Update V0 with the new setupHour
    Serial.print("Setup Day Hour: ");
    Serial.println(setupDayHour);
  }
}

BLYNK_WRITE(V1) {
  setupDayMinute = param.asInt();  // Get the minute from Blynk
  if (setupDayMinute >= 0 && setupDayMinute < 60) {
    Blynk.virtualWrite(V1, setupDayMinute);  // Update V1 with the new setupMinute
    Serial.print("Setup Day Minute: ");
    Serial.println(setupDayMinute);
  }
}

BLYNK_WRITE(V2) {
  setupNightHour = param.asInt();  // Get the hour from Blynk
  if (setupNightHour >= 0 && setupNightHour < 24) {
    Blynk.virtualWrite(V2, setupNightHour);  // Update V2 with the new setupHour
    Serial.print("Setup night Hour: ");
    Serial.println(setupNightHour);
  }
}

BLYNK_WRITE(V3) {
  setupNightMinute = param.asInt();  // Get the minute from Blynk
  if (setupNightMinute >= 0 && setupNightMinute < 60) {
    Blynk.virtualWrite(V3, setupNightMinute);  // Update V3 with the new setupMinute
    Serial.print("Setup night Minute: ");
    Serial.println(setupNightMinute);
  }
}

void myTimerEvent() {
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, millis() / 1000);
}

void setup() {
  Serial.begin(115200);  // Initialize Serial for debugging

  Wire.begin(4, 17);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);    // Configure 2 as output for buzzer
  digitalWrite(BUZZER_PIN, LOW);  // Turn off buzzer initially
  door.attach(SERVO_PIN);
  door.write(0);

  lcd.init();       // Initialize LCD
  lcd.backlight();  // Turn on the backlight
  // WiFi.begin(ssid, pass);
  // lcd.setCursor(0, 0);  // Set cursor to start of line 1
  // lcd.print("Connecting");
  // lcd.setCursor(6, 1);  // Set cursor to start of line 2
  // lcd.print("to WiFi...");

  // while (true) {
  //   if (WiFi.status() == WL_CONNECTED) {
  //     break;
  //   } else {
  //     lcd.clear();
  //     lcd.print(F("WiFi Failed!"));  // Dùng F()
  //     Serial.println(F("WiFi Connection Failed."));
  //     delay(1000);
  //     lcd.clear();
  //     WiFi.begin(ssid, pass);
  //     lcd.setCursor(0, 0);  // Set cursor to start of line 1
  //     lcd.print("Connecting");
  //     lcd.setCursor(6, 1);  // Set cursor to start of line 2
  //     lcd.print("to WiFi...");
  //   }
  // }
// --- Phần khởi tạo LCD và bắt đầu kết nối WiFi (Giữ nguyên) ---
int count = 0;
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (count++ > 20) {  // Timeout 10s
      lcd.clear();
      lcd.print(F("WiFi Failed!"));
      Serial.println(F("WiFi Connection Failed."));
      WiFi.begin(ssid, pass);
      lcd.clear();
      lcd.setCursor(0, 0);  // Set cursor to start of line 1
      lcd.print("Connecting");
      lcd.setCursor(6, 1);  // Set cursor to start of line 2
      lcd.print("to WiFi...");
    }
  }


// --- Phần xử lý khi đã kết nối thành công ---

// Nếu code chạy đến đây, nghĩa là đã kết nối thành công
Serial.println("\nWiFi Connected!");
Serial.print("IP Address: ");
Serial.println(WiFi.localIP());

// Hiển thị thông báo và IP lên LCD
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("WiFi Connected!");
lcd.setCursor(0, 1);
lcd.print(WiFi.localIP()); // Hiển thị địa chỉ IP lên LCD
  lcd.clear();
  lcd.setCursor(5, 0);  // Set cursor to start of line 1
  lcd.print("SMART");
  lcd.setCursor(1, 1);  // Set cursor to start of line 1
  lcd.print("AUTOMATIC DOOR");
  delay(5000);

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, myTimerEvent);

  lcd.clear();

  if (!rtc.begin()) {  // If RTC is not found
    lcd.clear();
    lcd.print("Couldn't find RTC");
    Serial.println("Couldn't find RTC");
    while (1)
      delay(1000);
  }
  if (!rtc.isrunning()) {  // If RTC is not running
    lcd.clear();
    lcd.print("RTC is NOT running!");
    Serial.println("RTC is NOT running!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Set time from computer
  }

  lcd.clear();
}

void loop() {
  displayTime();  // Display time on LCD
  // displayDate();  // Display date on LCD
  DateTime now = rtc.now();

  if (now.hour() == setupNightHour && now.minute() == setupNightMinute && now.second() == 0) {
    nightMode();
  } else if (now.hour() == setupDayHour && now.minute() == setupDayMinute && now.second() == 0) {
    dayMode();
  } else {
    int currentMinutes = now.hour() * 60 + now.minute();
    int nightMins = setupNightHour * 60 + setupNightMinute;
    int dayMins = setupDayHour * 60 + setupDayMinute;
    if (currentMinutes >= dayMins && currentMinutes < nightMins) {
      dayMode();
    } else {
      nightMode();
    }
  }

  // Update Blynk
  Blynk.run();
  timer.run();
}

// Function to display time on LCD
void displayTime() {
  unsigned long currentMillis = millis();
  if (currentMillis - timePreMillis >= timeInterval) {
    timePreMillis = currentMillis;
    DateTime timeNow = rtc.now();
    lcd.setCursor(4, 1);
    // if (timeNow.hour() <= 9)
    // lcd.print("0");
    // lcd.print(timeNow.hour());
    Serial.print(timeNow.hour());

    // lcd.print(':');
    Serial.print(':');
    // if (timeNow.minute() <= 9)
    // lcd.print("0");
    // lcd.print(timeNow.minute());
    Serial.print(timeNow.minute());
    Serial.print(':');
    // lcd.print(':');
    // if (timeNow.second() <= 9)
    // lcd.print("0");
    // lcd.print(timeNow.second());
    // lcd.print(" ");
    Serial.print(timeNow.second());
    Serial.println();
  }
}

// Function to display date on LCD
void displayDate() {
  DateTime dateNow = rtc.now();
  if (!showingDate || dateNow.hour() == 0) {
    lcd.setCursor(1, 0);  // Set cursor to column 1, line 1
    lcd.print(daysOfTheWeek[dateNow.dayOfTheWeek()]);
    lcd.print(",");
    if (dateNow.day() <= 9)
      lcd.print("0");
    lcd.print(dateNow.day());
    lcd.print('/');
    if (dateNow.month() <= 9)
      lcd.print("0");
    lcd.print(dateNow.month());
    lcd.print('/');
    lcd.print(dateNow.year());
    showingDate = true;  // Set the flag to indicate date is displayed
  }
}

void sendBlynkNotification(String message) {
  // Send a notification through Blynk
  Blynk.logEvent("suspicion_warning", message.c_str());
  delay(50);                        // Delay to ensure the notification is sent
  Serial.print("Sent to Blynk: ");  // Print the message to Serial for debugging
  Serial.println(message);
}

void lcdClose() {
  lcd.setCursor(5, 0);
  lcd.print("SORRY");
  lcd.setCursor(2, 1);
  lcd.print("WE'RE CLOSED");
}

void lcdOpen() {
  lcd.setCursor(4, 0);
  lcd.print("WELCOME");
  lcd.setCursor(3, 1);
  lcd.print("WE'RE OPEN");
}

void lcdWarning() {
  lcd.setCursor(4, 0);
  lcd.print("WARNING");
  lcd.setCursor(3, 1);
  lcd.print("STAY AWAY");
}

void servoOpen() {
  pos = 0;
  door.write(pos);
  delay(500);
  isOpen = true;
}

void servoClose() {
  pos = 180;
  door.write(pos);
  delay(500);
  isOpen = false;
}

void nightMode() {
  if (isOpen) {
    servoClose();
  }
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);

  // Serial.println(distanceInch);
  if ((duration * 0.0133 / 2) < 7) {
    if (isWarning == false) {
      lcd.clear();
      warningPreMillis = millis();
      blynkNotificationSent = false;  // Reset the flag when a new warning is
    }
    isWarning = true;
    lcdWarning();
    digitalWrite(BUZZER_PIN, HIGH);
    delay(50);
    digitalWrite(BUZZER_PIN, LOW);
    ultrasoniTimer = duration * 0.0133 / 2 * 20;
    delay(ultrasoniTimer);

    if (!blynkNotificationSent && (millis() - warningPreMillis >= warningInterval)) {
      sendBlynkNotification("Warning! Suspicious activity detected near the automatic door.");
      blynkNotificationSent = true;  // Set the flag to true after sending notification
    }

  } else {
    if (isWarning) lcd.clear();
    lcdClose();
    isWarning = false;
  }
}

void dayMode() {
  lcdOpen();
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);

  if ((duration * 0.0133 / 2) < 10) {
    isOpen = true;
    servoOpen();
  } else {
    delay(2000);
    servoClose();
  }
}
