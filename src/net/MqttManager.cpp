#include "MqttManager.h"
#include "mqtt_config.h"
#include "../hw/StatusLED.h"

// Define missing MQTT constants from config
#ifndef MQTT_PASS
#define MQTT_PASS MQTT_PASSWORD
#endif

#ifndef MQTT_CLIENT_ID
#define MQTT_CLIENT_ID "ikea_lamp_esp32"
#endif

// Static member initialization
MqttManager* MqttManager::instance = nullptr;

const char* MqttManager::MQTT_BASE = "ikea_lamp";
const char* MqttManager::TOPIC_CMD_POWER      = "ikea_lamp/cmnd/power";
const char* MqttManager::TOPIC_CMD_BRIGHTNESS = "ikea_lamp/cmnd/brightness";
const char* MqttManager::TOPIC_CMD_COLOR      = "ikea_lamp/cmnd/color";
const char* MqttManager::TOPIC_CMD_ANIMATION  = "ikea_lamp/cmnd/animation";
const char* MqttManager::TOPIC_CMD_PAUSE      = "ikea_lamp/cmnd/pause";
const char* MqttManager::TOPIC_CMD_MODE       = "ikea_lamp/cmnd/mode";
const char* MqttManager::TOPIC_CMD_APPLY_DEFAULTS = "ikea_lamp/cmnd/apply_defaults";
const char* MqttManager::TOPIC_CFG_DEFAULT_BRI   = "ikea_lamp/config/default_brightness/set";
const char* MqttManager::TOPIC_CFG_DEFAULT_COLOR = "ikea_lamp/config/default_color/set";
const char* MqttManager::TOPIC_CFG_SUNRISE_MIN   = "ikea_lamp/config/sunrise_minutes/set";
const char* MqttManager::TOPIC_CFG_MIN_PWM       = "ikea_lamp/config/min_pwm/set";
const char* MqttManager::TOPIC_CFG_MAX_PWM       = "ikea_lamp/config/max_pwm/set";
const char* MqttManager::TOPIC_CFG_SAVE    = "ikea_lamp/config/save";
const char* MqttManager::TOPIC_CFG_RESET   = "ikea_lamp/config/reset";
const char* MqttManager::TOPIC_CFG_REQUEST = "ikea_lamp/config/request";
const char* MqttManager::TOPIC_STATE_JSON  = "ikea_lamp/state/json";
const char* MqttManager::TOPIC_CFG_STATE   = "ikea_lamp/config/state";
const char* MqttManager::TOPIC_DIAGNOSTICS = "ikea_lamp/diagnostics";
const char* MqttManager::TOPIC_HEARTBEAT   = "ikea_lamp/heartbeat";

MqttManager::MqttManager() 
  : client(espClient), statusLED(nullptr), lastReconnectAttempt(0) {
  instance = this;
}

void MqttManager::begin(MessageCallback callback) {
  Serial.println("[MQTT] Initializing MQTT manager");
  messageCallback = callback;
  
  // Keep default buffer size (256 bytes)
  
  // Reduce keepalive to detect connection issues faster
  client.setKeepAlive(15);
  
  // Set socket timeout to prevent long blocking
  client.setSocketTimeout(5);
  
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(mqttCallbackWrapper);
  
  // Don't connect immediately - wait for WiFi to be ready
  // Connection will be attempted in loop()
}

void MqttManager::loop() {
  if (!client.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > RECONNECT_INTERVAL_MS) {
      lastReconnectAttempt = now;
      connectMqtt();
    }
  } else {
    // Throttle client.loop() to reduce WiFi overhead
    static unsigned long lastClientLoop = 0;
    unsigned long now = millis();
    if ((now - lastClientLoop) >= 20) {  // Call max 50 times/sec
      client.loop();
      lastClientLoop = now;
    }
  }
}

bool MqttManager::connected() {
  return client.connected();
}

void MqttManager::setStatusLED(StatusLED* led) {
  statusLED = led;
}

bool MqttManager::connectMqtt() {
  if (client.connected()) return true;
  
  // Don't attempt connection if WiFi isn't ready
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }

  if (statusLED) statusLED->mqttConnecting();
  Serial.printf("[MQTT] Connecting to %s:%u\n", MQTT_HOST, MQTT_PORT);
  
  bool success = false;
  if (strlen(MQTT_USER) > 0 || strlen(MQTT_PASS) > 0) {
    success = client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS);
  } else {
    success = client.connect(MQTT_CLIENT_ID);
  }

  if (!success) {
    Serial.printf("[MQTT] Connection failed, rc=%d\n", client.state());
    if (statusLED) statusLED->mqttFailed();
    return false;
  }

  Serial.println("[MQTT] Connected");
  if (statusLED) statusLED->mqttConnected();
  subscribeToTopics();
  
  return true;
}

void MqttManager::subscribeToTopics() {
  Serial.println("[MQTT] Subscribing to topics");
  
  client.subscribe(TOPIC_CMD_POWER);
  client.subscribe(TOPIC_CMD_BRIGHTNESS);
  client.subscribe(TOPIC_CMD_COLOR);
  client.subscribe(TOPIC_CMD_ANIMATION);
  client.subscribe(TOPIC_CMD_PAUSE);
  client.subscribe(TOPIC_CMD_MODE);
  client.subscribe(TOPIC_CMD_APPLY_DEFAULTS);
  
  client.subscribe(TOPIC_CFG_DEFAULT_BRI);
  client.subscribe(TOPIC_CFG_DEFAULT_COLOR);
  client.subscribe(TOPIC_CFG_SUNRISE_MIN);
  client.subscribe(TOPIC_CFG_MIN_PWM);
  client.subscribe(TOPIC_CFG_MAX_PWM);
  client.subscribe(TOPIC_CFG_SAVE);
  client.subscribe(TOPIC_CFG_RESET);
  client.subscribe(TOPIC_CFG_REQUEST);
}

void MqttManager::publishState(const DeviceState& state, bool retain) {
  if (!client.connected()) return;

  char buf[256];
  
  // Build compact message - include animation params only if animation active
  if (state.mode == LampMode::ANIMATION && state.animationName.length() > 0) {
    snprintf(buf, sizeof(buf),
             "{\"pwr\":%d,\"bri\":%u,\"rgb\":[%u,%u,%u],"
             "\"anim\":\"%s\",\"pause\":%d,\"prog\":%u,"
             "\"dur\":%u,\"final_bri\":%u,\"final_rgb\":[%u,%u,%u],\"end\":\"%s\","
             "\"ver\":%lu}",
             state.powerOn ? 1 : 0,
             state.brightness,
             state.colorR, state.colorG, state.colorB,
             state.animationName.c_str(),
             state.animationPaused ? 1 : 0,
             state.progress,
             state.animDurationMinutes,
             state.animFinalBrightness,
             state.animFinalR, state.animFinalG, state.animFinalB,
             state.animEndBehavior.c_str(),
             (unsigned long)state.version);
  } else {
    // Static mode - show empty animation to indicate no animation running
    snprintf(buf, sizeof(buf),
             "{\"pwr\":%d,\"bri\":%u,\"rgb\":[%u,%u,%u],\"anim\":\"\",\"ver\":%lu}",
             state.powerOn ? 1 : 0,
             state.brightness,
             state.colorR, state.colorG, state.colorB,
             (unsigned long)state.version);
  }

  // Log state publishes to help debug
  Serial.print("[MQTT] Publishing to ");
  Serial.print(TOPIC_STATE_JSON);
  Serial.print(": ");
  Serial.println(buf);
  
  bool success = client.publish(TOPIC_STATE_JSON, buf, retain);
  if (!success) {
    Serial.println("[MQTT] ERROR: Failed to publish state!");
  } else {
    Serial.println("[MQTT] State published successfully");
  }
}

void MqttManager::publishConfig(const DeviceConfig& config) {
  if (!client.connected()) return;

  char buf[256];
  snprintf(buf, sizeof(buf),
           "{\"default_brightness\":%u,"
           "\"default_color\":[%u,%u,%u],"
           "\"sunrise_minutes\":%u,"
           "\"sunrise_final_brightness\":%u,"
           "\"min_pwm\":%u,"
           "\"max_pwm\":%u,"
           "\"version\":%lu}",
           config.defaultBrightness,
           config.defaultColorR, config.defaultColorG, config.defaultColorB,
           config.sunriseMinutes,
           config.sunriseFinalBrightness,
           config.minPwmPercent,
           config.maxPwmPercent,
           (unsigned long)config.version);

  client.publish(TOPIC_CFG_STATE, buf, true);
  // Serial output removed - was blocking loop
}

void MqttManager::publishDiagnostics(unsigned long uptime, uint32_t freeHeap,
                                      uint32_t minHeap, const String& resetReason,
                                      unsigned long loopCount) {
  if (!client.connected()) return;

  char buf[300];
  unsigned long loopsPerSec = (uptime > 0) ? (loopCount / uptime) : 0;
  
  snprintf(buf, sizeof(buf),
           "{\"uptime_s\":%lu,"
           "\"free_heap\":%lu,"
           "\"min_heap\":%lu,"
           "\"reset_reason\":\"%s\","
           "\"loop_count\":%lu,"
           "\"loops_per_sec\":%lu,"
           "\"wifi_rssi\":%d}",
           uptime, (unsigned long)freeHeap, (unsigned long)minHeap,
           resetReason.c_str(), loopCount, loopsPerSec, WiFi.RSSI());

  client.publish(TOPIC_DIAGNOSTICS, buf, false);
  // Serial output removed - was blocking loop and causing watchdog timeouts
}

void MqttManager::publishHeartbeat() {
  if (!client.connected()) return;
  
  char buf[32];
  snprintf(buf, sizeof(buf), "%lu", millis() / 1000);
  client.publish(TOPIC_HEARTBEAT, buf, false);
}

void MqttManager::mqttCallbackWrapper(char* topic, byte* payload, unsigned int length) {
  if (instance && instance->messageCallback) {
    // Use static buffers to avoid heap fragmentation from String objects
    static char topicBuf[64];
    static char msgBuf[256];
    
    // Copy topic
    strncpy(topicBuf, topic, sizeof(topicBuf) - 1);
    topicBuf[sizeof(topicBuf) - 1] = '\0';
    
    // Copy payload
    size_t copyLen = (length < sizeof(msgBuf) - 1) ? length : (sizeof(msgBuf) - 1);
    memcpy(msgBuf, payload, copyLen);
    msgBuf[copyLen] = '\0';
    
    // Trim trailing whitespace
    while (copyLen > 0 && (msgBuf[copyLen-1] == ' ' || msgBuf[copyLen-1] == '\n' || msgBuf[copyLen-1] == '\r')) {
      msgBuf[--copyLen] = '\0';
    }

    // Serial output removed - was blocking loop
    instance->messageCallback(String(topicBuf), String(msgBuf));
  }
}
