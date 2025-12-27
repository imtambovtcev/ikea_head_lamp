#ifndef RAINBOW_ANIMATION_H
#define RAINBOW_ANIMATION_H

#include <Arduino.h>
#include "../state/DeviceState.h"
#include "../state/DeviceConfig.h"

/**
 * Rainbow color cycling animation.
 * 
 * Cycles through hue spectrum at configurable speed.
 * Colors change fast enough to track visually.
 */
class RainbowAnimation {
public:
  RainbowAnimation();

  void start(DeviceState* state, DeviceConfig* config);
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
  
  // HSV to RGB conversion
  void hsvToRgb(float h, float s, float v, uint8_t& r, uint8_t& g, uint8_t& b);
};

#endif // RAINBOW_ANIMATION_H
