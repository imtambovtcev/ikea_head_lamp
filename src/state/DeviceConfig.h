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
