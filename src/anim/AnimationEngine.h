#ifndef ANIMATION_ENGINE_H
#define ANIMATION_ENGINE_H

#include <Arduino.h>
#include "SunriseAnimation.h"
#include "SunsetAnimation.h"
#include "RainbowAnimation.h"
#include "FireAnimation.h"
#include "BreatheAnimation.h"
#include "OceanAnimation.h"
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
   * Start fire animation.
   * 
   * @param intensity Flicker intensity 0-100 (default: 70)
   * @param speed Flicker speed 1-10 (default: 5)
   */
  void startFire(uint8_t intensity = 70, uint8_t speed = 5);

  /**
   * Start breathe animation.
   * 
   * @param cycleDuration Seconds per breath cycle (default: 4)
   * @param maxBrightness Max brightness 0-100 (default: 70)
   * @param minBrightness Min brightness 0-100 (default: 10)
   * @param targetR Red component (0 = use current)
   * @param targetG Green component (0 = use current)
   * @param targetB Blue component (0 = use current)
   */
  void startBreathe(uint8_t cycleDuration = 4, uint8_t maxBrightness = 70, 
                    uint8_t minBrightness = 10, uint8_t targetR = 0, 
                    uint8_t targetG = 0, uint8_t targetB = 0);

  /**
   * Start sunset animation.
   * 
   * @param durationMinutes Duration in minutes (0 = use config default)
   * @param finalBrightness Final brightness 0-100 (0 = turn off)
   */
  void startSunset(uint8_t durationMinutes = 0, uint8_t finalBrightness = 0);

  /**
   * Start ocean animation.
   * 
   * @param speed Wave speed 1-10 (default: 5)
   * @param brightness Max brightness 0-100 (default: 70)
   */
  void startOcean(uint8_t speed = 5, uint8_t brightness = 70);

  /**
   * Start favorite animation from config.
   */
  void startFavorite();

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
  SunsetAnimation sunset;
  RainbowAnimation rainbow;
  FireAnimation fire;
  BreatheAnimation breathe;
  OceanAnimation ocean;
};

#endif // ANIMATION_ENGINE_H
