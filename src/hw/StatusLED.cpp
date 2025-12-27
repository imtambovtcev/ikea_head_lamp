#include "StatusLED.h"

StatusLED::StatusLED() {
}

void StatusLED::begin() {
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);  // HIGH = OFF (inverted logic)
  Serial.println("[LED] Status LED initialized on GPIO 8 (inverted)");
}

void StatusLED::blink(uint8_t count, uint16_t delayMs) {
  for (uint8_t i = 0; i < count; i++) {
    digitalWrite(PIN_LED, LOW);   // LOW = ON (inverted)
    delay(delayMs);
    digitalWrite(PIN_LED, HIGH);  // HIGH = OFF (inverted)
    if (i < count - 1) {
      delay(delayMs);
    }
  }
}

void StatusLED::startupAnimation() {
  Serial.println("[LED] Startup animation");
  // Three quick blinks to indicate boot
  for (uint8_t i = 0; i < 3; i++) {
    digitalWrite(PIN_LED, LOW);   // LOW = ON (inverted)
    delay(150);
    digitalWrite(PIN_LED, HIGH);  // HIGH = OFF (inverted)
    delay(150);
  }
}

void StatusLED::wifiConnecting() {
  // Single slow blink during connection attempt
  digitalWrite(PIN_LED, LOW);   // LOW = ON (inverted)
  delay(100);
  digitalWrite(PIN_LED, HIGH);  // HIGH = OFF (inverted)
}

void StatusLED::wifiConnected() {
  Serial.println("[LED] WiFi connected");
  // Two quick blinks = WiFi success
  blink(2, 100);
}

void StatusLED::wifiFailed() {
  Serial.println("[LED] WiFi failed");
  // Five fast blinks = WiFi error
  blink(5, 80);
}

void StatusLED::mqttConnecting() {
  // Single medium blink
  digitalWrite(PIN_LED, LOW);   // LOW = ON (inverted)
  delay(50);
  digitalWrite(PIN_LED, HIGH);  // HIGH = OFF (inverted)
}

void StatusLED::mqttConnected() {
  Serial.println("[LED] MQTT connected");
  // Three quick blinks = MQTT success
  blink(3, 80);
}

void StatusLED::mqttFailed() {
  Serial.println("[LED] MQTT failed");
  // One long + three short = MQTT error
  digitalWrite(PIN_LED, LOW);   // LOW = ON (inverted)
  delay(300);
  digitalWrite(PIN_LED, HIGH);  // HIGH = OFF (inverted)
  delay(100);
  blink(3, 60);
}

void StatusLED::set(bool on) {
  digitalWrite(PIN_LED, on ? LOW : HIGH);  // Inverted logic
}
