#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H

#include <Arduino.h>
#include <Preferences.h>

/**
 * Persistent device configuration (stored in NVS flash).
 * 
 * Responsibilities:
 * - Default lamp settings
 * - Animation parameters
 * - PWM calibration
 * - Load/save to non-volatile storage
 */
class DeviceConfig {
public:
  uint8_t  defaultBrightness;
  uint8_t  defaultColorR;
  uint8_t  defaultColorG;
  uint8_t  defaultColorB;

  uint16_t sunriseMinutes;
  uint8_t  sunriseFinalBrightness;

  uint8_t  minPwmPercent;
  uint8_t  maxPwmPercent;

  // Favorite animation (triggered by double-click)
  String   favoriteAnimation;    // "fire", "breathe", "ocean", "rainbow", etc.
  uint8_t  favAnimParam1;        // Generic param 1 (intensity, speed, duration, etc.)
  uint8_t  favAnimParam2;        // Generic param 2 (speed, brightness, etc.)
  uint8_t  favAnimParam3;        // Generic param 3 (min_brightness, etc.)
  uint8_t  favAnimColorR;        // Color R for animations that support it
  uint8_t  favAnimColorG;        // Color G
  uint8_t  favAnimColorB;        // Color B

  uint32_t version;         // Config version, increments on save

  DeviceConfig();

  /**
   * Load configuration from NVS flash.
   * Falls back to defaults if not present.
   */
  void load();

  /**
   * Save current configuration to NVS flash.
   * Increments version counter.
   */
  void save();

  /**
   * Reset configuration to built-in defaults and save.
   */
  void reset();

private:
  static const char* NVS_NAMESPACE;

  /**
   * Clamp configuration values to valid ranges.
   */
  void clampValues();
};

#endif // DEVICE_CONFIG_H
