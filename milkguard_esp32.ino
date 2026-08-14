#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// ---------------- PIN DEFINITIONS ----------------
#define TEMP_PIN 4
#define MQ135_PIN 34

#define RED_PIN 25
#define GREEN_PIN 26
#define BLUE_PIN 27

// ---------------- OLED ----------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ---------------- TEMPERATURE SENSOR ----------------
OneWire oneWire(TEMP_PIN);
DallasTemperature tempSensor(&oneWire);

// ---------------- THRESHOLDS ----------------
// These values are prototype calibration values.
// Adjust them after collecting your own sensor data.

float TEMP_WARNING = 8.0;
float TEMP_HIGH = 12.0;

int GAS_WARNING = 1500;
int GAS_HIGH = 2200;

// ----------------------------------------------------

void setup() {

  Serial.begin(115200);

  // Sensor initialization
  tempSensor.begin();

  // LED pins
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // MQ-135
  pinMode(MQ135_PIN, INPUT);

  // OLED initialization
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED initialization failed!");
    while (1);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(10, 10);
  display.println("MilkGuard");

  display.setTextSize(1);
  display.setCursor(20, 40);
  display.println("Initializing...");

  display.display();

  delay(2000);
}

// ----------------------------------------------------

void setLED(String status) {

  if (status == "FRESH") {

    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, LOW);

  }

  else if (status == "WARNING") {

    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(BLUE_PIN, LOW);

  }

  else {

    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
  }
}

// ----------------------------------------------------

String determineStatus(float temperature, int gasValue) {

  // High temperature or high VOC indication
  if (temperature >= TEMP_HIGH || gasValue >= GAS_HIGH) {
    return "HIGH RISK";
  }

  // Moderate temperature or VOC indication
  else if (temperature >= TEMP_WARNING ||
           gasValue >= GAS_WARNING) {
    return "WARNING";
  }

  // Normal condition
  else {
    return "FRESH";
  }
}

// ----------------------------------------------------

void displayData(float temperature, int gasValue, String status) {

  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("MILKGUARD");

  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

  // Temperature
  display.setCursor(0, 18);
  display.print("Temp: ");
  display.print(temperature, 1);
  display.println(" C");

  // Gas/VOC reading
  display.setCursor(0, 30);
  display.print("VOC:  ");
  display.println(gasValue);

  // Status
  display.setCursor(0, 45);
  display.print("STATUS: ");

  display.setTextSize(1);
  display.println(status);

  display.display();
}

// ----------------------------------------------------

void loop() {

  // -------- READ TEMPERATURE --------

  tempSensor.requestTemperatures();

  float temperature = tempSensor.getTempCByIndex(0);

  // -------- READ MQ-135 --------

  int gasValue = analogRead(MQ135_PIN);

  // -------- DETERMINE STATUS --------

  String status = determineStatus(temperature, gasValue);

  // -------- LED INDICATION --------

  setLED(status);

  // -------- OLED --------

  displayData(temperature, gasValue, status);

  // -------- SERIAL MONITOR --------

  Serial.println("--------------------------------");

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("VOC Sensor: ");
  Serial.println(gasValue);

  Serial.print("Milk Status: ");
  Serial.println(status);

  Serial.println("--------------------------------");

  delay(2000);
}
