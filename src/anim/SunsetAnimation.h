#ifndef SUNSET_ANIMATION_H
#define SUNSET_ANIMATION_H

#include <Arduino.h>
#include "../state/DeviceState.h"
#include "../state/DeviceConfig.h"

/**
 * Sunset animation - reverse of sunrise.
 * 
 * Gradually dims from current brightness through warm colors
 * (warm white → orange → red → off), perfect for bedtime routine.
 */
class SunsetAnimation {
public:
  SunsetAnimation();

  /**
   * Start sunset animation.
   * 
   * @param state Device state
   * @param config Device config
   * @param durationMinutes Duration in minutes (0 = use config default)
   * @param finalBrightness Final brightness 0-100 (0 = turn off, default)
   */
  void start(DeviceState* state, DeviceConfig* config, 
             uint8_t durationMinutes = 0, uint8_t finalBrightness = 0);
  void stop(DeviceState* state);
  void setPaused(bool shouldPause, DeviceState* state);
  
  /**
   * Update animation state.
   * @return true if animation completed, false otherwise
   */
  bool update(DeviceState* state, DeviceConfig* config);
  
  bool isActive() const;
  bool isPaused() const;

private:
  bool active;
  bool paused;
  unsigned long startMillis;
  unsigned long pausedOffset;
  unsigned long lastUpdateTime;
  
  uint8_t durationMinutes;
  uint8_t finalBrightness;
  uint8_t startBrightness;
  uint8_t startR, startG, startB;
};

#endif // SUNSET_ANIMATION_H
