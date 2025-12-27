#include "DeviceState.h"

DeviceState::DeviceState() 
  : powerOn(false),
    mode(LampMode::STATIC),
    colorR(255),
    colorG(200),
    colorB(160),
    brightness(70),
    animationPaused(false),
    animationName(""),
    progress(0),
    sessionId(1),
    version(0) {
}

void DeviceState::bumpVersion() {
  version++;
}

void DeviceState::togglePower() {
  powerOn = !powerOn;
  bumpVersion();
}

void DeviceState::setStaticMode() {
  mode = LampMode::STATIC;
  animationName = "";
  animationPaused = false;
  progress = 0;
  bumpVersion();
}

void DeviceState::setAnimationMode(const String& animName) {
  mode = LampMode::ANIMATION;
  animationName = animName;
  animationPaused = false;
  progress = 0;
  bumpVersion();
}
