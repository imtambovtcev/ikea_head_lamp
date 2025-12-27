#include "Button.h"

Button::Button() 
  : lastStable(HIGH), lastRaw(HIGH), lastChange(0), 
    pressStartTime(0), longPressReported(false),
    lastReleaseTime(0), awaitingSecondClick(false) {
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
      // Button pressed
      if (awaitingSecondClick && (now - lastReleaseTime) < DOUBLE_CLICK_MS) {
        // Second press within double-click window - it's a double-click!
        awaitingSecondClick = false;
        longPressReported = true;  // Prevent release from triggering single press
        return ButtonEvent::DoublePress;
      }
      
      // First press or too late for double-click
      pressStartTime = now;
      longPressReported = false;
      awaitingSecondClick = false;
    } else {
      // Button released
      if (!longPressReported) {
        // Short press released - start waiting for potential second click
        lastReleaseTime = now;
        awaitingSecondClick = true;
        // Don't return Press yet - wait to see if there's a second click
      }
    }
  }

  // Check for long press (button held down)
  if (lastStable == LOW && !longPressReported) {
    if ((now - pressStartTime) >= 1000) {  // 1 second long press
      longPressReported = true;
      awaitingSecondClick = false;
      return ButtonEvent::LongPress;
    }
  }

  // Check if double-click window expired
  if (awaitingSecondClick && (now - lastReleaseTime) >= DOUBLE_CLICK_MS) {
    awaitingSecondClick = false;
    return ButtonEvent::Press;  // Single press confirmed
  }

  return ButtonEvent::None;
}
