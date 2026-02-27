#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

RTC_DS1307 rtc;

#define BUZZER 9
#define VOLTAGE_PIN A0

float calibration = 5.0 / 1023.0;
bool wasConnected = false;

void beepBuzzer(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(BUZZER, LOW);
    delay(200);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED not found"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  if (!rtc.begin()) {
    Serial.println("RTC not found");
    while (1);
  }
  if (!rtc.isrunning()) {

    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  DateTime now = rtc.now();

  int sensorValue = analogRead(VOLTAGE_PIN);
  float voltage = sensorValue * calibration;

  String status = "";

  if (voltage >= 4.20 && voltage <= 4.33) {
    status = "Safe Charging";
    if (!wasConnected) {
      beepBuzzer(0); 
      wasConnected = true;
    }
  }
  else if (voltage >= 4.33 && voltage <= 4.60) {
    status = "No Device";
    if (wasConnected) {
      beepBuzzer(1);  
      wasConnected = false;
    }
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Date: ");
  display.print(now.day()); display.print("/");
  display.print(now.month()); display.print("/");
  display.print(now.year());

  display.setCursor(0, 10);
  display.print("Time: ");
  display.print(now.hour()); display.print(":");
  display.print(now.minute()); display.print(":");
  display.print(now.second());

  display.setCursor(0, 20);
  display.print("Voltage: ");
  display.print(voltage, 2);
  display.println(" V");

  display.setCursor(0, 30);
  display.print("Status: ");
  display.println(status);

  display.setCursor(0, 50);
  display.println("Safe: Data Blocked");

  display.display();

  Serial.print(now.timestamp());
  Serial.print(" | Voltage: ");
  Serial.print(voltage, 2);
  Serial.print(" V | ");
  Serial.println(status);

  delay(2000);
}
