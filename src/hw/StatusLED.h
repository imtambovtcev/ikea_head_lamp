#ifndef STATUS_LED_H
#define STATUS_LED_H

#include <Arduino.h>

/**
 * Status LED for visual feedback.
 * 
 * Uses internal LED (GPIO 8) to indicate:
 * - Startup sequence
 * - Button presses
 * - MQTT command received
 */
class StatusLED {
public:
  StatusLED();

  /**
   * Initialize LED pin.
   */
  void begin();

  /**
   * Quick blink for visual feedback.
   * 
   * @param count Number of blinks (default 1)
   * @param delayMs Delay between blinks in ms (default 100)
   */
  void blink(uint8_t count = 1, uint16_t delayMs = 100);

  /**
   * Startup animation - multiple blinks.
   */
  void startupAnimation();

  /**
   * WiFi connecting pattern - slow pulse.
   */
  void wifiConnecting();

  /**
   * WiFi connected - 2 quick blinks.
   */
  void wifiConnected();

  /**
   * WiFi failed - 5 fast blinks.
   */
  void wifiFailed();

  /**
   * MQTT connecting pattern - medium blink.
   */
  void mqttConnecting();

  /**
   * MQTT connected - 3 quick blinks.
   */
  void mqttConnected();

  /**
   * MQTT failed - long blink + 3 short.
   */
  void mqttFailed();

  /**
   * Set LED state directly.
   */
  void set(bool on);

private:
  static const uint8_t PIN_LED = 8;
};

#endif // STATUS_LED_H
