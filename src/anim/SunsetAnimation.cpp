#include "SunsetAnimation.h"

SunsetAnimation::SunsetAnimation() 
  : active(false), paused(false), startMillis(0), pausedOffset(0),
    lastUpdateTime(0), durationMinutes(30), finalBrightness(0),
    startBrightness(100), startR(255), startG(147), startB(41) {
}

void SunsetAnimation::start(DeviceState* state, DeviceConfig* config, 
                             uint8_t durMin, uint8_t finalBri) {
  if (!state || !config) return;
  
  active = true;
  paused = false;
  startMillis = millis();
  pausedOffset = 0;
  lastUpdateTime = 0;
  
  // Use config default if not specified
  durationMinutes = (durMin == 0) ? config->sunriseMinutes : durMin;
  durationMinutes = constrain(durationMinutes, 1, 180);
  
  finalBrightness = constrain(finalBri, 0, 100);
  
  // Capture starting state
  startBrightness = state->brightness;
  startR = state->colorR;
  startG = state->colorG;
  startB = state->colorB;
  
  state->powerOn = true;
  state->setAnimationMode("sunset");
  state->animationPaused = false;
  state->progress = 0;
  
  // Store parameters in state for MQTT visibility
  state->animDurationMinutes = durationMinutes;
  state->animFinalBrightness = finalBrightness;
  state->animFinalR = 255;  // Deep red
  state->animFinalG = 80;
  state->animFinalB = 0;
  state->animEndBehavior = (finalBrightness == 0) ? "off" : "static";
  
  state->bumpVersion();
}

void SunsetAnimation::stop(DeviceState* state) {
  if (!state || !active) return;
  
  active = false;
  paused = false;
  state->setStaticMode();
  state->bumpVersion();
}

void SunsetAnimation::setPaused(bool shouldPause, DeviceState* state) {
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

bool SunsetAnimation::update(DeviceState* state, DeviceConfig* config) {
  if (!active || paused || !state || !config) return false;
  
  unsigned long now = millis();
  
  // Throttle to 10 Hz (100ms intervals)
  if (now - lastUpdateTime < 100) {
    return false;
  }
  lastUpdateTime = now;
  
  // Calculate elapsed time
  unsigned long elapsed = now - startMillis;
  unsigned long totalMillis = (unsigned long)durationMinutes * 60UL * 1000UL;
  
  if (elapsed >= totalMillis) {
    // Animation complete
    state->brightness = finalBrightness;
    state->colorR = 255;
    state->colorG = 80;
    state->colorB = 0;
    state->progress = 100;
    
    if (finalBrightness == 0) {
      state->powerOn = false;
    }
    
    state->bumpVersion();
    stop(state);
    return true;
  }
  
  // Progress (0.0 to 1.0)
  float progress = (float)elapsed / (float)totalMillis;
  state->progress = (uint8_t)(progress * 100.0f);
  
  // Reverse color temperature progression (70% of animation)
  // Start color → Warm White (255,147,41) → Orange (255,100,20) → Deep Red (255,80,0)
  float colorProgress = progress / 0.7f;
  if (colorProgress > 1.0f) colorProgress = 1.0f;
  
  // Target: Deep Red (255, 80, 0)
  uint8_t targetR = 255;
  uint8_t targetG = 80;
  uint8_t targetB = 0;
  
  // Interpolate from start color to deep red
  uint8_t r = startR + (uint8_t)((targetR - startR) * colorProgress);
  uint8_t g = startG - (uint8_t)((startG - targetG) * colorProgress);
  uint8_t b = startB - (uint8_t)((startB - targetB) * colorProgress);
  
  // Brightness dims from start to final
  uint8_t brightness = startBrightness - (uint8_t)((startBrightness - finalBrightness) * progress);
  
  state->colorR = r;
  state->colorG = g;
  state->colorB = b;
  state->brightness = brightness;
  
  state->bumpVersion();
  
  return false;
}

bool SunsetAnimation::isActive() const {
  return active;
}

bool SunsetAnimation::isPaused() const {
  return paused;
}
