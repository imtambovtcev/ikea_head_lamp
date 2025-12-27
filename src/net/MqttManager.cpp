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

MqttManager::MqttManager() 
  : client(espClient), statusLED(nullptr), lastReconnectAttempt(0) {
  instance = this;
}

void MqttManager::begin(MessageCallback callback) {
  Serial.println("[MQTT] Initializing MQTT manager");
  messageCallback = callback;
  
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(mqttCallbackWrapper);
  
  connectMqtt();
}

void MqttManager::loop() {
  if (!client.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > RECONNECT_INTERVAL_MS) {
      lastReconnectAttempt = now;
      connectMqtt();
    }
  } else {
    client.loop();
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

  char buf[320];
  const char* modeStr = (state.mode == LampMode::STATIC) ? "static" : "animation";

  snprintf(buf, sizeof(buf),
           "{\"power\":\"%s\","
           "\"mode\":\"%s\","
           "\"animation\":\"%s\","
           "\"paused\":%s,"
           "\"progress\":%u,"
           "\"brightness\":%u,"
           "\"color\":[%u,%u,%u],"
           "\"session\":%lu,"
           "\"version\":%lu}",
           state.powerOn ? "on" : "off",
           modeStr,
           state.animationName.c_str(),
           state.animationPaused ? "true" : "false",
           state.progress,
           state.brightness,
           state.colorR, state.colorG, state.colorB,
           (unsigned long)state.sessionId,
           (unsigned long)state.version);

  client.publish(TOPIC_STATE_JSON, buf, retain);
  Serial.printf("[MQTT] Published state: %s\n", buf);
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
  Serial.printf("[MQTT] Published config: %s\n", buf);
}

void MqttManager::mqttCallbackWrapper(char* topic, byte* payload, unsigned int length) {
  if (instance && instance->messageCallback) {
    String t = String(topic);
    String msg;
    msg.reserve(length + 1);
    for (unsigned int i = 0; i < length; i++) {
      msg += (char)payload[i];
    }
    msg.trim();

    Serial.printf("[MQTT] RX topic='%s' payload='%s'\n", t.c_str(), msg.c_str());
    instance->messageCallback(t, msg);
  }
}
