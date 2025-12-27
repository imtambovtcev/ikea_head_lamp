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
}

void AnimationEngine::startSunrise() {
  if (!state || !config) return;
  
  sunrise.start(state, config);
}

void AnimationEngine::stop() {
  if (!state) return;
  
  if (sunrise.isActive()) {
    sunrise.stop(state);
  }
}

void AnimationEngine::setPaused(bool paused) {
  if (!state) return;
  
  if (sunrise.isActive()) {
    sunrise.setPaused(paused, state);
  }
}

bool AnimationEngine::isActive() const {
  return sunrise.isActive();
}
