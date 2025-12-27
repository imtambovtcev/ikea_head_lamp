#include "Button.h"

Button::Button() 
  : lastStable(HIGH), lastRaw(HIGH), lastChange(0) {
}

void Button::begin() {
  Serial.println("[BTN] Initializing button");
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  lastRaw = lastStable = digitalRead(PIN_BUTTON);
}

ButtonEvent Button::update() {
  int raw = digitalRead(PIN_BUTTON);

  // Detect edge
  if (raw != lastRaw) {
    lastRaw = raw;
    lastChange = millis();
    return ButtonEvent::None;
  }

  // Wait for debounce period
  if ((millis() - lastChange) < DEBOUNCE_MS) {
    return ButtonEvent::None;
  }

  // Check if stable state changed
  if (raw != lastStable) {
    lastStable = raw;

    if (raw == LOW) {
      Serial.println("[BTN] Press detected");
      return ButtonEvent::Press;
    }
  }

  return ButtonEvent::None;
}
