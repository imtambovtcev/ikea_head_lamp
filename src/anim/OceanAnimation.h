#ifndef OCEAN_ANIMATION_H
#define OCEAN_ANIMATION_H

#include <Arduino.h>
#include "../state/DeviceState.h"
#include "../state/DeviceConfig.h"

/**
 * Ocean/Water wave animation.
 * 
 * Gentle waves through blue-cyan-teal spectrum with
 * smooth transitions, creating a calming aquatic effect.
 */
class OceanAnimation {
public:
  OceanAnimation();

  /**
   * Start ocean animation.
   * 
   * @param state Device state
   * @param config Device config
   * @param speed Wave speed 1-10 (default: 5)
   * @param brightness Max brightness 0-100 (default: 70)
   */
  void start(DeviceState* state, DeviceConfig* config, 
             uint8_t speed = 5, uint8_t brightness = 70);
  void stop(DeviceState* state);
  void setPaused(bool shouldPause, DeviceState* state);
  
  /**
   * Update animation state.
   * @return true if animation completed, false otherwise (ocean loops indefinitely)
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
  
  uint8_t speed;       // 1-10
  uint8_t maxBrightness;
  
  float wavePhase;
};

#endif // OCEAN_ANIMATION_H
