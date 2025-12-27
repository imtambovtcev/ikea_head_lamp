#ifndef LAMP_HARDWARE_H
#define LAMP_HARDWARE_H

#include <Arduino.h>

/**
 * Hardware abstraction for PWM-driven RGB LED lamp.
 * 
 * Responsibilities:
 * - Initialize PWM channels
 * - Apply brightness and color to physical LEDs
 * - Map logical brightness to physical PWM range
 */
class LampHardware {
public:
  LampHardware();

  /**
   * Initialize PWM channels and attach to pins.
   * Call once in setup().
   */
  void begin();

  /**
   * Apply state to physical LEDs.
   * 
   * @param power        True = lamp on, false = all LEDs off
   * @param brightness   Logical brightness (0-100)
   * @param r,g,b        Base RGB color values (0-255)
   * @param minPwmPercent Physical minimum PWM duty (0-100%)
   * @param maxPwmPercent Physical maximum PWM duty (0-100%)
   */
  void apply(bool power, uint8_t brightness, 
             uint8_t r, uint8_t g, uint8_t b,
             uint8_t minPwmPercent, uint8_t maxPwmPercent);

private:
  static const uint8_t PIN_RED   = 1;
  static const uint8_t PIN_GREEN = 4;
  static const uint8_t PIN_BLUE  = 3;

  static const uint8_t PWM_CHANNEL_RED   = 0;
  static const uint8_t PWM_CHANNEL_GREEN = 1;
  static const uint8_t PWM_CHANNEL_BLUE  = 2;

  static const uint16_t PWM_FREQ = 5000;  // Hz
  static const uint8_t  PWM_BITS = 8;     // 0-255 duty

  /**
   * Map logical brightness (0-100) to physical PWM percentage.
   * Accounts for minimum PWM needed to light LEDs.
   */
  float logicalToPhysical(uint8_t logical, uint8_t minPwm, uint8_t maxPwm);
};

#endif // LAMP_HARDWARE_H
