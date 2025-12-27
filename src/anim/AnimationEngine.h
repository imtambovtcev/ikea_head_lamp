#ifndef ANIMATION_ENGINE_H
#define ANIMATION_ENGINE_H

#include <Arduino.h>
#include "SunriseAnimation.h"
#include "RainbowAnimation.h"
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
   * 
   * @param durationMinutes Duration in minutes (0 = use config default)
   * @param targetBrightness Final brightness (0 = use config default)
   * @param targetR Red component (0 = use config default)
   * @param targetG Green component (0 = use config default)
   * @param targetB Blue component (0 = use config default)
   */
  void startSunrise(uint8_t durationMinutes = 0, uint8_t targetBrightness = 0,
                    uint8_t targetR = 0, uint8_t targetG = 0, uint8_t targetB = 0);

  /**
   * Start rainbow animation.
   */
  void startRainbow();

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
   * Toggle pause state of active animation.
   */
  void togglePause();

  /**
   * Check if any animation is active.
   */
  bool isActive() const;

private:
  DeviceState* state;
  DeviceConfig* config;
  SunriseAnimation sunrise;
  RainbowAnimation rainbow;
};

#endif // ANIMATION_ENGINE_H
