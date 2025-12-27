#include "SunriseAnimation.h"

SunriseAnimation::SunriseAnimation() 
  : active(false), paused(false), startMillis(0), pausedOffset(0) {
}

void SunriseAnimation::start(DeviceState* state, DeviceConfig* config) {
  Serial.println("[ANIM] Starting sunrise");
  
  active = true;
  paused = false;
  startMillis = millis();
  pausedOffset = 0;

  state->setAnimationMode("sunrise");
  state->powerOn = true;
  state->brightness = 1;  // Start from very dim
  state->colorR = config->defaultColorR;
  state->colorG = config->defaultColorG;
  state->colorB = config->defaultColorB;
  state->progress = 0;
}

void SunriseAnimation::stop(DeviceState* state) {
  if (!active) return;
  
  Serial.println("[ANIM] Stopping sunrise");
  
  active = false;
  paused = false;
  state->setStaticMode();
}

void SunriseAnimation::setPaused(bool shouldPause, DeviceState* state) {
  if (!active) return;

  if (shouldPause && !paused) {
    // Pausing
    paused = true;
    pausedOffset = millis() - startMillis;
    state->animationPaused = true;
    state->bumpVersion();
    Serial.println("[ANIM] Sunrise paused");
  } else if (!shouldPause && paused) {
    // Resuming
    paused = false;
    startMillis = millis() - pausedOffset;
    state->animationPaused = false;
    state->bumpVersion();
    Serial.println("[ANIM] Sunrise resumed");
  }
}

bool SunriseAnimation::update(DeviceState* state, DeviceConfig* config) {
  if (!active || paused) return false;

  unsigned long now = millis();
  unsigned long elapsed = now - startMillis;

  unsigned long totalMs = (unsigned long)config->sunriseMinutes * 60UL * 1000UL;
  if (totalMs < 1000UL) totalMs = 1000UL;

  float progress = (float)elapsed / (float)totalMs;
  if (progress >= 1.0f) {
    progress = 1.0f;
  }

  uint8_t progressPct = (uint8_t)round(progress * 100.0f);
  if (progressPct != state->progress) {
    state->progress = progressPct;
    state->bumpVersion();
  }

  // Brightness ramp: from 1% to sunriseFinalBrightness
  float startBri = 1.0f;
  float endBri = (float)config->sunriseFinalBrightness;
  float logicalBri = startBri + (endBri - startBri) * progress;

  if (logicalBri > 100.0f) logicalBri = 100.0f;
  if (logicalBri < 1.0f)   logicalBri = 1.0f;

  state->brightness = (uint8_t)round(logicalBri);
  state->powerOn = true;

  // Check if complete
  if (progress >= 1.0f) {
    stop(state);
    return true;  // Animation complete
  }

  return false;
}

bool SunriseAnimation::isActive() const {
  return active;
}

bool SunriseAnimation::isPaused() const {
  return paused;
}
