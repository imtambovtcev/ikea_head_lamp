#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <functional>
#include "../state/DeviceState.h"
#include "../state/DeviceConfig.h"

class StatusLED;

/**
 * MQTT connection and message routing.
 * 
 * Responsibilities:
 * - Connect and maintain MQTT connection
 * - Subscribe to command topics
 * - Publish state and config
 * - Route messages to callback handler
 */
class MqttManager {
public:
  using MessageCallback = std::function<void(const String& topic, const String& payload)>;

  MqttManager();

  /**
   * Initialize MQTT client and set callback.
   * 
   * @param callback Function to handle incoming MQTT messages
   */
  void begin(MessageCallback callback);

  /**
   * Maintain MQTT connection and process messages.
   * Call in loop().
   */
  void loop();

  /**
   * Publish current device state to MQTT.
   * 
   * @param state Current device state
   * @param retain Whether to retain the message
   */
  void publishState(const DeviceState& state, bool retain = false);

  /**
   * Publish current device configuration to MQTT.
   * 
   * @param config Current device config
   */
  void publishConfig(const DeviceConfig& config);

  /**
   * Publish system diagnostics (uptime, heap, reset reason).
   * 
   * @param uptime Uptime in seconds
   * @param freeHeap Free heap in bytes
   * @param minHeap Minimum heap seen
   * @param resetReason Reset reason string
   * @param loopCount Loop iteration count
   */
  void publishDiagnostics(unsigned long uptime, uint32_t freeHeap, 
                         uint32_t minHeap, const String& resetReason,
                         unsigned long loopCount);

  /**
   * Publish heartbeat (simple alive signal).
   */
  void publishHeartbeat();

  /**
   * Check if MQTT is currently connected.
   */
  bool connected();

  /**
   * Set status LED for visual feedback.
   */
  void setStatusLED(StatusLED* led);

private:
  WiFiClient espClient;
  PubSubClient client;
  MessageCallback messageCallback;
  StatusLED* statusLED;
  unsigned long lastReconnectAttempt;
  static const unsigned long RECONNECT_INTERVAL_MS = 5000;

  // MQTT Topics
  static const char* MQTT_BASE;
  static const char* TOPIC_CMD_POWER;
  static const char* TOPIC_CMD_BRIGHTNESS;
  static const char* TOPIC_CMD_COLOR;
  static const char* TOPIC_CMD_ANIMATION;
  static const char* TOPIC_CMD_PAUSE;
  static const char* TOPIC_CMD_MODE;
  static const char* TOPIC_CMD_APPLY_DEFAULTS;
  static const char* TOPIC_CFG_DEFAULT_BRI;
  static const char* TOPIC_CFG_DEFAULT_COLOR;
  static const char* TOPIC_CFG_SUNRISE_MIN;
  static const char* TOPIC_CFG_MIN_PWM;
  static const char* TOPIC_CFG_MAX_PWM;
  static const char* TOPIC_CFG_SAVE;
  static const char* TOPIC_CFG_RESET;
  static const char* TOPIC_CFG_REQUEST;
  static const char* TOPIC_STATE_JSON;
  static const char* TOPIC_CFG_STATE;
  static const char* TOPIC_DIAGNOSTICS;   // System health info
  static const char* TOPIC_HEARTBEAT;     // Alive signal

  bool connectMqtt();
  void subscribeToTopics();
  static void mqttCallbackWrapper(char* topic, byte* payload, unsigned int length);
  
  static MqttManager* instance;
};

#endif // MQTT_MANAGER_H
