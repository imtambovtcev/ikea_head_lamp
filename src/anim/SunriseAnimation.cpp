#include "SunriseAnimation.h"

SunriseAnimation::SunriseAnimation() 
  : active(false), paused(false), startMillis(0), pausedOffset(0), lastUpdateTime(0),
    durationMs(0), targetBrightness(100), targetR(255), targetG(255), targetB(255) {
}

void SunriseAnimation::start(DeviceState* state, DeviceConfig* config,
                              uint8_t durationMinutes, uint8_t targetBri,
                              uint8_t targetRed, uint8_t targetGreen, uint8_t targetBlue) {
  Serial.println("[ANIM] Starting sunrise");
  
  active = true;
  paused = false;
  startMillis = millis();
  pausedOffset = 0;

  // Use provided parameters or fall back to config
  uint8_t duration = (durationMinutes > 0) ? durationMinutes : config->sunriseMinutes;
  targetBrightness = (targetBri > 0) ? targetBri : config->sunriseFinalBrightness;
  targetR = (targetRed > 0 || targetGreen > 0 || targetBlue > 0) ? targetRed : config->defaultColorR;
  targetG = (targetGreen > 0 || targetRed > 0 || targetBlue > 0) ? targetGreen : config->defaultColorG;
  targetB = (targetBlue > 0 || targetRed > 0 || targetGreen > 0) ? targetBlue : config->defaultColorB;
  
  // Capture duration at start time
  durationMs = (unsigned long)duration * 60UL * 1000UL;
  if (durationMs < 1000UL) durationMs = 1000UL;

  state->setAnimationMode("sunrise");
  state->powerOn = true;
  state->brightness = 1;  // Start from very dim
  state->colorR = targetR;
  state->colorG = targetG;
  state->colorB = targetB;
  state->progress = 0;
  
  // Store animation parameters in state for MQTT
  state->animDurationMinutes = duration;
  state->animFinalBrightness = targetBrightness;
  state->animFinalR = targetR;
  state->animFinalG = targetG;
  state->animFinalB = targetB;
  state->animEndBehavior = "static";  // Sunrise ends in constant light
  state->bumpVersion();  // Trigger MQTT publish
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
    // Pausing - save elapsed time so far
    paused = true;
    pausedOffset = millis() - startMillis;  // How much time has elapsed
    state->animationPaused = true;
    state->bumpVersion();
    Serial.println("[ANIM] Sunrise paused");
  } else if (!shouldPause && paused) {
    // Resuming - adjust start time to maintain elapsed time
    paused = false;
    startMillis = millis() - pausedOffset;  // Resume from where we paused
    pausedOffset = 0;  // Clear pause offset
    state->animationPaused = false;
    state->bumpVersion();
    Serial.println("[ANIM] Sunrise resumed");
  }
}

bool SunriseAnimation::update(DeviceState* state, DeviceConfig* config) {
  if (!active || paused) return false;

  unsigned long now = millis();
  
  // Throttle updates to reduce CPU load (update every 100ms)
  if ((now - lastUpdateTime) < 100) {
    return false;
  }
  lastUpdateTime = now;
  
  // Calculate elapsed time (pausedOffset is 0 when running, only used during pause)
  unsigned long elapsed = now - startMillis;

  // Use captured duration from start time
  float progress = (float)elapsed / (float)durationMs;
  if (progress >= 1.0f) {
    progress = 1.0f;
  }

  uint8_t progressPct = (uint8_t)round(progress * 100.0f);
  if (progressPct != state->progress) {
    state->progress = progressPct;
    state->bumpVersion();
  }

  // Brightness ramp: from 1% to captured target brightness
  float startBri = 1.0f;
  float endBri = (float)targetBrightness;
  float logicalBri = startBri + (endBri - startBri) * progress;

  if (logicalBri > 100.0f) logicalBri = 100.0f;
  if (logicalBri < 1.0f)   logicalBri = 1.0f;

  uint8_t newBrightness = (uint8_t)round(logicalBri);
  if (newBrightness != state->brightness) {
    state->brightness = newBrightness;
    state->bumpVersion();  // Trigger hardware update
  }
  state->powerOn = true;

  // Check if complete
  if (progress >= 1.0f) {
    // Ensure final state is set before stopping
    state->brightness = targetBrightness;
    state->progress = 100;
    state->powerOn = true;
    state->bumpVersion();
    
    // Transition to static mode with current color/brightness
    Serial.println("[ANIM] Sunrise complete - transitioning to static mode");
    active = false;
    paused = false;
    state->setStaticMode();
    
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
