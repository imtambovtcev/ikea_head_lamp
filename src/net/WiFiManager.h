#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>

class StatusLED;

/**
 * WiFi connection manager.
 * 
 * Responsibilities:
 * - Establish and maintain WiFi connection
 * - Auto-reconnect on disconnection
 * - Connection status reporting
 */
class WiFiManager {
public:
  WiFiManager();

  /**
   * Connect to WiFi network.
   * Blocks until connected.
   */
  void begin();

  /**
   * Maintain WiFi connection.
   * Call in loop() - handles reconnection if needed.
   */
  void loop();

  /**
   * Check if WiFi is currently connected.
   */
  bool connected();

  /**
   * Set status LED for visual feedback.
   */
  void setStatusLED(StatusLED* led);

private:
  StatusLED* statusLED;
  bool wasConnected;
  unsigned long lastReconnectAttempt;
  static const unsigned long RECONNECT_INTERVAL_MS = 10000;

  void connect();
};

#endif // WIFI_MANAGER_H
