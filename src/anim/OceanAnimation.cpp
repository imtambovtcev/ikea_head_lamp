#include "OceanAnimation.h"

OceanAnimation::OceanAnimation() 
  : active(false), paused(false), startMillis(0), pausedOffset(0),
    lastUpdateTime(0), speed(5), maxBrightness(70), wavePhase(0.0f) {
}

void OceanAnimation::start(DeviceState* state, DeviceConfig* config, 
                            uint8_t spd, uint8_t brightness) {
  if (!state) return;
  
  active = true;
  paused = false;
  startMillis = millis();
  pausedOffset = 0;
  lastUpdateTime = 0;
  wavePhase = 0.0f;
  
  speed = constrain(spd, 1, 10);
  maxBrightness = constrain(brightness, 0, 100);
  
  state->powerOn = true;
  state->setAnimationMode("ocean");
  state->animationPaused = false;
  
  // Store parameters in state for MQTT visibility
  state->animDurationMinutes = 0;  // Loops indefinitely
  state->animFinalBrightness = maxBrightness;
  state->animFinalR = 0;
  state->animFinalG = 150;  // Representative cyan
  state->animFinalB = 200;
  state->animEndBehavior = "loop";
  
  state->bumpVersion();
}

void OceanAnimation::stop(DeviceState* state) {
  if (!state || !active) return;
  
  active = false;
  paused = false;
  state->setStaticMode();
  state->bumpVersion();
}

void OceanAnimation::setPaused(bool shouldPause, DeviceState* state) {
  if (!state || !active) return;
  
  if (shouldPause && !paused) {
    // Pause: capture current offset
    unsigned long elapsed = millis() - startMillis;
    pausedOffset = elapsed;
  } else if (!shouldPause && paused) {
    // Resume: adjust start time
    startMillis = millis() - pausedOffset;
  }
  
  paused = shouldPause;
  state->animationPaused = paused;
  state->bumpVersion();
}

bool OceanAnimation::update(DeviceState* state, DeviceConfig* config) {
  if (!active || paused || !state || !config) return false;
  
  unsigned long now = millis();
  
  // Throttle to ~30 FPS (33ms)
  if (now - lastUpdateTime < 33) {
    return false;
  }
  lastUpdateTime = now;
  
  // Update wave phase
  wavePhase += 0.005f * speed;
  if (wavePhase > 2.0f * PI) {
    wavePhase -= 2.0f * PI;
  }
  
  // Create wave effect using multiple sine waves
  float wave1 = sin(wavePhase) * 0.5f;
  float wave2 = sin(wavePhase * 1.3f + 1.0f) * 0.3f;
  float wave3 = sin(wavePhase * 0.7f + 2.0f) * 0.2f;
  float combinedWave = (wave1 + wave2 + wave3 + 1.0f) / 2.0f;  // 0.0 to 1.0
  
  // Map to ocean colors: Deep Blue → Cyan → Teal
  // Deep Blue: (0, 100, 180)
  // Cyan: (0, 180, 220)
  // Teal: (0, 200, 180)
  
  float colorPhase = combinedWave;
  
  uint8_t r = 0;
  uint8_t g, b;
  
  if (colorPhase < 0.5f) {
    // Deep Blue → Cyan
    float t = colorPhase * 2.0f;
    g = 100 + (uint8_t)(t * 80);   // 100 to 180
    b = 180 + (uint8_t)(t * 40);   // 180 to 220
  } else {
    // Cyan → Teal
    float t = (colorPhase - 0.5f) * 2.0f;
    g = 180 + (uint8_t)(t * 20);   // 180 to 200
    b = 220 - (uint8_t)(t * 40);   // 220 to 180
  }
  
  // Subtle brightness variation (wave effect)
  float brightnessFactor = 0.7f + (sin(wavePhase * 0.5f) * 0.3f);
  uint8_t brightness = (uint8_t)(maxBrightness * brightnessFactor);
  
  state->colorR = r;
  state->colorG = g;
  state->colorB = b;
  state->brightness = brightness;
  state->progress = (uint8_t)((wavePhase / (2.0f * PI)) * 100.0f);
  
  state->bumpVersion();
  
  return false;  // Ocean loops indefinitely
}

bool OceanAnimation::isActive() const {
  return active;
}

bool OceanAnimation::isPaused() const {
  return paused;
}
