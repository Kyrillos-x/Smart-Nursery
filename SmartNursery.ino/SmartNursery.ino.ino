/*
  Nursery Incubator Control System - Version kv1
  ----------------------------------------------
  Description:
  This Arduino Mega sketch controls a nursery incubator using temperature,
  humidity, baby temperature, heart rate, and sound sensors, plus manual switches.
  Outputs include a heater, fan (two speeds), buzzer, LEDs, and two I2C LCDs (16x2).

  -----------------------------
  Hardware Connections:
  -----------------------------
  Inputs:
    - DHT22 (Temp+Humidity)      : Digital Pin 2
    - LM35 (Baby Temp)           : Analog Pin A0
    - Sound Sensor (Optional)    : Analog Pin A1
    - Pulse Sensor (Heart Rate)  : Analog Pin A2
    - Manual Button              : Digital Pin 7
    - System ON/OFF Switch       : Digital Pin 8

  Outputs:
    - Heater Relay               : Digital Pin 3
    - Fan (PWM 2 speeds)         : Digital Pin 5
    - Alarm Buzzer               : Digital Pin 9
    - LED1 (System/Error)        : Digital Pin 10
    - LED2 (Heater)              : Digital Pin 11
    - LED3 (Fan)                 : Digital Pin 12
    - LCD1 Heart Rate (I2C 16x2) : Address 0x27
    - LCD2 Status (I2C 16x2)     : Address 0x3F

  -----------------------------
  Operational Logic:
  -----------------------------
  - System ON/OFF via switch
  - Heart rate read continuously
  - Other sensors read every 5 seconds
  - Heater on if ambient <36.5°C; fan low speed always, high speed >38°C
  - Errors -> flash LED1, buzzer, Serial, LCD2
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <PulseSensorPlayground.h>

// Pins
#define DHTPIN          2
#define DHTTYPE         DHT22
#define BABY_TEMP_PIN   A0
#define SOUND_PIN       A1
#define PULSE_PIN       A2
#define HEATER_PIN      3
#define FAN_PIN         5
#define BUZZER_PIN      9
#define LED_SYS         10
#define LED_HEAT        11
#define LED_FAN         12
#define BTN_MANUAL      7
#define SW_SYS          8

// Thresholds & timings
const float T_LOW = 36.5, T_HIGH = 38.0;
const float BT_MIN = 36.5, BT_MAX = 37.5;
const int P_MIN = 100, P_MAX = 160;
const int SOUND_TH = 600;
const int FAN_LOW = 85, FAN_HIGH = 255;
const unsigned long INT_S = 5000, INT_P = 3000;

// State
unsigned long lastS = 0, lastP = 0;
bool err = false;

// Objects
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd1(0x27, 16, 2);  // Heart Rate LCD
LiquidCrystal_I2C lcd2(0x3F, 16, 2);  // Status LCD
PulseSensorPlayground pulse;

void setup() {
  Serial.begin(9600);
  dht.begin();
  pulse.analogInput(PULSE_PIN);
  pulse.begin();

  lcd1.init(); lcd1.backlight();
  lcd2.init(); lcd2.backlight();

  pinMode(HEATER_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_SYS, OUTPUT);
  pinMode(LED_HEAT, OUTPUT);
  pinMode(LED_FAN, OUTPUT);
  pinMode(BTN_MANUAL, INPUT_PULLUP);
  pinMode(SW_SYS, INPUT_PULLUP);

  digitalWrite(LED_SYS, HIGH);
}

void loop() {
  unsigned long t = millis();
  bool systemOn = !digitalRead(SW_SYS);
  bool manual = !digitalRead(BTN_MANUAL);

  if (!systemOn) {
    offAll();
    return;
  }

  int bpm = pulse.getBeatsPerMinute();
  if (pulse.sawStartOfBeat()) lastP = t;
  showHeartRate(bpm);

  if (t - lastP > INT_P) reportError("Pulse", "No signal");

  if (t - lastS >= INT_S) {
    lastS = t;
    err = false;

    float envTemp = dht.readTemperature();
    float envHum = dht.readHumidity();
    float babyVolt = analogRead(BABY_TEMP_PIN) * (5.0 / 1023.0);
    float babyTemp = babyVolt * 100.0;
    int soundLvl = analogRead(SOUND_PIN);

    if (isnan(envTemp) || isnan(envHum)) reportError("DHT", "Sensor Fail");
    else if (babyTemp < BT_MIN - 5 || babyTemp > BT_MAX + 5) reportError("BabyT", "OOR");
    else if (soundLvl > SOUND_TH * 1.5) reportError("Noise", "Loud");

    if (!err) {
      digitalWrite(HEATER_PIN, manual ? HIGH : (envTemp < T_LOW));
      digitalWrite(LED_HEAT, digitalRead(HEATER_PIN));

      int fanSpeed = manual ? 0 : (envTemp > T_HIGH ? FAN_HIGH : FAN_LOW);
      analogWrite(FAN_PIN, fanSpeed);
      digitalWrite(LED_FAN, fanSpeed > 0);
      noTone(BUZZER_PIN);

      showStatus(envTemp, envHum, babyTemp);
    }
  }
}

void offAll() {
  digitalWrite(HEATER_PIN, LOW);
  analogWrite(FAN_PIN, 0);
  noTone(BUZZER_PIN);
  digitalWrite(LED_HEAT, LOW);
  digitalWrite(LED_FAN, LOW);
  digitalWrite(LED_SYS, LOW);
}

void reportError(const char* label, const char* message) {
  err = true;
  for (int i = 0; i < 6; i++) {
    digitalWrite(LED_SYS, i % 2);
    delay(100);
  }
  digitalWrite(LED_SYS, HIGH);
  Serial.print("[ERROR] "); Serial.print(label); Serial.print(": "); Serial.println(message);
  lcd2.clear();
  lcd2.print("ERR: "); lcd2.print(label);
  lcd2.setCursor(0, 1);
  lcd2.print(message);
  tone(BUZZER_PIN, 1000);
}

void showHeartRate(int bpm) {
  lcd1.clear();
  lcd1.setCursor(0, 0);
  lcd1.print("Heart Rate:");
  lcd1.setCursor(0, 1);
  lcd1.print(bpm); lcd1.print(" BPM");
}

void showStatus(float t, float h, float bt) {
  lcd2.clear();
  lcd2.setCursor(0, 0);
  lcd2.print("T:"); lcd2.print(t); lcd2.print((char)223); lcd2.print("C ");
  lcd2.print("H:"); lcd2.print(h); lcd2.print("%");
  lcd2.setCursor(0, 1);
  lcd2.print("BT:"); lcd2.print(bt); lcd2.print((char)223); lcd2.print("C");
}
