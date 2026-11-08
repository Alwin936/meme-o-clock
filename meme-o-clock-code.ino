
#include <Arduino.h>
#include <Wire.h>
#include <ESP_I2S.h>
#include <math.h>

// =====================================================
// MEME O' CLOCK
// Arduino Nano ESP32
// =====================================================

// ---------------- PIN ASSIGNMENTS ----------------

// Buttons
const int BTN_HOUR   = D3;  // SW1
const int BTN_MINUTE = D5;  // SW2
const int BTN_SNOOZE = D2;  // SW3
const int BTN_ALARM  = D4;  // SW4

// I2C
const int I2C_SDA = A4;
const int I2C_SCL = A5;

// MAX98357A I2S
const int I2S_BCLK = D7;
const int I2S_DIN  = D8;
const int I2S_LRCLK = D9;

// APDS-9306
const int SENSOR_INT = D10;
const uint8_t SENSOR_ADDRESS = 0x52;

// ---------------- AUDIO ----------------

I2SClass I2S;

const int SAMPLE_RATE = 16000;
const int AUDIO_BUFFER_SIZE = 128;

int16_t audioBuffer[AUDIO_BUFFER_SIZE];

float audioPhase = 0.0;
bool audioReady = false;

unsigned long alarmStartedAt = 0;

// ---------------- CLOCK ----------------

// Initial time: 07:00
// Set actual time using Serial Monitor.

uint32_t totalMinutes = 7 * 60;

unsigned long lastMinuteUpdate = 0;

const unsigned long MINUTE_INTERVAL = 60000;

int getHour() {
  return (totalMinutes / 60) % 24;
}

int getMinute() {
  return totalMinutes % 60;
}

uint32_t getDay() {
  return totalMinutes / 1440;
}

// ---------------- ALARM ----------------

int alarmHour = 7;
int alarmMinute = 5;

bool alarmEnabled = false;
bool alarmActive = false;

bool snoozePending = false;

uint32_t snoozeTime = 0;

uint32_t lastAlarmDay = UINT32_MAX;

const int SNOOZE_DURATION = 5;

// ---------------- BUTTON DEBOUNCING ----------------

struct Button {
  int pin;
  bool lastReading = HIGH;
  bool stableState = HIGH;
  unsigned long lastChange = 0;
};

Button hourButton   {BTN_HOUR};
Button minuteButton {BTN_MINUTE};
Button snoozeButton {BTN_SNOOZE};
Button alarmButton  {BTN_ALARM};

const unsigned long DEBOUNCE_TIME = 35;

bool buttonPressed(Button &button) {

  bool reading = digitalRead(button.pin);

  if (reading != button.lastReading) {
    button.lastChange = millis();
    button.lastReading = reading;
  }

  if (millis() - button.lastChange >= DEBOUNCE_TIME) {

    if (reading != button.stableState) {

      button.stableState = reading;

      if (button.stableState == LOW) {
        return true;
      }
    }
  }

  return false;
}

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  // Buttons have external pull-up resistors
  pinMode(BTN_HOUR, INPUT);
  pinMode(BTN_MINUTE, INPUT);
  pinMode(BTN_SNOOZE, INPUT);
  pinMode(BTN_ALARM, INPUT);

  pinMode(SENSOR_INT, INPUT);

  // Initialize I2C
  Wire.begin(I2C_SDA, I2C_SCL);

  // Initialize audio
  I2S.setPins(
    I2S_BCLK,
    I2S_LRCLK,
    I2S_DIN
  );

  audioReady = I2S.begin(
    I2S_MODE_STD,
    SAMPLE_RATE,
    I2S_DATA_BIT_WIDTH_16BIT,
    I2S_SLOT_MODE_MONO
  );

  if (audioReady) {
    Serial.println("MAX98357A initialized!");
  } else {
    Serial.println("I2S initialization failed!");
  }

  // Initialize light sensor
  initializeLightSensor();

  lastMinuteUpdate = millis();

  Serial.println();
  Serial.println("==========================");
  Serial.println("     MEME O' CLOCK");
  Serial.println("==========================");

  printTime();
  printAlarm();

  Serial.println("Commands:");
  Serial.println("T=HH:MM  Set current time");
  Serial.println("A=HH:MM  Set alarm time");
  Serial.println("?        Show status");
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop() {

  updateClock();

  handleButtons();

  checkAlarm();

  updateAudio();

  updateLightSensor();

  handleSerial();
}

// =====================================================
// CLOCK SYSTEM
// =====================================================

void updateClock() {

  unsigned long now = millis();

  if (now - lastMinuteUpdate >= MINUTE_INTERVAL) {

    // Preserve timing without blocking
    lastMinuteUpdate += MINUTE_INTERVAL;

    totalMinutes++;

    printTime();
  }
}

void printTime() {

  Serial.printf(
    "Current time: %02d:%02d\n",
    getHour(),
    getMinute()
  );
}

// =====================================================
// BUTTON SYSTEM
// =====================================================

void handleButtons() {

  // SW1: Increase alarm hour
  if (buttonPressed(hourButton)) {

    alarmHour = (alarmHour + 1) % 24;

    printAlarm();
  }

  // SW2: Increase alarm minute
  if (buttonPressed(minuteButton)) {

    alarmMinute = (alarmMinute + 1) % 60;

    printAlarm();
  }

  // SW3: Snooze
  if (buttonPressed(snoozeButton)) {

    if (alarmActive) {

      snoozePending = true;

      snoozeTime = totalMinutes + SNOOZE_DURATION;

      alarmActive = false;

      stopAudio();

      Serial.println("SNOOZED FOR 5 MINUTES");

    } else {

      printAlarm();
    }
  }

  // SW4: Toggle alarm
  if (buttonPressed(alarmButton)) {

    alarmEnabled = !alarmEnabled;

    if (!alarmEnabled) {

      alarmActive = false;
      snoozePending = false;

      stopAudio();

      Serial.println("ALARM DISABLED");

    } else {

      Serial.println("ALARM ENABLED");
    }
  }
}

void printAlarm() {

  Serial.printf(
    "Alarm time: %02d:%02d\n",
    alarmHour,
    alarmMinute
  );
}

// =====================================================
// ALARM SYSTEM
// =====================================================

void checkAlarm() {

  uint32_t currentDay = getDay();

  // Normal alarm
  if (
    alarmEnabled &&
    !alarmActive &&
    !snoozePending &&
    getHour() == alarmHour &&
    getMinute() == alarmMinute &&
    lastAlarmDay != currentDay
  ) {

    lastAlarmDay = currentDay;

    startAlarm();
  }

  // Snooze expiration
  if (
    alarmEnabled &&
    snoozePending &&
    totalMinutes >= snoozeTime
  ) {

    snoozePending = false;

    startAlarm();
  }
}

void startAlarm() {

  alarmActive = true;

  alarmStartedAt = millis();

  Serial.println();
  Serial.println("==========================");
  Serial.println("       WAKE UP!");
  Serial.println("==========================");

  Serial.println("Alarm activated!");
}

void stopAlarm() {

  alarmActive = false;
  snoozePending = false;

  stopAudio();
}

// =====================================================
// MAX98357A AUDIO SYSTEM
// =====================================================

float getAlarmFrequency(unsigned long elapsed) {

  unsigned long position = elapsed % 1400;

  if (position < 450) {
    return 880.0;
  }

  if (position >= 550 && position < 1000) {
    return 660.0;
  }

  return 0.0;
}

void updateAudio() {

  if (!audioReady || !alarmActive) {
    return;
  }

  unsigned long elapsed = millis() - alarmStartedAt;

  float frequency = getAlarmFrequency(elapsed);

  for (int i = 0; i < AUDIO_BUFFER_SIZE; i++) {

    if (frequency == 0) {

      audioBuffer[i] = 0;

    } else {

      float sample = sin(audioPhase);

      audioBuffer[i] = (int16_t)(sample * 5000);

      audioPhase +=
        2.0 * PI * frequency / SAMPLE_RATE;

      if (audioPhase >= 2.0 * PI) {
        audioPhase -= 2.0 * PI;
      }
    }
  }

  I2S.write(
    (uint8_t*)audioBuffer,
    sizeof(audioBuffer)
  );
}

void stopAudio() {

  if (!audioReady) {
    return;
  }

  memset(audioBuffer, 0, sizeof(audioBuffer));

  I2S.write(
    (uint8_t*)audioBuffer,
    sizeof(audioBuffer)
  );
}

// =====================================================
// APDS-9306 LIGHT SENSOR
// =====================================================

bool lightSensorAvailable = false;

unsigned long lastSensorRead = 0;

const unsigned long SENSOR_INTERVAL = 1000;

uint32_t lightReading = 0;

bool writeSensorRegister(uint8_t reg, uint8_t value) {

  Wire.beginTransmission(SENSOR_ADDRESS);

  Wire.write(reg);
  Wire.write(value);

  return Wire.endTransmission() == 0;
}

bool readSensorRegister(uint8_t reg, uint8_t &value) {

  Wire.beginTransmission(SENSOR_ADDRESS);

  Wire.write(reg);

  if (Wire.endTransmission(false) != 0) {
    return false;
  }

  if (Wire.requestFrom(SENSOR_ADDRESS, (uint8_t)1) != 1) {
    return false;
  }

  value = Wire.read();

  return true;
}

void initializeLightSensor() {

  uint8_t deviceID = 0;

  if (!readSensorRegister(0x06, deviceID)) {

    Serial.println("APDS-9306 not detected!");

    return;
  }

  Serial.printf(
    "APDS-9306 detected. Device ID: 0x%02X\n",
    deviceID
  );

  // 18-bit resolution, 100ms measurement rate
  writeSensorRegister(0x04, 0x22);

  // Gain setting: 3x
  writeSensorRegister(0x05, 0x01);

  // Enable ambient light sensing
  writeSensorRegister(0x00, 0x02);

  lightSensorAvailable = true;

  Serial.println("Light sensor initialized!");
}

void updateLightSensor() {

  if (!lightSensorAvailable) {
    return;
  }

  if (millis() - lastSensorRead < SENSOR_INTERVAL) {
    return;
  }

  lastSensorRead = millis();

  uint8_t status = 0;

  if (!readSensorRegister(0x07, status)) {
    return;
  }

  // Bit 3 indicates new ALS data
  if (!(status & 0x08)) {
    return;
  }

  Wire.beginTransmission(SENSOR_ADDRESS);

  Wire.write(0x0D);

  if (Wire.endTransmission(false) != 0) {
    return;
  }

  if (Wire.requestFrom(SENSOR_ADDRESS, (uint8_t)3) != 3) {
    return;
  }

  uint32_t low = Wire.read();
  uint32_t middle = Wire.read();
  uint32_t high = Wire.read();

  lightReading =
    low |
    (middle << 8) |
    ((high & 0x0F) << 16);

  Serial.printf(
    "Ambient light raw reading: %lu\n",
    (unsigned long)lightReading
  );
}

// =====================================================
// SERIAL COMMANDS
// =====================================================

char serialBuffer[32];
int serialIndex = 0;

void handleSerial() {

  while (Serial.available()) {

    char c = Serial.read();

    if (c == '\n' || c == '\r') {

      if (serialIndex > 0) {

        serialBuffer[serialIndex] = '\0';

        processCommand(serialBuffer);

        serialIndex = 0;
      }

    } else if (serialIndex < 31) {

      serialBuffer[serialIndex++] = c;
    }
  }
}

void processCommand(char *command) {

  if (strcmp(command, "?") == 0) {

    printTime();
    printAlarm();

    Serial.printf(
      "Alarm enabled: %s\n",
      alarmEnabled ? "YES" : "NO"
    );

    Serial.printf(
      "Alarm active: %s\n",
      alarmActive ? "YES" : "NO"
    );

    return;
  }

  int h, m;

  // Set current time: T=07:30
  if (
    sscanf(command, "T=%d:%d", &h, &m) == 2 &&
    h >= 0 && h < 24 &&
    m >= 0 && m < 60
  ) {

    totalMinutes = h * 60 + m;

    lastMinuteUpdate = millis();

    lastAlarmDay = UINT32_MAX;
    snoozePending = false;

    Serial.println("Clock updated!");

    printTime();

    return;
  }

  // Set alarm time: A=08:15
  if (
    sscanf(command, "A=%d:%d", &h, &m) == 2 &&
    h >= 0 && h < 24 &&
    m >= 0 && m < 60
  ) {

    alarmHour = h;
    alarmMinute = m;

    Serial.println("Alarm updated!");

    printAlarm();

    return;
  }

  Serial.println("Invalid command.");
}
