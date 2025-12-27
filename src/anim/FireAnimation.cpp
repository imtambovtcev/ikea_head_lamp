#include "FireAnimation.h"

FireAnimation::FireAnimation() 
  : active(false), paused(false), lastUpdateTime(0), 
    intensity(70), speed(5), noiseOffset(0.0f) {
}

void FireAnimation::start(DeviceState* state, DeviceConfig* config, uint8_t intens, uint8_t spd) {
  if (!state) return;
  
  active = true;
  paused = false;
  lastUpdateTime = millis();
  noiseOffset = 0.0f;
  
  intensity = constrain(intens, 0, 100);
  speed = constrain(spd, 1, 10);
  
  state->powerOn = true;
  state->setAnimationMode("fire");
  state->animationPaused = false;
  
  // Store parameters in state for MQTT visibility
  state->animDurationMinutes = 0;  // Loops indefinitely
  state->animFinalBrightness = 0;
  state->animFinalR = 0;
  state->animFinalG = 0;
  state->animFinalB = 0;
  state->animEndBehavior = "loop";
  
  state->bumpVersion();
}

void FireAnimation::stop(DeviceState* state) {
  if (!state || !active) return;
  
  active = false;
  paused = false;
  state->setStaticMode();
  state->bumpVersion();
}

void FireAnimation::setPaused(bool shouldPause, DeviceState* state) {
  if (!state || !active) return;
  
  paused = shouldPause;
  state->animationPaused = paused;
  state->bumpVersion();
}

bool FireAnimation::update(DeviceState* state, DeviceConfig* config) {
  if (!active || paused || !state || !config) return false;
  
  unsigned long now = millis();
  
  // Throttle to ~30 FPS (33ms)
  if (now - lastUpdateTime < 33) {
    return false;
  }
  lastUpdateTime = now;
  
  // Update noise offset based on speed
  noiseOffset += 0.01f * speed;
  
  // Generate flame color with smooth random variations
  float flicker = smoothNoise(noiseOffset);
  
  // Base flame colors (warm spectrum)
  // Red: 255, Orange: 255,100,0, Yellow: 255,180,0
  float colorMix = (flicker + 1.0f) / 2.0f;  // 0.0 to 1.0
  
  // Interpolate between deep red and bright orange-yellow
  uint8_t r = 255;
  uint8_t g = (uint8_t)(80 + colorMix * 100);  // 80 to 180
  uint8_t b = 0;
  
  // Brightness flicker based on intensity parameter
  float brightnessFactor = 0.5f + (flicker * 0.5f * intensity / 100.0f);
  brightnessFactor = constrain(brightnessFactor, 0.3f, 1.0f);
  
  uint8_t brightness = (uint8_t)(70 * brightnessFactor);  // Base 70%, flicker around it
  
  state->colorR = r;
  state->colorG = g;
  state->colorB = b;
  state->brightness = brightness;
  state->progress = 0;  // Fire doesn't have progress
  
  state->bumpVersion();
  
  return false;  // Fire loops indefinitely
}

// Simple smooth noise approximation using sine waves
float FireAnimation::smoothNoise(float x) {
  // Combine multiple sine waves for organic-looking noise
  float n1 = sin(x * 1.0f) * 0.5f;
  float n2 = sin(x * 2.3f) * 0.3f;
  float n3 = sin(x * 4.7f) * 0.2f;
  return constrain(n1 + n2 + n3, -1.0f, 1.0f);
}

bool FireAnimation::isActive() const {
  return active;
}

bool FireAnimation::isPaused() const {
  return paused;
}
