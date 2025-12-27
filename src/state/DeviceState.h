#ifndef DEVICE_STATE_H
#define DEVICE_STATE_H

#include <Arduino.h>
#include "DeviceTypes.h"

/**
 * Runtime state of the device (ephemeral, not persisted).
 * 
 * Responsibilities:
 * - Current lamp power/brightness/color
 * - Active animation status
 * - Version tracking for MQTT sync
 */
class DeviceState {
public:
  bool     powerOn;
  LampMode mode;

  uint8_t  colorR;
  uint8_t  colorG;
  uint8_t  colorB;
  uint8_t  brightness;      // Logical 0-100

  bool     animationPaused;
  String   animationName;
  uint8_t  progress;        // 0-100 for animation progress

  uint32_t sessionId;       // Unique per boot
  uint32_t version;         // Increments on each state change

  DeviceState();

  /**
   * Increment version counter.
   * Call after any state change to trigger MQTT publish.
   */
  void bumpVersion();

  /**
   * Toggle power on/off.
   */
  void togglePower();

  /**
   * Set lamp to static mode (stop animation).
   */
  void setStaticMode();

  /**
   * Set lamp to animation mode.
   */
  void setAnimationMode(const String& animName);
};

#endif // DEVICE_STATE_H
