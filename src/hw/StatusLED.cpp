#include "StatusLED.h"

StatusLED::StatusLED() {
}

void StatusLED::begin() {
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);  // HIGH = OFF (inverted logic)
  Serial.println("[LED] Status LED initialized on GPIO 8 (inverted)");
}

void StatusLED::blink(uint8_t count, uint16_t delayMs) {
  // Quick non-blocking blink
  for (uint8_t i = 0; i < count; i++) {
    digitalWrite(PIN_LED, LOW);   // LOW = ON (inverted)
    delayMicroseconds(delayMs * 1000);  // Non-blocking for short delays
    digitalWrite(PIN_LED, HIGH);  // HIGH = OFF (inverted)
    if (i < count - 1) {
      delayMicroseconds(delayMs * 1000);
    }
  }
}

void StatusLED::startupAnimation() {
  Serial.println("[LED] Startup animation");
  // Quick startup blinks - acceptable to block during setup
  for (uint8_t i = 0; i < 3; i++) {
    digitalWrite(PIN_LED, LOW);   // LOW = ON (inverted)
    delay(100);  // Shorter delay
    digitalWrite(PIN_LED, HIGH);  // HIGH = OFF (inverted)
    delay(100);
  }
}

void StatusLED::wifiConnecting() {
  // Single quick pulse - non-blocking
  digitalWrite(PIN_LED, LOW);   // LOW = ON (inverted)
  delayMicroseconds(50000);     // 50ms
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
  // Single quick pulse - non-blocking
  digitalWrite(PIN_LED, LOW);   // LOW = ON (inverted)
  delayMicroseconds(30000);     // 30ms
  digitalWrite(PIN_LED, HIGH);  // HIGH = OFF (inverted)
}

void StatusLED::mqttConnected() {
  Serial.println("[LED] MQTT connected");
  // Three quick blinks = MQTT success
  blink(3, 80);
}

void StatusLED::mqttFailed() {
  Serial.println("[LED] MQTT failed");
  // Quick error indication
  digitalWrite(PIN_LED, LOW);   // LOW = ON (inverted)
  delayMicroseconds(200000);    // 200ms
  digitalWrite(PIN_LED, HIGH);  // HIGH = OFF (inverted)
  delayMicroseconds(50000);
  blink(3, 40);  // Fast blinks
}

void StatusLED::set(bool on) {
  digitalWrite(PIN_LED, on ? LOW : HIGH);  // Inverted logic
}
