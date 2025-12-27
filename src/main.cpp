#include <Arduino.h>
#include "esp_system.h"
#include "esp_task_wdt.h"

#include "hw/LampHardware.h"
#include "hw/Button.h"
#include "hw/StatusLED.h"
#include "state/DeviceState.h"
#include "state/DeviceConfig.h"
#include "state/SystemMonitor.h"
#include "net/WiFiManager.h"
#include "net/MqttManager.h"
#include "anim/AnimationEngine.h"

// ======================= MODULE INSTANCES ===================

LampHardware lamp;
Button button;
StatusLED statusLED;
DeviceState state;
DeviceConfig config;
SystemMonitor sysmon;
WiFiManager wifi;
MqttManager mqtt;
AnimationEngine anim;

// ======================= CONFIG FLAGS =======================

bool configDirty = false;

// ======================= PERIODIC PUBLISHING ================

unsigned long lastStatePublish = 0;
const unsigned long STATE_PUBLISH_INTERVAL_MS = 10000;  // Every 10s (reduced to lower MQTT load)

unsigned long lastDiagnosticsPublish = 0;
const unsigned long DIAGNOSTICS_PUBLISH_INTERVAL_MS = 30000;  // Every 30s (reduced from 10s)

unsigned long lastHeartbeat = 0;
const unsigned long HEARTBEAT_INTERVAL_MS = 10000;  // Every 10s (reduced from 5s)

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
  if (topic == "ikea_head_lamp/cmnd/power") {
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
  if (topic == "ikea_head_lamp/cmnd/brightness") {
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
  if (topic == "ikea_head_lamp/cmnd/color") {
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
  if (topic == "ikea_head_lamp/cmnd/mode") {
    if (lower == "static") {
      anim.stop();
      mqtt.publishState(state, true);
    } else if (lower == "animation") {
      anim.startSunrise();
      mqtt.publishState(state, true);
    }
    return;
  }

  // ---- Command: STATE QUERY ----
  if (topic == "ikea_head_lamp/cmnd/query" || topic == "ikea_head_lamp/cmnd/state") {
    mqtt.publishState(state, false);
    return;
  }

  // ---- Command: COLOR TEST ----
  if (topic == "ikea_head_lamp/cmnd/test") {
    if (lower == "color" || lower == "rgb") {
      // Cycle through R→G→B at 70% brightness for 2 seconds each
      state.powerOn = true;
      state.brightness = 70;
      
      // Red
      state.colorR = 255; state.colorG = 0; state.colorB = 0;
      state.bumpVersion();
      mqtt.publishState(state, false);
      delay(2000);
      
      // Green
      state.colorR = 0; state.colorG = 255; state.colorB = 0;
      state.bumpVersion();
      mqtt.publishState(state, false);
      delay(2000);
      
      // Blue
      state.colorR = 0; state.colorG = 0; state.colorB = 255;
      state.bumpVersion();
      mqtt.publishState(state, false);
      delay(2000);
      
      // Back to warm white
      state.colorR = config.defaultColorR;
      state.colorG = config.defaultColorG;
      state.colorB = config.defaultColorB;
      state.bumpVersion();
      mqtt.publishState(state, false);
    }
    return;
  }

  // ---- Command: ANIMATION ----
  if (topic == "ikea_head_lamp/cmnd/animation") {
    // Parse animation command: "sunrise" or "sunrise:duration=1,brightness=80,color=0,100,255"
    int colonIdx = msg.indexOf(':');
    String animName = (colonIdx > 0) ? msg.substring(0, colonIdx) : msg;
    animName.toLowerCase();
    
    if (animName == "sunrise") {
      // Parse optional parameters
      uint8_t duration = 0;  // 0 = use default
      uint8_t brightness = 0;
      uint8_t r = 0, g = 0, b = 0;
      
      if (colonIdx > 0) {
        String params = msg.substring(colonIdx + 1);
        
        // Parse duration
        int durIdx = params.indexOf("duration=");
        if (durIdx >= 0) {
          int durEnd = params.indexOf(',', durIdx);
          if (durEnd < 0) durEnd = params.length();
          duration = params.substring(durIdx + 9, durEnd).toInt();
        }
        
        // Parse brightness
        int briIdx = params.indexOf("brightness=");
        if (briIdx >= 0) {
          int briEnd = params.indexOf(',', briIdx);
          if (briEnd < 0) briEnd = params.length();
          brightness = params.substring(briIdx + 11, briEnd).toInt();
        }
        
        // Parse color
        int colIdx = params.indexOf("color=");
        if (colIdx >= 0) {
          int colStart = colIdx + 6;
          int comma1 = params.indexOf(',', colStart);
          int comma2 = params.indexOf(',', comma1 + 1);
          int colEnd = params.indexOf(',', comma2 + 1);
          if (colEnd < 0) colEnd = params.length();
          
          if (comma1 > colStart && comma2 > comma1) {
            r = params.substring(colStart, comma1).toInt();
            g = params.substring(comma1 + 1, comma2).toInt();
            b = params.substring(comma2 + 1, colEnd).toInt();
          }
        }
      }
      
      anim.startSunrise(duration, brightness, r, g, b);
      mqtt.publishState(state, true);
    } else if (animName == "sunset") {
      // Parse optional parameters
      uint8_t duration = 0;  // 0 = use default
      uint8_t finalBrightness = 0;  // 0 = turn off
      
      if (colonIdx > 0) {
        String params = msg.substring(colonIdx + 1);
        
        // Parse duration
        int durIdx = params.indexOf("duration=");
        if (durIdx >= 0) {
          int durEnd = params.indexOf(',', durIdx);
          if (durEnd < 0) durEnd = params.length();
          duration = params.substring(durIdx + 9, durEnd).toInt();
        }
        
        // Parse final brightness
        int briIdx = params.indexOf("brightness=");
        if (briIdx >= 0) {
          int briEnd = params.indexOf(',', briIdx);
          if (briEnd < 0) briEnd = params.length();
          finalBrightness = params.substring(briIdx + 11, briEnd).toInt();
        }
      }
      
      anim.startSunset(duration, finalBrightness);
      mqtt.publishState(state, true);
    } else if (animName == "rainbow") {
      anim.startRainbow();
      mqtt.publishState(state, true);
    } else if (animName == "fire") {
      // Parse optional parameters
      uint8_t intensity = 70;
      uint8_t speed = 5;
      
      if (colonIdx > 0) {
        String params = msg.substring(colonIdx + 1);
        
        // Parse intensity
        int intIdx = params.indexOf("intensity=");
        if (intIdx >= 0) {
          int intEnd = params.indexOf(',', intIdx);
          if (intEnd < 0) intEnd = params.length();
          intensity = params.substring(intIdx + 10, intEnd).toInt();
        }
        
        // Parse speed
        int spdIdx = params.indexOf("speed=");
        if (spdIdx >= 0) {
          int spdEnd = params.indexOf(',', spdIdx);
          if (spdEnd < 0) spdEnd = params.length();
          speed = params.substring(spdIdx + 6, spdEnd).toInt();
        }
      }
      
      anim.startFire(intensity, speed);
      mqtt.publishState(state, true);
    } else if (animName == "breathe") {
      // Parse optional parameters
      uint8_t cycleDuration = 4;
      uint8_t maxBrightness = 70;
      uint8_t minBrightness = 10;
      uint8_t r = 0, g = 0, b = 0;
      
      if (colonIdx > 0) {
        String params = msg.substring(colonIdx + 1);
        
        // Parse duration (cycle duration in seconds)
        int durIdx = params.indexOf("duration=");
        if (durIdx >= 0) {
          int durEnd = params.indexOf(',', durIdx);
          if (durEnd < 0) durEnd = params.length();
          cycleDuration = params.substring(durIdx + 9, durEnd).toInt();
        }
        
        // Parse max brightness
        int briIdx = params.indexOf("brightness=");
        if (briIdx >= 0) {
          int briEnd = params.indexOf(',', briIdx);
          if (briEnd < 0) briEnd = params.length();
          maxBrightness = params.substring(briIdx + 11, briEnd).toInt();
        }
        
        // Parse min brightness
        int minIdx = params.indexOf("min_brightness=");
        if (minIdx >= 0) {
          int minEnd = params.indexOf(',', minIdx);
          if (minEnd < 0) minEnd = params.length();
          minBrightness = params.substring(minIdx + 15, minEnd).toInt();
        }
        
        // Parse color
        int colIdx = params.indexOf("color=");
        if (colIdx >= 0) {
          int colStart = colIdx + 6;
          int comma1 = params.indexOf(',', colStart);
          int comma2 = params.indexOf(',', comma1 + 1);
          int colEnd = params.indexOf(',', comma2 + 1);
          if (colEnd < 0) colEnd = params.length();
          
          if (comma1 > colStart && comma2 > comma1) {
            r = params.substring(colStart, comma1).toInt();
            g = params.substring(comma1 + 1, comma2).toInt();
            b = params.substring(comma2 + 1, colEnd).toInt();
          }
        }
      }
      
      anim.startBreathe(cycleDuration, maxBrightness, minBrightness, r, g, b);
      mqtt.publishState(state, true);
    } else if (animName == "ocean") {
      // Parse optional parameters
      uint8_t speed = 5;
      uint8_t brightness = 70;
      
      if (colonIdx > 0) {
        String params = msg.substring(colonIdx + 1);
        
        // Parse speed
        int spdIdx = params.indexOf("speed=");
        if (spdIdx >= 0) {
          int spdEnd = params.indexOf(',', spdIdx);
          if (spdEnd < 0) spdEnd = params.length();
          speed = params.substring(spdIdx + 6, spdEnd).toInt();
        }
        
        // Parse brightness
        int briIdx = params.indexOf("brightness=");
        if (briIdx >= 0) {
          int briEnd = params.indexOf(',', briIdx);
          if (briEnd < 0) briEnd = params.length();
          brightness = params.substring(briIdx + 11, briEnd).toInt();
        }
      }
      
      anim.startOcean(speed, brightness);
      mqtt.publishState(state, true);
    } else if (animName == "stop") {
      anim.stop();
      mqtt.publishState(state, true);
    } else {
      Serial.println("[CMD] Unknown animation");
    }
    return;
  }

  // ---- Command: PAUSE ----
  if (topic == "ikea_head_lamp/cmnd/pause") {
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
  if (topic == "ikea_head_lamp/cmnd/apply_defaults") {
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
  if (topic == "ikea_head_lamp/config/default_brightness/set") {
    int v = msg.toInt();
    if (v < 1) v = 1;
    if (v > 100) v = 100;
    config.defaultBrightness = (uint8_t)v;
    configDirty = true;
    mqtt.publishConfig(config);
    return;
  }

  // ---- CONFIG: default_color ----
  if (topic == "ikea_head_lamp/config/default_color/set") {
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
  if (topic == "ikea_head_lamp/config/sunrise_minutes/set") {
    int v = msg.toInt();
    if (v < 5) v = 5;
    if (v > 180) v = 180;
    config.sunriseMinutes = (uint16_t)v;
    configDirty = true;
    mqtt.publishConfig(config);
    return;
  }

  // ---- CONFIG: min_pwm ----
  if (topic == "ikea_head_lamp/config/min_pwm/set") {
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
  if (topic == "ikea_head_lamp/config/max_pwm/set") {
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
  if (topic == "ikea_head_lamp/config/save") {
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
  if (topic == "ikea_head_lamp/config/reset") {
    config.reset();
    configDirty = false;
    mqtt.publishConfig(config);
    return;
  }

  // ---- CONFIG: request ----
  if (topic == "ikea_head_lamp/config/request") {
    mqtt.publishConfig(config);
    return;
  }
}

// ======================= SETUP ==============================

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== IKEA Head Lamp – Modular Firmware ===");

  // Initialize system monitor
  sysmon.begin();

  // Enable watchdog (30 second timeout - increased for WiFi/MQTT blocking)
  esp_task_wdt_init(30, true);
  esp_task_wdt_add(NULL);
  Serial.println("[SYS] Watchdog enabled (30s timeout)");

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

  // Initial MQTT publishes will happen in loop() once connected
  
  Serial.println("[MAIN] Setup complete");
}

// ======================= MAIN LOOP ===============

void loop() {
  esp_task_wdt_reset();

  // Update system monitor
  sysmon.incrementLoop();
  sysmon.update();

  // Maintain network connections
  wifi.loop();
  mqtt.loop();
  
  // Publish initial state/config once after first MQTT connection
  static bool initialPublishDone = false;
  if (!initialPublishDone && mqtt.connected()) {
    mqtt.publishConfig(config);
    mqtt.publishState(state, true);
    initialPublishDone = true;
  }

  // Handle button input
  ButtonEvent btnEvent = button.update();
  
  if (btnEvent == ButtonEvent::Press) {
    statusLED.blink(1, 50);  // Quick blink on button press
    
    // Single click: Toggle power (turn on to static default color OR turn off)
    state.togglePower();
    
    if (state.powerOn) {
      // Turn on: Set to static mode with default settings
      anim.stop();
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
  
  if (btnEvent == ButtonEvent::LongPress) {
    statusLED.blink(2, 50);  // Double blink on long press
    
    // Long press: Toggle pause/play current animation
    anim.togglePause();
    
    mqtt.publishState(state, true);
  }
  
  if (btnEvent == ButtonEvent::DoublePress) {
    statusLED.blink(3, 50);  // Triple blink on double-press
    
    // Double click: Start rainbow animation
    anim.startRainbow();
    
    mqtt.publishState(state, true);
  }

  // Update animations
  anim.loop();

  // Apply state to hardware only if changed (throttled to ~30 FPS max)
  static unsigned long lastHardwareUpdate = 0;
  unsigned long now = millis();
  
  if ((now - lastHardwareUpdate) >= 33) {  // Max 30 updates/sec
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
    lastHardwareUpdate = now;
  }

  // Periodic state publishing (only if state changed since last publish)
  static uint32_t lastPublishedVersion = 0;
  if (now - lastStatePublish > STATE_PUBLISH_INTERVAL_MS) {
    lastStatePublish = now;
    if (state.version != lastPublishedVersion) {
      mqtt.publishState(state, false);
      lastPublishedVersion = state.version;
    }
  }

  // Periodic heartbeat (so you know it's alive)
  if (now - lastHeartbeat > HEARTBEAT_INTERVAL_MS) {
    lastHeartbeat = now;
    mqtt.publishHeartbeat();
  }

  // Periodic diagnostics
  if (now - lastDiagnosticsPublish > DIAGNOSTICS_PUBLISH_INTERVAL_MS) {
    lastDiagnosticsPublish = now;
    
    // Calculate loops per second
    static unsigned long lastLoopCount = 0;
    unsigned long currentLoopCount = sysmon.getLoopCount();
    unsigned long loopsPerSec = (currentLoopCount - lastLoopCount) / (DIAGNOSTICS_PUBLISH_INTERVAL_MS / 1000);
    lastLoopCount = currentLoopCount;
    
    // Diagnostics published to MQTT - no serial output needed
    // (was blocking and causing the slow loop it was trying to warn about!)
    
    mqtt.publishDiagnostics(sysmon.getUptimeSeconds(), sysmon.getFreeHeap(),
                            sysmon.getMinFreeHeap(), sysmon.getResetReason(),
                            sysmon.getLoopCount());
  }
  
  // Small delay to reduce CPU load and heat (allows WiFi to use light sleep)
  delay(1);  // 1ms delay = ~1000 loops/sec max (still plenty responsive)
}
