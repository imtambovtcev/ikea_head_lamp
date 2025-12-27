#ifndef SUNRISE_ANIMATION_H
#define SUNRISE_ANIMATION_H

#include <Arduino.h>
#include "../state/DeviceState.h"
#include "../state/DeviceConfig.h"

/**
 * Sunrise animation implementation.
 * 
 * Responsibilities:
 * - Smooth brightness ramp from dim to target
 * - Progress tracking
 * - Pause/resume support
 */
class SunriseAnimation {
public:
  SunriseAnimation();

  /**
   * Start the sunrise animation.
   * 
   * @param state Device state to update
   * @param config Device config (for duration and target brightness)
   */
  void start(DeviceState* state, DeviceConfig* config);

  /**
   * Stop the animation.
   * 
   * @param state Device state to update
   */
  void stop(DeviceState* state);

  /**
   * Pause or resume the animation.
   * 
   * @param paused True to pause, false to resume
   * @param state Device state to update
   */
  void setPaused(bool paused, DeviceState* state);

  /**
   * Update animation state. Call every loop iteration.
   * 
   * @param state Device state to update
   * @param config Device config (for animation parameters)
   * @return True if animation is complete
   */
  bool update(DeviceState* state, DeviceConfig* config);

  /**
   * Check if animation is currently active.
   */
  bool isActive() const;

  /**
   * Check if animation is currently paused.
   */
  bool isPaused() const;

private:
  bool active;
  bool paused;
  unsigned long startMillis;
  unsigned long pausedOffset;  // Elapsed time when paused
};

#endif // SUNRISE_ANIMATION_H
