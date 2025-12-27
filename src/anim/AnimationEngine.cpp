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
  
  if (sunset.isActive()) {
    sunset.update(state, config);
  }
  
  if (rainbow.isActive()) {
    rainbow.update(state, config);
  }
  
  if (fire.isActive()) {
    fire.update(state, config);
  }
  
  if (breathe.isActive()) {
    breathe.update(state, config);
  }
  
  if (ocean.isActive()) {
    ocean.update(state, config);
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

void AnimationEngine::startFire(uint8_t intensity, uint8_t speed) {
  if (!state || !config) return;
  
  // Stop any active animation first
  stop();
  
  fire.start(state, config, intensity, speed);
}

void AnimationEngine::startBreathe(uint8_t cycleDuration, uint8_t maxBrightness, 
                                    uint8_t minBrightness, uint8_t targetR, 
                                    uint8_t targetG, uint8_t targetB) {
  if (!state || !config) return;
  
  // Stop any active animation first
  stop();
  
  breathe.start(state, config, cycleDuration, maxBrightness, minBrightness, 
                targetR, targetG, targetB);
}

void AnimationEngine::startSunset(uint8_t durationMinutes, uint8_t finalBrightness) {
  if (!state || !config) return;
  
  // Stop any active animation first
  stop();
  
  sunset.start(state, config, durationMinutes, finalBrightness);
}

void AnimationEngine::startOcean(uint8_t speed, uint8_t brightness) {
  if (!state || !config) return;
  
  // Stop any active animation first
  stop();
  
  ocean.start(state, config, speed, brightness);
}

void AnimationEngine::startFavorite() {
  if (!state || !config) return;
  
  // Stop any active animation first
  stop();
  
  // Start animation based on favoriteAnimation name
  String anim = config->favoriteAnimation;
  
  if (anim == "sunrise") {
    startSunrise(config->favAnimParam1, config->favAnimParam2, 
                 config->favAnimColorR, config->favAnimColorG, config->favAnimColorB);
  } else if (anim == "sunset") {
    startSunset(config->favAnimParam1, config->favAnimParam2);
  } else if (anim == "fire") {
    startFire(config->favAnimParam1, config->favAnimParam2);
  } else if (anim == "breathe") {
    startBreathe(config->favAnimParam1, config->favAnimParam2, config->favAnimParam3,
                 config->favAnimColorR, config->favAnimColorG, config->favAnimColorB);
  } else if (anim == "ocean") {
    startOcean(config->favAnimParam1, config->favAnimParam2);
  } else if (anim == "rainbow") {
    startRainbow();
  } else {
    // Default to fire if unknown
    startFire(70, 5);
  }
}

void AnimationEngine::stop() {
  if (!state) return;
  
  if (sunrise.isActive()) {
    sunrise.stop(state);
  }
  
  if (sunset.isActive()) {
    sunset.stop(state);
  }
  
  if (rainbow.isActive()) {
    rainbow.stop(state);
  }
  
  if (fire.isActive()) {
    fire.stop(state);
  }
  
  if (breathe.isActive()) {
    breathe.stop(state);
  }
  
  if (ocean.isActive()) {
    ocean.stop(state);
  }
}

void AnimationEngine::setPaused(bool paused) {
  if (!state) return;
  
  if (sunrise.isActive()) {
    sunrise.setPaused(paused, state);
  }
  
  if (sunset.isActive()) {
    sunset.setPaused(paused, state);
  }
  
  if (rainbow.isActive()) {
    rainbow.setPaused(paused, state);
  }
  
  if (fire.isActive()) {
    fire.setPaused(paused, state);
  }
  
  if (breathe.isActive()) {
    breathe.setPaused(paused, state);
  }
  
  if (ocean.isActive()) {
    ocean.setPaused(paused, state);
  }
}

void AnimationEngine::togglePause() {
  if (!state) return;
  
  // Toggle the pause state
  setPaused(!state->animationPaused);
}

bool AnimationEngine::isActive() const {
  return sunrise.isActive() || sunset.isActive() || rainbow.isActive() || 
         fire.isActive() || breathe.isActive() || ocean.isActive();
}
