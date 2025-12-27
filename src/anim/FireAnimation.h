#ifndef FIRE_ANIMATION_H
#define FIRE_ANIMATION_H

#include <Arduino.h>
#include "../state/DeviceState.h"
#include "../state/DeviceConfig.h"

/**
 * Fire/Candle flickering animation.
 * 
 * Simulates flickering flames with random intensity variations
 * through warm colors (red → orange → yellow).
 */
class FireAnimation {
public:
  FireAnimation();

  /**
   * Start fire animation.
   * 
   * @param state Device state
   * @param config Device config
   * @param intensity Flicker intensity 0-100 (default: 70)
   * @param speed Flicker speed 1-10 (default: 5)
   */
  void start(DeviceState* state, DeviceConfig* config, uint8_t intensity = 70, uint8_t speed = 5);
  void stop(DeviceState* state);
  void setPaused(bool shouldPause, DeviceState* state);
  
  /**
   * Update animation state.
   * @return true if animation completed, false otherwise (fire loops indefinitely)
   */
  bool update(DeviceState* state, DeviceConfig* config);
  
  bool isActive() const;
  bool isPaused() const;

private:
  bool active;
  bool paused;
  unsigned long lastUpdateTime;
  
  uint8_t intensity;  // 0-100
  uint8_t speed;      // 1-10
  
  // Smooth flickering using Perlin-like noise approximation
  float noiseOffset;
  float smoothNoise(float x);
};

#endif // FIRE_ANIMATION_H
