#include <Arduino.h>

#include "hw/LampHardware.h"
#include "hw/Button.h"
#include "hw/StatusLED.h"
#include "state/DeviceState.h"
#include "state/DeviceConfig.h"
#include "net/WiFiManager.h"
#include "net/MqttManager.h"
#include "anim/AnimationEngine.h"

// ======================= MODULE INSTANCES ===================

LampHardware lamp;
Button button;
StatusLED statusLED;
DeviceState state;
DeviceConfig config;
WiFiManager wifi;
MqttManager mqtt;
AnimationEngine anim;

// ======================= CONFIG FLAGS =======================

bool configDirty = false;

// ======================= PERIODIC PUBLISHING ================

unsigned long lastStatePublish = 0;
const unsigned long STATE_PUBLISH_INTERVAL_MS = 1000;

// ======================= STATE CHANGE TRACKING ==============

struct LastAppliedState {
  bool powerOn = false;
  uint8_t brightness = 0;
  uint8_t r = 0, g = 0, b = 0;
  uint8_t minPwm = 0, maxPwm = 0;
  
  bool hasChanged(bool power, uint8_t bri, uint8_t red, uint8_t green, uint8_t blue, uint8_t minP, uint8_t maxP) {
    return powerOn != power || brightness != bri || r != red || g != green || b != blue || minPwm != minP || maxPwm != maxP;
  }
  
  void update(bool power, uint8_t bri, uint8_t red, uint8_t green, uint8_t blue, uint8_t minP, uint8_t maxP) {
    powerOn = power; brightness = bri; r = red; g = green; b = blue; minPwm = minP; maxPwm = maxP;
  }
};

LastAppliedState lastApplied;

// ======================= MQTT MESSAGE HANDLER ===============

void handleMqttMessage(const String& topic, const String& msg) {
  statusLED.blink(1, 30);  // Quick blink on MQTT command
  
  String lower = msg;
  lower.toLowerCase();

  // ---- Command: POWER ----
  if (topic == "ikea_lamp/cmnd/power") {
    if (lower == "toggle") {
      state.togglePower();
    } else if (lower == "on") {
      state.powerOn = true;
      state.bumpVersion();
    } else if (lower == "off") {
      state.powerOn = false;
      state.bumpVersion();
    }

    if (state.powerOn && state.brightness == 0) {
      state.brightness = config.defaultBrightness;
      state.colorR = config.defaultColorR;
      state.colorG = config.defaultColorG;
      state.colorB = config.defaultColorB;
    }

    if (!state.powerOn) {
      anim.stop();
    }

    mqtt.publishState(state, true);
    return;
  }

  // ---- Command: BRIGHTNESS ----
  if (topic == "ikea_lamp/cmnd/brightness") {
    int v = msg.toInt();
    if (v < 0) v = 0;
    if (v > 100) v = 100;
    state.brightness = (uint8_t)v;
    state.powerOn = (v > 0);
    state.bumpVersion();
    mqtt.publishState(state, true);
    return;
  }

  // ---- Command: COLOR (R,G,B) ----
  if (topic == "ikea_lamp/cmnd/color") {
    int r = 0, g = 0, b = 0;
    int firstComma  = msg.indexOf(',');
    int secondComma = msg.indexOf(',', firstComma + 1);

    if (firstComma > 0 && secondComma > firstComma) {
      r = msg.substring(0, firstComma).toInt();
      g = msg.substring(firstComma + 1, secondComma).toInt();
      b = msg.substring(secondComma + 1).toInt();

      if (r < 0) r = 0; if (r > 255) r = 255;
      if (g < 0) g = 0; if (g > 255) g = 255;
      if (b < 0) b = 0; if (b > 255) b = 255;

      state.colorR = (uint8_t)r;
      state.colorG = (uint8_t)g;
      state.colorB = (uint8_t)b;
      state.bumpVersion();

      mqtt.publishState(state, true);
    }
    return;
  }

  // ---- Command: MODE ----
  if (topic == "ikea_lamp/cmnd/mode") {
    if (lower == "static") {
      anim.stop();
      mqtt.publishState(state, true);
    } else if (lower == "animation") {
      anim.startSunrise();
      mqtt.publishState(state, true);
    }
    return;
  }

  // ---- Command: ANIMATION ----
  if (topic == "ikea_lamp/cmnd/animation") {
    if (lower == "sunrise") {
      anim.startSunrise();
      mqtt.publishState(state, true);
    } else {
      Serial.println("[CMD] Unknown animation");
    }
    return;
  }

  // ---- Command: PAUSE ----
  if (topic == "ikea_lamp/cmnd/pause") {
    if (!anim.isActive()) return;

    bool newPaused = state.animationPaused;
    if (lower == "toggle") {
      newPaused = !state.animationPaused;
    } else if (lower == "true" || lower == "1" || lower == "on") {
      newPaused = true;
    } else if (lower == "false" || lower == "0" || lower == "off") {
      newPaused = false;
    }

    anim.setPaused(newPaused);
    mqtt.publishState(state, true);
    return;
  }

  // ---- APPLY DEFAULTS ----
  if (topic == "ikea_lamp/cmnd/apply_defaults") {
    state.colorR = config.defaultColorR;
    state.colorG = config.defaultColorG;
    state.colorB = config.defaultColorB;
    state.brightness = config.defaultBrightness;
    state.powerOn = true;
    anim.stop();
    mqtt.publishState(state, true);
    return;
  }

  // ---- CONFIG: default_brightness ----
  if (topic == "ikea_lamp/config/default_brightness/set") {
    int v = msg.toInt();
    if (v < 1) v = 1;
    if (v > 100) v = 100;
    config.defaultBrightness = (uint8_t)v;
    configDirty = true;
    mqtt.publishConfig(config);
    return;
  }

  // ---- CONFIG: default_color ----
  if (topic == "ikea_lamp/config/default_color/set") {
    int r = 0, g = 0, b = 0;
    int c1 = msg.indexOf(',');
    int c2 = msg.indexOf(',', c1 + 1);
    if (c1 > 0 && c2 > c1) {
      r = msg.substring(0, c1).toInt();
      g = msg.substring(c1 + 1, c2).toInt();
      b = msg.substring(c2 + 1).toInt();
      if (r < 0) r = 0; if (r > 255) r = 255;
      if (g < 0) g = 0; if (g > 255) g = 255;
      if (b < 0) b = 0; if (b > 255) b = 255;
      config.defaultColorR = r;
      config.defaultColorG = g;
      config.defaultColorB = b;
      configDirty = true;
      mqtt.publishConfig(config);
    }
    return;
  }

  // ---- CONFIG: sunrise_minutes ----
  if (topic == "ikea_lamp/config/sunrise_minutes/set") {
    int v = msg.toInt();
    if (v < 5) v = 5;
    if (v > 180) v = 180;
    config.sunriseMinutes = (uint16_t)v;
    configDirty = true;
    mqtt.publishConfig(config);
    return;
  }

  // ---- CONFIG: min_pwm ----
  if (topic == "ikea_lamp/config/min_pwm/set") {
    int v = msg.toInt();
    if (v < 0) v = 0;
    if (v > 100) v = 100;
    config.minPwmPercent = (uint8_t)v;
    if (config.maxPwmPercent <= config.minPwmPercent)
      config.maxPwmPercent = config.minPwmPercent + 1;
    configDirty = true;
    mqtt.publishConfig(config);
    return;
  }

  // ---- CONFIG: max_pwm ----
  if (topic == "ikea_lamp/config/max_pwm/set") {
    int v = msg.toInt();
    if (v < 0) v = 0;
    if (v > 100) v = 100;
    config.maxPwmPercent = (uint8_t)v;
    if (config.maxPwmPercent <= config.minPwmPercent)
      config.minPwmPercent = config.maxPwmPercent - 1;
    configDirty = true;
    mqtt.publishConfig(config);
    return;
  }

  // ---- CONFIG: save ----
  if (topic == "ikea_lamp/config/save") {
    if (configDirty) {
      config.save();
      configDirty = false;
    } else {
      Serial.println("[CFG] Save requested but config not dirty – skipping");
    }
    mqtt.publishConfig(config);
    return;
  }

  // ---- CONFIG: reset ----
  if (topic == "ikea_lamp/config/reset") {
    config.reset();
    configDirty = false;
    mqtt.publishConfig(config);
    return;
  }

  // ---- CONFIG: request ----
  if (topic == "ikea_lamp/config/request") {
    mqtt.publishConfig(config);
    return;
  }
}

// ======================= SETUP ==============================

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== IKEA Head Lamp – Modular Firmware ===");

  // Initialize config and state
  config.load();
  
  state.powerOn = false;
  state.brightness = config.defaultBrightness;
  state.colorR = config.defaultColorR;
  state.colorG = config.defaultColorG;
  state.colorB = config.defaultColorB;
  state.sessionId = (uint32_t)esp_random();
  state.version = 1;

  // Initialize hardware
  lamp.begin();
  button.begin();
  statusLED.begin();
  statusLED.startupAnimation();

  // Initialize network
  wifi.setStatusLED(&statusLED);
  wifi.begin();
  
  mqtt.setStatusLED(&statusLED);
  mqtt.begin(handleMqttMessage);

  // Initialize animation engine
  anim.begin(&state, &config);

  // Apply initial state to hardware
  lamp.apply(state.powerOn, state.brightness, 
             state.colorR, state.colorG, state.colorB,
             config.minPwmPercent, config.maxPwmPercent);

  // Publish initial state and config
  mqtt.publishConfig(config);
  mqtt.publishState(state, true);

  Serial.println("[MAIN] Setup complete");
}

// ======================= LOOP ===============================

void loop() {
  // Maintain network connections
  wifi.loop();
  mqtt.loop();

  // Handle button input
  if (button.update() == ButtonEvent::Press) {
    statusLED.blink(1, 50);  // Quick blink on button press
    
    state.togglePower();
    
    if (state.powerOn && state.brightness == 0) {
      state.brightness = config.defaultBrightness;
      state.colorR = config.defaultColorR;
      state.colorG = config.defaultColorG;
      state.colorB = config.defaultColorB;
    }
    
    if (!state.powerOn) {
      anim.stop();
    }
    
    mqtt.publishState(state, true);
  }

  // Update animations
  anim.loop();

  // Apply state to hardware only if changed
  if (lastApplied.hasChanged(state.powerOn, state.brightness,
                              state.colorR, state.colorG, state.colorB,
                              config.minPwmPercent, config.maxPwmPercent)) {
    lamp.apply(state.powerOn, state.brightness,
               state.colorR, state.colorG, state.colorB,
               config.minPwmPercent, config.maxPwmPercent);
    lastApplied.update(state.powerOn, state.brightness,
                       state.colorR, state.colorG, state.colorB,
                       config.minPwmPercent, config.maxPwmPercent);
  }

  // Periodic state publishing
  unsigned long now = millis();
  if (now - lastStatePublish > STATE_PUBLISH_INTERVAL_MS) {
    lastStatePublish = now;
    mqtt.publishState(state, false);
  }
}
