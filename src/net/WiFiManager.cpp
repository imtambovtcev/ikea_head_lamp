#include "WiFiManager.h"
#include "wifi_config.h"
#include "../hw/StatusLED.h"

WiFiManager::WiFiManager() 
  : statusLED(nullptr), wasConnected(false), lastReconnectAttempt(0) {
}

void WiFiManager::begin() {
  Serial.println("[WIFI] Starting WiFi manager");
  WiFi.mode(WIFI_STA);
  connect();
}

void WiFiManager::loop() {
  bool isConnected = (WiFi.status() == WL_CONNECTED);

  if (!isConnected && wasConnected) {
    Serial.println("[WIFI] Connection lost");
    if (statusLED) statusLED->wifiFailed();
    wasConnected = false;
  }

  if (!isConnected) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > RECONNECT_INTERVAL_MS) {
      lastReconnectAttempt = now;
      if (statusLED) statusLED->wifiConnecting();
      // Serial output removed - was blocking loop
      WiFi.reconnect();  // Non-blocking
    }
  } else if (!wasConnected) {
    Serial.printf("[WIFI] Connected. IP=%s\n", WiFi.localIP().toString().c_str());
    if (statusLED) statusLED->wifiConnected();
    wasConnected = true;
  }
}

bool WiFiManager::connected() {
  return WiFi.status() == WL_CONNECTED;
}

void WiFiManager::setStatusLED(StatusLED* led) {
  statusLED = led;
}

void WiFiManager::connect() {
  Serial.printf("[WIFI] Connecting to %s\n", WIFI_SSID);
  
  // Enable light sleep for power savings while maintaining connectivity
  WiFi.setSleep(WIFI_PS_MIN_MODEM);  // Light sleep between DTIM beacons
  WiFi.setAutoReconnect(true);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Non-blocking connection - check in loop()
  // Just initiate, don't wait here
  Serial.println("[WIFI] Connection initiated (non-blocking)");
}
