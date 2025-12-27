#ifndef BREATHE_ANIMATION_H
#define BREATHE_ANIMATION_H

#include <Arduino.h>
#include "../state/DeviceState.h"
#include "../state/DeviceConfig.h"

/**
 * Breathe/Pulse animation.
 * 
 * Smooth fade in/out at a specific color, like breathing.
 * Perfect for meditation, relaxation, or visual feedback.
 */
class BreatheAnimation {
public:
  BreatheAnimation();

  /**
   * Start breathe animation.
   * 
   * @param state Device state
   * @param config Device config
   * @param cycleDuration Seconds per complete breath cycle (default: 4)
   * @param maxBrightness Max brightness 0-100 (default: 70)
   * @param minBrightness Min brightness 0-100 (default: 10)
   * @param targetR Red component (0 = use current)
   * @param targetG Green component (0 = use current)
   * @param targetB Blue component (0 = use current)
   */
  void start(DeviceState* state, DeviceConfig* config, 
             uint8_t cycleDuration = 4, uint8_t maxBrightness = 70, 
             uint8_t minBrightness = 10, uint8_t targetR = 0, 
             uint8_t targetG = 0, uint8_t targetB = 0);
  void stop(DeviceState* state);
  void setPaused(bool shouldPause, DeviceState* state);
  
  /**
   * Update animation state.
   * @return true if animation completed, false otherwise (breathe loops indefinitely)
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
  
  uint8_t cycleDuration;   // Seconds
  uint8_t maxBrightness;
  uint8_t minBrightness;
  uint8_t targetR, targetG, targetB;
};

#endif // BREATHE_ANIMATION_H
