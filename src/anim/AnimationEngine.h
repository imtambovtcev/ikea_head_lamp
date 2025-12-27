#ifndef ANIMATION_ENGINE_H
#define ANIMATION_ENGINE_H

#include <Arduino.h>
#include "SunriseAnimation.h"
#include "../state/DeviceState.h"
#include "../state/DeviceConfig.h"

/**
 * Animation engine coordinator.
 * 
 * Responsibilities:
 * - Manage active animation
 * - Route animation commands
 * - Update animation state each loop
 */
class AnimationEngine {
public:
  AnimationEngine();

  /**
   * Initialize animation engine.
   * 
   * @param state Pointer to device state
   * @param config Pointer to device config
   */
  void begin(DeviceState* state, DeviceConfig* config);

  /**
   * Update active animation. Call every loop iteration.
   */
  void loop();

  /**
   * Start sunrise animation.
   */
  void startSunrise();

  /**
   * Stop any active animation.
   */
  void stop();

  /**
   * Pause or resume active animation.
   * 
   * @param paused True to pause, false to resume
   */
  void setPaused(bool paused);

  /**
   * Check if any animation is active.
   */
  bool isActive() const;

private:
  DeviceState* state;
  DeviceConfig* config;
  SunriseAnimation sunrise;
};

#endif // ANIMATION_ENGINE_H
