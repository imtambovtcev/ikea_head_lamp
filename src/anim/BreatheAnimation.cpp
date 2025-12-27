#include "BreatheAnimation.h"

BreatheAnimation::BreatheAnimation() 
  : active(false), paused(false), startMillis(0), pausedOffset(0),
    lastUpdateTime(0), cycleDuration(4), maxBrightness(70), 
    minBrightness(10), targetR(0), targetG(0), targetB(0) {
}

void BreatheAnimation::start(DeviceState* state, DeviceConfig* config, 
                              uint8_t cycleDur, uint8_t maxBri, uint8_t minBri,
                              uint8_t r, uint8_t g, uint8_t b) {
  if (!state) return;
  
  active = true;
  paused = false;
  startMillis = millis();
  pausedOffset = 0;
  lastUpdateTime = 0;
  
  cycleDuration = constrain(cycleDur, 1, 60);
  maxBrightness = constrain(maxBri, 0, 100);
  minBrightness = constrain(minBri, 0, 100);
  
  // If color provided, use it; otherwise use current color
  if (r == 0 && g == 0 && b == 0) {
    targetR = state->colorR;
    targetG = state->colorG;
    targetB = state->colorB;
  } else {
    targetR = r;
    targetG = g;
    targetB = b;
  }
  
  state->powerOn = true;
  state->setAnimationMode("breathe");
  state->animationPaused = false;
  
  // Store parameters in state for MQTT visibility
  state->animDurationMinutes = 0;  // Loops indefinitely
  state->animFinalBrightness = maxBrightness;
  state->animFinalR = targetR;
  state->animFinalG = targetG;
  state->animFinalB = targetB;
  state->animEndBehavior = "loop";
  
  state->bumpVersion();
}

void BreatheAnimation::stop(DeviceState* state) {
  if (!state || !active) return;
  
  active = false;
  paused = false;
  state->setStaticMode();
  state->bumpVersion();
}

void BreatheAnimation::setPaused(bool shouldPause, DeviceState* state) {
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

bool BreatheAnimation::update(DeviceState* state, DeviceConfig* config) {
  if (!active || paused || !state || !config) return false;
  
  unsigned long now = millis();
  
  // Throttle to ~30 FPS (33ms)
  if (now - lastUpdateTime < 33) {
    return false;
  }
  lastUpdateTime = now;
  
  // Calculate elapsed time
  unsigned long elapsed = now - startMillis;
  unsigned long cycleMillis = (unsigned long)cycleDuration * 1000UL;
  
  // Position in cycle (0.0 to 1.0)
  float cycleProgress = (float)(elapsed % cycleMillis) / (float)cycleMillis;
  
  // Use sine wave for smooth breathing (0 to 1 to 0)
  float breatheFactor = (sin(cycleProgress * 2.0f * PI - PI / 2.0f) + 1.0f) / 2.0f;
  
  // Map to brightness range
  uint8_t brightness = minBrightness + (uint8_t)(breatheFactor * (maxBrightness - minBrightness));
  
  state->colorR = targetR;
  state->colorG = targetG;
  state->colorB = targetB;
  state->brightness = brightness;
  state->progress = (uint8_t)(cycleProgress * 100);
  
  state->bumpVersion();
  
  return false;  // Breathe loops indefinitely
}

bool BreatheAnimation::isActive() const {
  return active;
}

bool BreatheAnimation::isPaused() const {
  return paused;
}
