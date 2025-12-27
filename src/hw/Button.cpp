#include "Button.h"

Button::Button() 
  : lastStable(HIGH), lastRaw(HIGH), lastChange(0), 
    pressStartTime(0), longPressReported(false) {
}

void Button::begin() {
  Serial.println("[BTN] Initializing button");
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  lastRaw = lastStable = digitalRead(PIN_BUTTON);
}

ButtonEvent Button::update() {
  int raw = digitalRead(PIN_BUTTON);
  unsigned long now = millis();

  // Detect edge
  if (raw != lastRaw) {
    lastRaw = raw;
    lastChange = now;
    return ButtonEvent::None;
  }

  // Wait for debounce period
  if ((now - lastChange) < DEBOUNCE_MS) {
    return ButtonEvent::None;
  }

  // Check if stable state changed
  if (raw != lastStable) {
    lastStable = raw;

    if (raw == LOW) {
      // Button pressed - start tracking
      pressStartTime = now;
      longPressReported = false;
    } else {
      // Button released
      if (!longPressReported) {
        // Short press (released before long press threshold)
        return ButtonEvent::Press;
      }
    }
  }

  // Check for long press (button held down)
  if (lastStable == LOW && !longPressReported) {
    if ((now - pressStartTime) >= 1000) {  // 1 second long press
      longPressReported = true;
      return ButtonEvent::LongPress;
    }
  }

  return ButtonEvent::None;
}
