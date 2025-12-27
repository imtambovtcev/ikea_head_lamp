#include "RainbowAnimation.h"

RainbowAnimation::RainbowAnimation() 
  : active(false), paused(false), startMillis(0), pausedOffset(0), lastUpdateTime(0) {
}

void RainbowAnimation::start(DeviceState* state, DeviceConfig* config) {
  active = true;
  paused = false;
  startMillis = millis();
  pausedOffset = 0;

  state->setAnimationMode("rainbow");
  state->powerOn = true;
  state->brightness = config->defaultBrightness;
  state->progress = 0;
}

void RainbowAnimation::stop(DeviceState* state) {
  if (!active) return;
  active = false;
  paused = false;
  state->setStaticMode();
}

void RainbowAnimation::setPaused(bool shouldPause, DeviceState* state) {
  if (!active) return;

  if (shouldPause && !paused) {
    paused = true;
    pausedOffset = millis() - startMillis;
    state->animationPaused = true;
    state->bumpVersion();
  } else if (!shouldPause && paused) {
    paused = false;
    startMillis = millis() - pausedOffset;
    state->animationPaused = false;
    state->bumpVersion();
  }
}

bool RainbowAnimation::update(DeviceState* state, DeviceConfig* config) {
  if (!active || paused) return false;

  unsigned long now = millis();
  
  // Throttle updates to ~60 FPS to reduce CPU load
  if ((now - lastUpdateTime) < 16) {
    return false;
  }
  lastUpdateTime = now;
  
  unsigned long elapsed = now - startMillis;

  // Complete cycle every 10 seconds (fast enough to track)
  const unsigned long CYCLE_TIME_MS = 10000;
  float progress = (float)(elapsed % CYCLE_TIME_MS) / (float)CYCLE_TIME_MS;
  
  // Hue cycles from 0 to 360 degrees
  float hue = progress * 360.0f;
  
  // Convert HSV to RGB (full saturation, brightness from state)
  uint8_t r, g, b;
  float brightness = state->brightness / 100.0f;
  hsvToRgb(hue, 1.0f, brightness, r, g, b);
  
  // Update state if color changed
  if (r != state->colorR || g != state->colorG || b != state->colorB) {
    state->colorR = r;
    state->colorG = g;
    state->colorB = b;
    // Don't bump version for every color change - would spam MQTT
  }
  
  state->powerOn = true;
  
  // Rainbow never completes - runs indefinitely
  return false;
}

bool RainbowAnimation::isActive() const {
  return active;
}

bool RainbowAnimation::isPaused() const {
  return paused;
}

void RainbowAnimation::hsvToRgb(float h, float s, float v, uint8_t& r, uint8_t& g, uint8_t& b) {
  // HSV to RGB conversion
  // h: 0-360, s: 0-1, v: 0-1
  
  float c = v * s;
  float x = c * (1.0f - fabs(fmod(h / 60.0f, 2.0f) - 1.0f));
  float m = v - c;
  
  float rp, gp, bp;
  
  if (h < 60) {
    rp = c; gp = x; bp = 0;
  } else if (h < 120) {
    rp = x; gp = c; bp = 0;
  } else if (h < 180) {
    rp = 0; gp = c; bp = x;
  } else if (h < 240) {
    rp = 0; gp = x; bp = c;
  } else if (h < 300) {
    rp = x; gp = 0; bp = c;
  } else {
    rp = c; gp = 0; bp = x;
  }
  
  r = (uint8_t)round((rp + m) * 255.0f);
  g = (uint8_t)round((gp + m) * 255.0f);
  b = (uint8_t)round((bp + m) * 255.0f);
}
