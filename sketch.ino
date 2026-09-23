#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>



#define DHT_PIN 4
#define DHT_TYPE DHT22

#define GAS_SENSOR_PIN 34
#define LIGHT_SENSOR_PIN 35
#define PIR_SENSOR_PIN 27

#define TRIG_PIN 18
#define ECHO_PIN 19

#define SERVO_PIN 13

#define GREEN_LED_PIN 25
#define YELLOW_LED_PIN 26
#define RED_LED_PIN 2

#define BUZZER_PIN 5

#define OLED_SDA 21
#define OLED_SCL 22

// ===================== OLED CONFIGURATION =====================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

// ===================== SENSOR OBJECTS =====================

DHT dht(DHT_PIN, DHT_TYPE);
Servo emergencyServo;

// ===================== SYSTEM LIMITS =====================

// These values are for Wokwi simulation.
// They are NOT real-world safety limits.

const float HIGH_TEMPERATURE = 40.0;
const int HIGH_GAS_LEVEL = 2000;
const int FIRE_LIGHT_LEVEL = 3000;
const int CLOSE_DISTANCE = 30;

// Servo positions
const int SAFE_POSITION = 0;
const int WARNING_POSITION = 45;
const int EMERGENCY_POSITION = 90;

// ===================== SYSTEM STATES =====================

enum SystemState {
  SAFE,
  WARNING,
  CRITICAL,
  EMERGENCY
};

SystemState currentState = SAFE;

// ===================== SENSOR VARIABLES =====================

float temperature = 0;
float humidity = 0;

int gasLevel = 0;
int lightLevel = 0;
int motionDetected = 0;

long distanceCM = 0;

// ===================== SETUP =====================

void setup() {

  Serial.begin(115200);

  // Start sensors
  dht.begin();

  // Configure pins
  pinMode(PIR_SENSOR_PIN, INPUT);

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);

  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Start servo
  emergencyServo.attach(SERVO_PIN);
  emergencyServo.write(SAFE_POSITION);

  // Start OLED
  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        OLED_ADDRESS
      )) {

    Serial.println("OLED initialization failed!");

    while (true) {
      delay(100);
    }
  }

  // Initial OLED screen
  showStartupScreen();

  delay(2000);
}

// ===================== MAIN LOOP =====================

void loop() {

  readAllSensors();

  currentState = calculateRiskLevel();

  controlEmergencySystem();

  updateDisplay();

  printSystemStatus();

  delay(1000);
}

// ===================== SENSOR READING =====================

void readAllSensors() {

  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  gasLevel = analogRead(GAS_SENSOR_PIN);

  lightLevel = analogRead(LIGHT_SENSOR_PIN);

  motionDetected = digitalRead(PIR_SENSOR_PIN);

  distanceCM = measureDistance();
}

// ===================== RISK CALCULATION =====================

SystemState calculateRiskLevel() {

  // Sensor failure
  if (isnan(temperature) || isnan(humidity)) {
    return CRITICAL;
  }

  bool temperatureDanger =
    temperature >= HIGH_TEMPERATURE;

  bool gasDanger =
    gasLevel >= HIGH_GAS_LEVEL;

  bool possibleFire =
    lightLevel >= FIRE_LIGHT_LEVEL;

  bool nearbyObject =
    distanceCM > 0 &&
    distanceCM <= CLOSE_DISTANCE;

  // ------------------------------------------------
  // EMERGENCY
  // ------------------------------------------------
  // Strong light + high temperature
  // is treated as a possible fire condition.

  if (possibleFire && temperatureDanger) {
    return EMERGENCY;
  }

  // Very high gas level
  if (gasDanger && temperatureDanger) {
    return EMERGENCY;
  }

  // ------------------------------------------------
  // CRITICAL
  // ------------------------------------------------

  if (possibleFire || gasDanger) {
    return CRITICAL;
  }

  // ------------------------------------------------
  // WARNING
  // ------------------------------------------------

  if (temperatureDanger || nearbyObject) {
    return WARNING;
  }

  // ------------------------------------------------
  // SAFE
  // ------------------------------------------------

  return SAFE;
}

// ===================== EMERGENCY CONTROL =====================

void controlEmergencySystem() {

  switch (currentState) {

    case SAFE:

      emergencyServo.write(SAFE_POSITION);

      digitalWrite(GREEN_LED_PIN, HIGH);
      digitalWrite(YELLOW_LED_PIN, LOW);
      digitalWrite(RED_LED_PIN, LOW);

      noTone(BUZZER_PIN);

      break;


    case WARNING:

      emergencyServo.write(WARNING_POSITION);

      digitalWrite(GREEN_LED_PIN, LOW);
      digitalWrite(YELLOW_LED_PIN, HIGH);
      digitalWrite(RED_LED_PIN, LOW);

      // Short warning beep
      tone(BUZZER_PIN, 700, 100);

      break;


    case CRITICAL:

      emergencyServo.write(EMERGENCY_POSITION);

      digitalWrite(GREEN_LED_PIN, LOW);
      digitalWrite(YELLOW_LED_PIN, LOW);
      digitalWrite(RED_LED_PIN, HIGH);

      // Faster alarm
      tone(BUZZER_PIN, 1000, 300);

      break;


    case EMERGENCY:

      emergencyServo.write(EMERGENCY_POSITION);

      digitalWrite(GREEN_LED_PIN, LOW);
      digitalWrite(YELLOW_LED_PIN, LOW);
      digitalWrite(RED_LED_PIN, HIGH);

      // Continuous emergency alarm
      tone(BUZZER_PIN, 1500);

      break;
  }
}

// ===================== DISTANCE MEASUREMENT =====================

long measureDistance() {

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(
    ECHO_PIN,
    HIGH,
    30000
  );

  if (duration == 0) {
    return -1;
  }

  long distance =
    duration * 0.034 / 2;

  return distance;
}

// ===================== OLED STARTUP =====================

void showStartupScreen() {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);

  display.setCursor(5, 5);
  display.println("AUTONOMOUS");

  display.setCursor(5, 18);
  display.println("EMERGENCY");

  display.setCursor(5, 31);
  display.println("RESPONSE SYSTEM");

  display.setCursor(5, 50);
  display.println("Initializing...");

  display.display();
}

// ===================== OLED DASHBOARD =====================

void updateDisplay() {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("EMERGENCY RESPONSE");

  display.setCursor(0, 12);
  display.print("Temp: ");
  display.print(temperature, 1);
  display.println(" C");

  display.setCursor(0, 23);
  display.print("Gas : ");
  display.println(gasLevel);

  display.setCursor(0, 34);
  display.print("Light: ");
  display.println(lightLevel);

  display.setCursor(0, 45);
  display.print("Dist: ");

  if (distanceCM < 0) {
    display.println("--");
  } else {
    display.print(distanceCM);
    display.println(" cm");
  }

  display.setCursor(75, 45);

  if (currentState == SAFE) {
    display.print("SAFE");
  }
  else if (currentState == WARNING) {
    display.print("WARN");
  }
  else if (currentState == CRITICAL) {
    display.print("CRIT");
  }
  else {
    display.print("ALARM");
  }

  display.display();
}

// ===================== SERIAL MONITOR =====================

void printSystemStatus() {

  Serial.println();
  Serial.println("----------------------------------------");

  Serial.println("AUTONOMOUS EMERGENCY RESPONSE SYSTEM");

  Serial.println("----------------------------------------");

  Serial.print("Temperature : ");
  Serial.print(temperature, 1);
  Serial.println(" C");

  Serial.print("Humidity    : ");
  Serial.print(humidity, 1);
  Serial.println(" %");

  Serial.print("Gas Level   : ");
  Serial.println(gasLevel);

  Serial.print("Light Level : ");
  Serial.println(lightLevel);

  Serial.print("Motion      : ");

  if (motionDetected) {
    Serial.println("DETECTED");
  } else {
    Serial.println("CLEAR");
  }

  Serial.print("Distance    : ");

  if (distanceCM < 0) {
    Serial.println("No reading");
  } else {
    Serial.print(distanceCM);
    Serial.println(" cm");
  }

  Serial.print("System State: ");

  if (currentState == SAFE) {
    Serial.println("SAFE");
  }
  else if (currentState == WARNING) {
    Serial.println("WARNING");
  }
  else if (currentState == CRITICAL) {
    Serial.println("CRITICAL");
  }
  else {
    Serial.println("EMERGENCY");
  }

  Serial.print("Servo Angle : ");

  if (currentState == SAFE) {
    Serial.println("0 degrees");
  }
  else if (currentState == WARNING) {
    Serial.println("45 degrees");
  }
  else {
    Serial.println("90 degrees");
  }

  Serial.println("----------------------------------------");
}