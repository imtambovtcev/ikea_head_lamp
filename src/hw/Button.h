#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

/**
 * Debounced button handler.
 * 
 * Responsibilities:
 * - Hardware debouncing
 * - Generate clean button press events
 */
enum class ButtonEvent {
  None,
  Press
};

class Button {
public:
  Button();

  /**
   * Initialize button pin with internal pull-up.
   * Call once in setup().
   */
  void begin();

  /**
   * Update button state. Call every loop iteration.
   * 
   * @return ButtonEvent::Press on clean button press, ButtonEvent::None otherwise
   */
  ButtonEvent update();

private:
  static const uint8_t PIN_BUTTON = 5;
  static const unsigned long DEBOUNCE_MS = 40;

  bool lastStable;
  bool lastRaw;
  unsigned long lastChange;
};

#endif // BUTTON_H
