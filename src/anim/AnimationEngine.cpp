#include "AnimationEngine.h"

AnimationEngine::AnimationEngine() 
  : state(nullptr), config(nullptr) {
}

void AnimationEngine::begin(DeviceState* s, DeviceConfig* c) {
  Serial.println("[ANIM] Initializing animation engine");
  state = s;
  config = c;
}

void AnimationEngine::loop() {
  if (!state || !config) return;

  if (sunrise.isActive()) {
    sunrise.update(state, config);
  }
  
  if (rainbow.isActive()) {
    rainbow.update(state, config);
  }
}

void AnimationEngine::startSunrise(uint8_t durationMinutes, uint8_t targetBrightness,
                                   uint8_t targetR, uint8_t targetG, uint8_t targetB) {
  if (!state || !config) return;
  
  // Stop any active animation first
  stop();
  
  sunrise.start(state, config, durationMinutes, targetBrightness, targetR, targetG, targetB);
}

void AnimationEngine::startRainbow() {
  if (!state || !config) return;
  
  // Stop any active animation first
  stop();
  
  rainbow.start(state, config);
}

void AnimationEngine::stop() {
  if (!state) return;
  
  if (sunrise.isActive()) {
    sunrise.stop(state);
  }
  
  if (rainbow.isActive()) {
    rainbow.stop(state);
  }
}

void AnimationEngine::setPaused(bool paused) {
  if (!state) return;
  
  if (sunrise.isActive()) {
    sunrise.setPaused(paused, state);
  }
  
  if (rainbow.isActive()) {
    rainbow.setPaused(paused, state);
  }
}

bool AnimationEngine::isActive() const {
  return sunrise.isActive() || rainbow.isActive();
}
